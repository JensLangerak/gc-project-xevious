#include "entity.h"
#include "entity.h"
#include "player_entity.h"
#include "models.h"
#include "utils.h"
#include "bounding_box.h"
#include "bullet_entity.h"

#define PI 3.14

#define WEAPON_COOLDOWN 0.3

// @NOTE: Default constructor to set up gamestate conveniently. Probably a bad idea
PlayerEntity::PlayerEntity() : Entity(glm::vec3(1., 0., 0.))
{
	// -------------- Rendering & Testing section ------
	retrieveBoundingCube(models::playerShip);
	shipModel = models::ModelType::PlayerShip;
	weaponModel = models::ModelType::PlayerGun;
	shipScale = 0.1;
	weaponScale = 0.1;
	weaponAngle = 0;
	// @NOTE: Also needs to be transformed into world space
	relativeLocWeapon = glm::vec3(0., 1.0, 0.);
	type = EntityType::Player;

	// -------------- Gameplay section ------------------
	lives = 20;
	weaponCooldown = WEAPON_COOLDOWN;
	weaponReady = true;
}

void PlayerEntity::performAction(PlayerAction action, Gamestate* state)
{
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
			if (weaponReady)
			{
				glm::vec3 pos = state->player->position;
				float angle = state->player->weaponAngle;
				
				glm::vec3 dir = glm::normalize(glm::vec3(sin(angle), 0., -cos(angle)));
				BulletEntity* bullet = new BulletEntity(pos, dir);

				state->bulletList->push_back(bullet);
				weaponReady = false;
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
	position.z = clamp(position.z + y, -.9, 1.);
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
	glm::mat4 rotation = getRotationMatrix(0, -weaponAngle, 0);

	return translation * getScalingMatrix(weaponScale) * rotation;
}

void PlayerEntity::update(double tick , Gamestate* state)
{
	if (lives == 0)
	{
		state->mode = GameMode::Dead;
	}

	if (!weaponReady && weaponCooldown >= 0)
	{
		weaponCooldown -= tick;
	} else if (!weaponReady)
	{
		weaponCooldown = WEAPON_COOLDOWN;
		weaponReady = true;
	}
}

void PlayerEntity::onCollision(Entity* entity )
{
	// Decrease life by 1
	lives -= 1;
	std::cout << "Player has " << lives << " left\n"; 
}

void PlayerEntity::draw(long tick , glm::mat4 projView)
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