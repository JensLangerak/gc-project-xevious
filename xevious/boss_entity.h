#ifndef BOSS_ENTITY_H
#define BOSS_ENTITY_H
#include "entity.h"
#include "mesh_simplification.h"
#include "models.h"

class BossEntity : public Entity
{
public:
	BossEntity();
	void draw(long tick, glm::mat4 projView);
	void update(double tick, Gamestate* state);

	glm::mat4 getHeadTransformMatrix(int i );
	bool checkCollision(Entity* entity);
	void onCollision(Entity* entity);

	glm::vec3 centerOffsets[4];
	int lives[4] 				= {4, 4, 4, 4};
	models::ModelType models[4] = {models::ModelType::Dragon, models::ModelType::Dragon, models::ModelType::Dragon, models::ModelType::Dragon};
	glm::vec3 colors[4];
	float radius = 1.0;
	float accTime = .0;
	int stage = 0;

	// @NOTE: Let's just do one mesh at first!
	// MeshSimplification* level1;
};

#endif