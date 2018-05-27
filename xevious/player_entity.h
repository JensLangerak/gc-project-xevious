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
	void draw(long tick, glm::mat4 projView);
	void drawBoundingCube(glm::mat4 projView, glm::vec3 drawColor);

	void update(double tick, Gamestate* state);
	void onCollision(Entity* entity);
	
	void performAction(PlayerAction action);
	BoundingBox getProjectedBoundingBox();

	// @NOTE: Temporarily for testing only
	float weaponAngle;

	EntityType type = EntityType::Player;

	glm::vec4 getScreenPosition(glm::mat4 projView);
protected:
	// ============= Rendering related ============
	models::ModelType shipModel;
	models::ModelType weaponModel;

	float shipScale;
	float weaponScale;

	glm::vec3 relativeLocWeapon;

	glm::mat4 getShipTransform();
	glm::mat4 getWeaponTransform();

	// ============= Gameplay related =============
	int lives; // Number of lives that the player has before dying

};

#endif