#include "entity.h"
#include "boss_entity.h"
#include "models.h"
#include "mesh_simplification.h"
#include "utils.h"

#include <iostream>

BossEntity::BossEntity()
{
	// @NOTE: Using the david model
	// @TODO: Constructing the mesh simplification here is bad, should do it somewhere else!!!
	//level1 = new MeshSimplification(models::david.vertices, 20);
	scale = 0.2;
	radius = 2.;
	centerOffsets[0] = glm::vec3(1., 0., 0.) * radius;
	centerOffsets[1] = glm::vec3(-1., 0., 0.) * radius;
	centerOffsets[2] = glm::vec3(0., 0., 1.) * radius;
	centerOffsets[3] = glm::vec3(0., 0., -1.) * radius;

	position = glm::vec3(0. ,0., -1.5);

	// @TODO: Fix to retrieve bounding cube for correct model (generated simple model)
	retrieveBoundingCube(models::dragon);

	std::cout << "Got here\n";
	//std::cout << "num of vertices in simplified mesh" << level1->simplifiedMesh.size() << "\n"; 
}

// @TODO: Add selection for which head
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
		BoundingBox box = boundingCube.getProjectedBoundingBox(getHeadTransformMatrix(i));
		if (box.checkIntersection(entity->getProjectedBoundingBox()))
		{
			std::cout << "Hit the boss!\n";
			return true;
		}
	}
	return false;
}

void BossEntity::onCollision(Entity* entity)
{
	for (int i = 0; i < 4; ++i)
	{
		BoundingBox box = boundingCube.getProjectedBoundingBox(getHeadTransformMatrix(i));
		if (box.checkIntersection(entity->getProjectedBoundingBox()))
		{
			lives[i] -= 1;
			// @TODO: Fix
			models[i] = models::ModelType::StarEnemy;
		}
	}
}

void BossEntity::update(double tick, Gamestate* state)
{
	float angularVelocity = 0.001;
	orientation.y += angularVelocity * tick;

	accTime += tick;
	position.x = sin(accTime * 0.001) * 0.7;
	
	// First move towards player
	if (stage == 0)
	{
		// move into the stage
		position.z += 0.0007 * tick;

		if (position.z >= -.5)
		{
			stage = 1;
		}
	} else if (stage == 1)
	{
		// Then start attacking / moving
	}
}

void BossEntity::draw(long tick, glm::mat4 projView)
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

			//level1->drawGrid(projView);
			// glUseProgram(globals::mainProgram);
			// glBindVertexArray(level1->vao);
			// glDrawArrays(GL_TRIANGLES, 0, level1->simplifiedMesh.size());

			// Draw default entity at location for now
			models::drawModel(models[i]);
		}
	}
}