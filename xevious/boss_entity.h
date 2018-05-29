#ifndef BOSS_ENTITY_H
#define BOSS_ENTITY_H
#include "entity.h"
#include "mesh_simplification.h"
#include "models.h"

class BossEntity : public Entity
{
public:
	BossEntity();
	// ============= Rendering related ============
	void draw(long tick, glm::mat4 projView);

	// ============= Gameplay related ============
	void update(double tick, Gamestate* state);
	bool checkCollision(Entity* entity);
	void onCollision(Entity* entity);
private:
	// ============= Rendering related ============
	glm::mat4 getHeadTransformMatrix(int i );
	models::ModelType models[4] = {models::ModelType::Dragon, models::ModelType::Dragon, models::ModelType::Dragon, models::ModelType::Dragon};

	// ============= Gameplay related ============
	float radius = 1.0;
	float accTime = .0;
	int stage = 0;
	glm::vec3 centerOffsets[4];
	int lives[4] 				= {4, 4, 4, 4};
	int totalLives = 16;			// 4 lives * 4 heads
	glm::vec3 colors[4];	// @TODO: Unused right now
	EntityType type = EntityType::Enemy;


	float bulletCountdown = 2.0;
	bool bulletStormActive = true;
};

#endif