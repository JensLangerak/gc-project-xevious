#ifndef ENEMY_ENTITY_H
#define ENEMY_ENTITY_H
#include "entity.h"

class EnemyEntity : public Entity
{
public:
	// @NOTE: Do these need to be overridden?
	// void draw(long tick, glm::mat4 projView);
	// void drawBoundingCube(glm::mat4 projView, glm::vec3 drawColor);
	EnemyEntity();
	EnemyEntity(glm::vec2 pos);

	void update(double tick, Gamestate* state);
	void onCollision(Entity* entity);

	EntityType type = EntityType::Enemy;
	glm::vec3 direction;
};

#endif