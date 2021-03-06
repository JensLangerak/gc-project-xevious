#include "entity.h"
#include "boss_entity.h"
#include "models.h"
#include "mesh_simplification.h"
#include "utils.h"
#include "bullet_entity.h"
#include "player_entity.h"

#include <iostream>
#include <utility>

#define BULLET_DELAY 1.5

#define PI 3.14

BossEntity::BossEntity()
{
	for (int i = 0; i < NUMBER_OF_PLANETS; ++i)
	{
		planetModel[i] = models::ModelType::Dragon;
		moonModel[i] = models::ModelType::Dragon;
		// Set initial angles
		planetAngles[i] = PI * i;
		moonAngles[i] = PI/2 * i;

		// Set initial lives
		planetLives[i] = 4;
		moonLives[i] = 4;
	}

	position = glm::vec3(0. ,0., -1.5);
	// position = glm::vec3(0. ,0., 0);
	// @TODO: Fix to retrieve bounding cube for correct model (generated simple model)
	retrieveBoundingCube(models::dragon);
}


// @NOTE: Perhaps a better way to do this would have been to keep a subtree of entities, and defer rendering / collision detection
// 			Buuuuut that's not how Entity::draw() is architected right now, so we're gonna have to keep it like this.
//			Would have prevented us from manually having to overwrite drawDebug() (which is kind of errorprone in itself)
bool BossEntity::checkCollision(Entity* entity) 
{
	BoundingBox enemyBox = entity->getProjectedBoundingBox();
	for (int i = 0; i < NUMBER_OF_PLANETS; ++i)
	{
		glm::mat4 planetMatrix = getPlanetMatrix(i);

		// Planets
		BoundingBox collisionBox = boundingCube.getProjectedBoundingBox(planetMatrix * getScalingMatrix(planetSize));
		if (planetLives[i] > 0 && collisionBox.checkIntersection(enemyBox))
		{
			return true;
		}

		// Moons
		collisionBox = boundingCube.getProjectedBoundingBox(planetMatrix * getMoonSubMatrix(i) * getScalingMatrix(moonSize));
		if (moonLives[i] > 0 && collisionBox.checkIntersection(enemyBox))
		{
			return true;
		}
	}
	return false;
}

// @NOTE(Bad): It's super inefficient to have to check for collisions again here
//				Could have been solved by simply storing the pointer to the collision entity if we kept a subtree
void BossEntity::onCollision(Entity* entity) 
{
	// Note: will only be triggered if collision exists
	BoundingBox enemyBox = entity->getProjectedBoundingBox();
	for (int i = 0; i < NUMBER_OF_PLANETS; ++i)
	{
		glm::mat4 planetMatrix = getPlanetMatrix(i);

		// Planets
		BoundingBox collisionBox = boundingCube.getProjectedBoundingBox(planetMatrix * getScalingMatrix(planetSize));

		if (collisionBox.checkIntersection(enemyBox))
		{
			if (moonLives[i] <= 0)
			{
				planetLives[i] -= 1;
				if (planetLives[i] == 3)
				{
                    planetModel[i]  =  models::ModelType::BossDetailLevel1;
				} else if (planetLives[i] == 2)
				{
					planetModel[i] =  models::ModelType::BossDetailLevel2;
				} else if (planetLives[i] == 1)
				{
                    planetModel[i]  =  models::ModelType::BossDetailLevel3;
				}
			}
		}

		// Moons
		collisionBox = boundingCube.getProjectedBoundingBox(planetMatrix * getMoonSubMatrix(i) * getScalingMatrix(moonSize));
		if (collisionBox.checkIntersection(enemyBox))
		{
			moonLives[i] -= 1;
			if (moonLives[i] == 3)
			{
				moonModel[i] =  models::ModelType::BossDetailLevel1;
			} else if (moonLives[i] == 2)
			{
                moonModel[i] =  models::ModelType::BossDetailLevel2;
			} else if (moonLives[i] == 1)
			{
                moonModel[i] =  models::ModelType::BossDetailLevel3;
			}
		}
	}
	totalLives -= 1;

	if (totalLives == 0)
	{
		std::cout << "Moving into dying state\n";
		currentState = BossState::Dying; 
	}
}

