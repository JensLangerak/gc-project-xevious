#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

#include "entity.h"

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
	//void move(MovementDirection direction);
	void performAction(PlayerAction action);
protected:
	int lives; // Number of lives that the player has before dying
	
};

#endif