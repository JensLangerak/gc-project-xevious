#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

#include "entity.h"
#include "models.h" 
#include "utils.h"

// @NOTE: Perhaps add: Bomb
enum class PlayerAction
{
	MOVE_FORWARD,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_BACKWARD,
	ROLL,
	SHOOT
};

class PlayerEntity : public Entity 
{
public:
	PlayerEntity();
    // ============= Rendering related ============
	void draw(long tick, glm::mat4 projView);
	void drawBoundingCube(glm::mat4 projView, glm::vec3 drawColor);

    // ============= Gameplay related ============
	void update(double tick, Gamestate* state);
	void onCollision(Entity* entity);
	void performAction(PlayerAction action, Gamestate* state);
	BoundingBox getProjectedBoundingBox();

	// @NOTE: Temporarily for testing only
	glm::vec4 getScreenPosition(glm::mat4 projView);

	float weaponAngle;
protected:
	// ============= Rendering related ============
	glm::mat4 getShipTransform();
	glm::mat4 getWeaponTransform();

	models::ModelType shipModel;
	models::ModelType weaponModel;
	glm::vec3 relativeLocWeapon;
	float shipScale;
	float weaponScale;

	// ============= Gameplay related =============
	void movePlayer(float x, float y);
	int lives; // Number of lives that the player has before dying
	float weaponCooldown;
	bool weaponReady;
};

#endif