void BossEntity::drawBoundingCube(glm::mat4 projView, glm::vec3 drawColor)
{
	for (int i = 0; i < NUMBER_OF_PLANETS; ++i)
	{
		glm::mat4 planetMatrix = getPlanetMatrix(i);
		if (planetLives[i] > 0)
		{
			boundingCube.draw(projView *planetMatrix * getScalingMatrix(planetSize), drawColor);	
		}
		if (moonLives[i] > 0)
		{
			boundingCube.draw(projView * planetMatrix * getMoonSubMatrix(i) * getScalingMatrix(moonSize), drawColor);
		}
	}
}

glm::mat4 BossEntity::getPlanetMatrix(int i)
{
	glm::mat4 translation = getTranslationMatrix(glm::vec3(planetOffset, 0, 0));
	glm::mat4 rotation = getRotationMatrix(0, planetAngles[i], 0);
	glm::mat4 globalTranslation = getTranslationMatrix(position);
	return globalTranslation * rotation * translation;
}

glm::mat4 BossEntity::getMoonSubMatrix(int i)
{
	glm::mat4 translation = getTranslationMatrix(glm::vec3(moonOffset, 0, 0));
	glm::mat4 rotation = getRotationMatrix(0, moonAngles[i], 0);
	return rotation * translation;
}


void BossEntity::update(double tick, Gamestate* state)
{
	if (currentState == BossState::Entering)
	{
		position.z += 1.0 * tick;
		if (position.z >= -0.5)
		{

			currentState = BossState::Shooting;
		}
	}
	if (currentState == BossState::Shooting)
	{

        bulletCountdown -= tick;
	    bool shoot = bulletCountdown <= 0.f;
        if (shoot) {
            nextShooter++;
            nextShooter = nextShooter % NUMBER_OF_PLANETS;

            bulletCountdown = BULLET_DELAY;
        }
		for (int i = 0; i < NUMBER_OF_PLANETS; ++i)
		{
			moonAngles[i] += moonVelocity * tick;
			planetAngles[i] += planetVelocity * tick;

			if (planetLives[i] > 0 && shoot && i == nextShooter) {
                glm::vec3 pos =glm::vec3( getPlanetMatrix(i) * getScalingMatrix(planetSize) * glm::vec4(position, 1.0));
				glm::vec3 dir =state->player->position - pos;
                BulletEntity* bullet = new BulletEntity(pos, dir);
                bullet->texture = models::Textures::Beam2;
                bullet->orientation = glm::vec3(0, std::atan2(dir.x, dir.z), 0);

                state->entityList->push_back(bullet);

			}



		}	
	} else if (currentState == BossState::Dying)
	{
		// @TODO: How to die?
	}
}

void BossEntity::draw(long tick, glm::mat4 projView)
{
	for (int i = 0; i < NUMBER_OF_PLANETS; ++i)
	{
		glm::mat4 mvp;

		// @set color
		glm::vec3 color = glm::vec3(1., 0., 0.);
		glUniform3fv(glGetUniformLocation(globals::mainProgram, "color"), 1, glm::value_ptr(color));

		// Draw planet and moon
		glm::mat4 planetMatrix = getPlanetMatrix(i);
		
		if (planetLives[i] > 0)
		{
			mvp = projView * planetMatrix * getScalingMatrix(planetSize);

			glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
			glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "model"), 1, GL_FALSE, glm::value_ptr(projView));

			models::drawModel(planetModel[i]);
		}

		if (moonLives[i] > 0)
		{	
			// doing the transformation "stack" by hand here
			glm::mat4 moonMatrix = planetMatrix * getMoonSubMatrix(i);
			mvp = projView * moonMatrix * getScalingMatrix(moonSize);

			glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
			glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "model"), 1, GL_FALSE, glm::value_ptr(projView));

			models::drawModel(moonModel[i]);
		}
	}
}
