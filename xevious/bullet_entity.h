#ifndef BULLET_ENTITY_H
#define BULLET_ENTITY_H

#include "entity.h"
#include "utils.h"

class BulletEntity : public Entity
{
public:
	BulletEntity(glm::vec3 pos, glm::vec3 dir);
	~BulletEntity();

	// ============= Gameplay related ============
	void update(double tick, Gamestate* state);
	void onCollision(Entity* entity);
	EntityType type = EntityType::Bullet;
private:
	glm::vec3 direction;

	bool hasCollided;
	float flashRemaining;
	glm::vec3* lightPos;
	glm::vec3 lightPosOffset;
};

#endif