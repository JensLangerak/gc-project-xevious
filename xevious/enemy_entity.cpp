#include <iostream>
#include "entity.h"
#include "player_entity.h"
#include "enemy_entity.h"
#include "utils.h"

#define PI 3.14
#define ENEMY_SCALE 0.05
#define FALLING_SCALING_FACTOR 0.7

EnemyEntity::EnemyEntity()
{
}

EnemyEntity::EnemyEntity(glm::vec2 pos)
{
	position = glm::vec3(pos.x, 0., pos.y);
	model = models::ModelType::StarEnemy;
    color = glm::vec3(0., 1., 1.);
    scale = ENEMY_SCALE;
    boundingCube = models::makeBoundingCube(models::starEnemy.vertices);
	
	bbCenterOffset = 0.5f * boundingCube.dimensions;
    type = EntityType::Enemy;
}

void EnemyEntity::update(double tick, Gamestate* state)
{
	// While above height alpha, spiral every tick by angular velocity, and decrease height
	float MOVEMENT_SPEED = 0.005;

	if (lives > 0)
	{
		// Move in the direction of player
		glm::vec3 playerPos = (state->player)->position;
		direction = glm::normalize(playerPos - position); 	
		position += direction * MOVEMENT_SPEED;
	}
	else if (lives <= 0)
	{
		// (Spiral to the ground)
		color = glm::vec3(0.6, 0.6, 0.6);
		double angular_velocity = PI;
		double falling_velocity = 1.;

		orientation.y += angular_velocity * tick;
		position.y -= falling_velocity * tick;

		position += direction * MOVEMENT_SPEED;
		scale -= (scale * FALLING_SCALING_FACTOR * tick);

		if (position.y < -1.0)
		{
			canBeRemoved = true;
			// Change color to grey to distinguish from alive enemies;
			color = glm::vec3(.7, .7, .7);
			//std::cout << "Enemy died!!\n";
		}		
	}
}

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
	}
}