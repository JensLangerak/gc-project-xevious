#include <iostream>
#include "player_entity.h"
#include "enemy_entity.h"
#include "utils.h"


EnemyEntity::EnemyEntity()
{
}

EnemyEntity::EnemyEntity(glm::vec2 pos)
{
	// @TODO: Should the ground position be height of zero?
	position = glm::vec3(pos.x, 0., pos.y);
	model = models::ModelType::StarEnemy;
    color = glm::vec3(0., 1., 1.);
    scale = 0.05;
    boundingCube = models::makeBoundingCube(models::starEnemy.vertices);
}

void EnemyEntity::update(double tick, Gamestate* state)
{
	// @NOTE: What height should the ground be?
	// While above height alpha, spiral every tick by angular velocity, and decrease height

	// IF: NONDEAD
	if (isAlive)
	{
		float MOVEMENT_SPEED = 0.005;
		// Move in the direction of player
		glm::vec3 playerPos = (state->player)->position;
		glm::vec3 direction = glm::normalize(playerPos - position); 
		position += direction * MOVEMENT_SPEED;

		// @TODO: perhaps modify movement with a sine wave, to make behaviour less obvious
		// @TODO: alternatively, modify movement with random function
	}
	else if (!isAlive)
	{
		// (Spiral to the ground)
		double angular_velocity = 0.005;
		double falling_velocity = 0.0005;

		orientation.y += angular_velocity * tick;
		position.y -= falling_velocity * tick;

		if (position.y < -1.0)
		{
			canBeRemoved = true;

			// Change color to grey to distinguish from alive enemies;
			color = glm::vec3(.7, .7, .7);
			std::cout << "Enemy died!!\n";
		}		
	}
}

// @TODO: Implement collision
// OnCollision: Mark as dead
void EnemyEntity::onCollision(Entity* entity)
{
	if (entity->type != EntityType::Enemy)
	{
		isAlive = false;
		isCollidable = false;	
	}
}