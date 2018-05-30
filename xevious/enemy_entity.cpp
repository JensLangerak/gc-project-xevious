#include <iostream>
#include "entity.h"
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
	
	bbCenterOffset = 0.5f * boundingCube.dimensions;
    type = EntityType::Enemy;
}

void EnemyEntity::update(double tick, Gamestate* state)
{
	Entity::update(tick, state);
	// @NOTE: What height should the ground be?
	// While above height alpha, spiral every tick by angular velocity, and decrease height
	float MOVEMENT_SPEED = 0.005;

	// IF: NONDEAD
	if (lives > 0)
	{
		// Move in the direction of player
		glm::vec3 playerPos = (state->player)->position;
		direction = glm::normalize(playerPos - position); 	
		position += direction * MOVEMENT_SPEED;

		// @TODO: perhaps modify movement with a sine wave, to make behaviour less obvious
		// @TODO: alternatively, modify movement with random function
	}
	else if (lives <= 0)
	{
		// (Spiral to the ground)
		color = glm::vec3(0.6, 0.6, 0.6);
		double angular_velocity = 3.14;
		double falling_velocity = 1.;

		orientation.y += angular_velocity * tick;
		position.y -= falling_velocity * tick;

		position += direction * MOVEMENT_SPEED;
		scale -= (scale * 0.7 * tick);

		if (position.y < -1.0)
		{
			canBeRemoved = true;
			// Change color to grey to distinguish from alive enemies;
			color = glm::vec3(.7, .7, .7);
			//std::cout << "Enemy died!!\n";
		}		
	}

	// @TODO: mark for deletion once out of screen
}

// @TODO: Implement collision
// OnCollision: Mark as dead
void EnemyEntity::onCollision(Entity* entity)
{
	if (entity->type != EntityType::Enemy)
	{
		lives -= 1;
		if (lives == 0)
		{
			isAlive = false;
			isCollidable = false;
		}
		Entity::activateFlash();	
	}
}