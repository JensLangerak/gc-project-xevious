#include "entity.h"
#include "boss_entity.h"
#include "models.h"
#include "mesh_simplification.h"
#include "utils.h"
#include "bullet_entity.h"
#include "player_entity.h"

#include <iostream>
#include <utility>


#define BULLET_DELAY 0.5

BossEntity::BossEntity()
{
	scale = 0.4;
	radius = 2.;
	centerOffsets[0] = glm::vec3(1., 0., 0.) * radius;
	centerOffsets[1] = glm::vec3(-1., 0., 0.) * radius;
	centerOffsets[2] = glm::vec3(0., 0., 1.) * radius;
	centerOffsets[3] = glm::vec3(0., 0., -1.) * radius;

	position = glm::vec3(0. ,0., -1.5);

	// @TODO: Fix to retrieve bounding cube for correct model (generated simple model)
	retrieveBoundingCube(models::dragon);
}

// @NOTE: Can be optimized
glm::mat4 BossEntity::getHeadTransformMatrix(int i)
{
	// Offset before rotation
	glm::mat4 prerotationMatrix = getTranslationMatrix(centerOffsets[i]);
	glm::mat4 rotationMatrix = getRotationMatrix(orientation.x, orientation.y, orientation.z);
	glm::mat4 scalingMatrix = getScalingMatrix(scale);
	glm::mat4 translationMatrix = getTranslationMatrix(position);

	return translationMatrix * scalingMatrix * rotationMatrix * prerotationMatrix;
}

// @TODO(Dirty): This is ugly, wasted computations
bool BossEntity::checkCollision(Entity* entity)
{
	// check collision for all four entities
	for (int i = 0; i < 4; ++i)
	{
		if (lives[i] > 0)
		{
			BoundingBox box = boundingCube.getProjectedBoundingBox(getHeadTransformMatrix(i));
			if (box.checkIntersection(entity->getProjectedBoundingBox()))
			{
				std::cout << "Hit the boss!\n";
				return true;
			}	
		}
	}
	return false;
}

void BossEntity::onCollision(Entity* entity)
{
	for (int i = 0; i < 4; ++i)
	{
		BoundingBox box = boundingCube.getProjectedBoundingBox(getHeadTransformMatrix(i));
		if (box.checkIntersection(entity->getProjectedBoundingBox()) && lives[i] > 0)
		{
			lives[i] -= 1;
			switch (lives[i])
			{
				case 3:
					models[i] = models::ModelType::BossDetailLevel1;
				break;
				case 2:
					models[i] = models::ModelType::BossDetailLevel2;
				break; 
				case 1:
					models[i] = models::ModelType::BossDetailLevel3;
				// Mark grey red 
				break;
				case 0:
				default:
				break;
			}

			totalLives -= 1;
			if (totalLives == 0)
			{
				isAlive = false;
				isCollidable = 0;
			}
		}

	}
}

// bubbleSort by z value
struct LocIndexPair
{
	glm::vec4 location;
	int index;
};

void sort(LocIndexPair (&list)[4], int count)
{
	for (int i = 0; i < count; ++i)
	{
		for (int j = i; j < count; ++j)
		{
			if (list[j].location.z > list[i].location.z)
			{
				std::swap(list[i], list[j]);
			}
		}
	}
}


void BossEntity::update(double tick, Gamestate* state )
{
	float angularVelocity = 3.14/2;
	orientation.y += angularVelocity * tick;

	accTime += tick;
	position.x = sin(accTime * 3.14/3) * 0.7;
	
	// First move towards player
	if (stage == 0)
	{
		// move into the stage
		position.z += 1. * tick;

		if (position.z >= -.5)
		{
			stage = 1;
		}
	} else if (stage == 1)
	{
		// Then start attacking / moving
	}


	// @TODO: Implement bullets
	if (isAlive && bulletStormActive && bulletCountdown <= 0)
	{
		// transform * centerOffset  = location
		LocIndexPair positions[4];
		for (int i = 0; i < 4; ++i)
		{
			positions[i].location = getRotationMatrix(0, orientation.y, 0) * glm::vec4(centerOffsets[i], 1.0);
			positions[i].index = i;
		}
		sort(positions, 4);
		
		for (int i = 0; i < 4; ++i)
		{
			if (lives[positions[i].index] > 0)
			{
				// Shoot from here
				glm::vec4 bulletLocation = getHeadTransformMatrix(i) * glm::vec4(position, 1.0);
				// in the direction of the player (for now)
				glm::vec3 bulletLoc3 = glm::vec3(bulletLocation.x, bulletLocation.y, bulletLocation.z);
				glm::vec3 bulletDir = state->player->position - bulletLoc3;
				BulletEntity* bullet = new BulletEntity(bulletLoc3, bulletDir);
				state->entityList->push_back(bullet);
				// @TODO: Mark bullet as intended for player!!!
				break;
			}
		}

		bulletCountdown = BULLET_DELAY;
	} else {
		bulletCountdown -= tick;
	}

	// each x seconds, send a bullet from the foremost (living) head towards the player
	
	// sort locations by z value (higher is more in front)
	// for each
	// if alive -> shoot bullet towards player


	// @TODO: Implement death animation and potential win screen
}

void BossEntity::draw(long tick , glm::mat4 projView)
{
	// get mvp matrix by calculating m matrix
	// Get transformation matrix
	for (int i = 0; i < 4; ++i)
	{
		if (lives[i] > 0)
		{
			// @TODO: Decide rendering level on number of lives left
			glm::mat4 tMatrix = getHeadTransformMatrix(i);
			glm::mat4 mvp = projView * tMatrix;

			glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
			glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "model"), 1, GL_FALSE, glm::value_ptr(projView));
			glm::vec3 color = glm::vec3(1., 0., 0.);
			glUniform3fv(glGetUniformLocation(globals::mainProgram, "color"), 1, glm::value_ptr(color));

			// Draw default entity at location for now
			models::drawModel(models[i]);
		}
	}
}

// @TODO: implement drawBoundingCube();
// @TODO: fix collision such that on collision with player, the boss becomes invisible for a second or two
// @TODO: Make movement more unpredictable