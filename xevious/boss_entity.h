#ifndef BOSS_ENTITY_H
#define BOSS_ENTITY_H
#include "entity.h"
#include "mesh_simplification.h"
#include "models.h"

enum class BossState
{
	Entering,
	Shooting,
	Dying
};

class BossEntity : public Entity
{
public:
	BossEntity();
	// ============= Rendering related ============
	void draw(long tick, glm::mat4 projView);
	void drawBoundingCube(glm::mat4 projView, glm::vec3 drawColor);

	// ============= Gameplay related ============
	void update(double tick, Gamestate* state);
	bool checkCollision(Entity* entity);
	void onCollision(Entity* entity);
private:
	// ============= Rendering related ============
	models::ModelType moonModel;
	models::ModelType planetModel;

	glm::mat4 getPlanetMatrix(int i);
	glm::mat4 getMoonSubMatrix(int i);

	// ============= Gameplay related ============
	EntityType type = EntityType::Enemy;

	float planetVelocity = 0.4;
	float moonVelocity = 0.8;

	float planetSize = 0.3;
	float moonSize = 0.2;

	float moonAngles[2];
	float planetAngles[2];

	float planetOffset = 0.6;
	float moonOffset = 0.3;

	BossState currentState = BossState::Entering;
	float stateRunningTime = 0.;

	// @TODO: Reimplement
	int moonLives[2];
	int planetLives[2];

	int totalLives = 16;			// 4 lives * 4 heads
	float bulletCountdown = 2.0;
	bool bulletStormActive = true;
};

#endif