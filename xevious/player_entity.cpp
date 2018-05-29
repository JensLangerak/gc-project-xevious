#include "entity.h"
#include "entity.h"
#include "player_entity.h"
#include "models.h"
#include "utils.h"
#include "bounding_box.h"
#include "bullet_entity.h"

#define PI 3.14

// @NOTE: Default constructor to set up gamestate conveniently. Probably a bad idea
PlayerEntity::PlayerEntity() : Entity(glm::vec3(1., 0., 0.))
{
	// -------------- Rendering & Testing section ------

	// @NOTE: Super constructor is automatically called
	retrieveBoundingCube(models::playerShip);
	shipModel = models::ModelType::PlayerShip;
	weaponModel = models::ModelType::PlayerGun;
	shipScale = 0.05;
	weaponScale = 0.05;
	weaponAngle = 0;
	// @TODO: Tweak until looks right;
	// @NOTE: Also needs to be transformed into world space
	relativeLocWeapon = glm::vec3(0., 1.0, 0.);
	type = EntityType::Player;




	// -------------- Gameplay section ------------------
	lives = 20;
}

void PlayerEntity::performAction(PlayerAction action, Gamestate* state)
{
	// @TODO: Do bounds checking for player location to prevent moving outside of screen
	// @TODO: Change movement_amount to be dependent on tick (move actual action to update function)
	float movement_amount = 0.05;
	switch (action)
	{
		case PlayerAction::MOVE_FORWARD:
			movePlayer(0., -movement_amount);
		break;
		case PlayerAction::MOVE_BACKWARD:
			movePlayer(0., movement_amount);
		break;
		case PlayerAction::MOVE_LEFT:
			position.x -= movement_amount;
			movePlayer(-movement_amount, 0.);
		break;
		case PlayerAction::MOVE_RIGHT:
			movePlayer(movement_amount, 0.);
		break;
		case PlayerAction::ROLL:
			// @TODO: Implement "Start animation which is carried out in update"
		break;
		case PlayerAction::SHOOT:
			// Spawn new entity and set direction
			{
				glm::vec3 pos = state->player->position;
				float angle = state->player->weaponAngle;
				// @TODO: Fix the math here
				glm::vec3 dir = glm::vec3(sin(angle), 0., -cos(angle));
				BulletEntity* bullet = new BulletEntity(pos, dir);

				state->bulletList->push_back(bullet);
			}
		break;
		default:
		break;		
	}
}

float clamp(float val, float low, float high)
{
	if (val > high)
	{
		return high;
	}
	else if (val < low)
	{
		return low;
	}
	return val;
}

void PlayerEntity::movePlayer(float x, float y)
{
	position.x = clamp(position.x + x, -1., 1.);
	position.z = clamp(position.z + y, -1., .5);
}

glm::mat4 PlayerEntity::getShipTransform()
{
	glm::mat4 shipTransform = getRotationMatrix(orientation[0], orientation[1], orientation[2]);
	shipTransform[3] = glm::vec4(position, 1.);
	return shipTransform * getScalingMatrix(shipScale);
}

glm::mat4 PlayerEntity::getWeaponTransform()
{
	// @NOTE: Can be optimized by algebraically multiplying matrices
	// Translation matrix
	glm::vec4 relativeTranslation = getScalingMatrix(weaponScale) * glm::vec4(relativeLocWeapon, 0.);
	glm::vec4 vecTranslation = glm::vec4(position, 1.0) + relativeTranslation;
	glm::mat4 translation = glm::mat4();
	translation[0][0] = 1.;
	translation[1][1] = 1.;
	translation[2][2] = 1.;
	translation[3] = vecTranslation;

	// Horizontal rotation (gun orientation) matrix
	glm::mat4 rotation = glm::mat4();
	rotation[0][0] = cos(weaponAngle);
	rotation[2][0] = sin(weaponAngle);
	rotation[1][1] = 1.;
	rotation[2][0] = -sin(weaponAngle);
	rotation[2][2] = cos(weaponAngle);
	rotation[3][3] = 1.;

	return translation * getScalingMatrix(weaponScale) * rotation;
}

void PlayerEntity::update(double tick __attribute__((unused)), Gamestate* state)
{
	if (lives == 0)
	{
		state->mode = GameMode::Dead;
	}
}

void PlayerEntity::onCollision(Entity* entity __attribute__((unused)))
{
	// Decrease life by 1
	lives -= 1;
	std::cout << "Player has " << lives << " left\n"; 
}

void PlayerEntity::draw(long tick __attribute__((unused)), glm::mat4 projView)
{
	// 1. Draw the ship
	// Create Transformation matrices
	glm::mat4 shipTransform = getShipTransform();
	glm::mat4 shipMVP = projView * shipTransform;
	glm::mat4 shipModelMatrix = shipTransform;

	// Load transform into GPU
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(shipMVP));
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "model"), 1, GL_FALSE, glm::value_ptr(shipModelMatrix));
    
    glUniform3fv(glGetUniformLocation(globals::mainProgram, "color"), 1, glm::value_ptr(color));

	// Issue draw call with model
    // models::activateTexture(texture);
    models::drawModel(shipModel);

    // 2. Draw the cannon
    // Create Transformation matrices
    glm::mat4 weaponTransform = getWeaponTransform();
    glm::mat4 weaponMVP = projView * weaponTransform;
    glm::mat4 weaponModelMatrix = weaponTransform;

	// Load transform into GPU
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(weaponMVP));
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "model"), 1, GL_FALSE, glm::value_ptr(weaponModelMatrix));
    
    glUniform3fv(glGetUniformLocation(globals::mainProgram, "color"), 1, glm::value_ptr(color));

	// Issue draw call with model
    // models::activateTexture(texture);
    models::drawModel(weaponModel);
}

glm::vec4 PlayerEntity::getScreenPosition(glm::mat4 projView)
{
	return projView * getShipTransform() * glm::vec4(position, 1.);
}

void PlayerEntity::drawBoundingCube(glm::mat4 projView, glm::vec3 drawColor)
{
	boundingCube.draw(projView * getShipTransform(), drawColor);
}

BoundingBox PlayerEntity::getProjectedBoundingBox()
{
	return boundingCube.getProjectedBoundingBox(getShipTransform());
}