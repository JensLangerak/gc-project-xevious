#ifndef ENEMY_ENTITY_H
#define ENEMY_ENTITY_H
#include "entity.h"

class EnemyEntity : public Entity
{
public:
	EnemyEntity();
	EnemyEntity(glm::vec2 pos);

    // ============= Gameplay related ============
	void update(double tick, Gamestate* state);
	void onCollision(Entity* entity);
	EntityType type = EntityType::Enemy;
	glm::vec3 direction;
};

#endif