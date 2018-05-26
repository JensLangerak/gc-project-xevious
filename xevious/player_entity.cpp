#include "player_entity.h"

void PlayerEntity::performAction(PlayerAction action)
{
	// @TODO: Do bounds checking for player location to prevent moving outside of screen
	// @TODO: Change movement_amount to be dependent on tick (move actual action to update function)
	float movement_amount = 0.05;
	switch (action)
	{
		case PlayerAction::MOVE_FORWARD:
			position.z -= movement_amount;
		break;
		case PlayerAction::MOVE_BACKWARD:
			position.z += movement_amount;
		break;
		case PlayerAction::MOVE_LEFT:
			position.x -= movement_amount;
		break;
		case PlayerAction::MOVE_RIGHT:
			position.x += movement_amount;
		break;
		case PlayerAction::ROLL:
			// @TODO: Implement "Start animation which is carried out in update"
		break;
		case PlayerAction::SHOOT:
			// @TODO: Implement
		break;
		default:
		break;		
	}
}