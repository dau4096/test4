#include "includes.h"
#include "global.h"
#include "utils.h"
using namespace std;
using namespace utils;


bool prevJump = false;

namespace physics {

void playerMove(
		utils::Player *player
	) {

	Player playerCopy = *player;

	float playerSpeed = playerConfig::MOVE_SPEED_BASE;
	float maxV = playerConfig::MOVE_SPEED_BASE;

	if (keyMap["MOVE_CROUCH"]) {
		playerSpeed *= playerConfig::MOVE_SPEED_CROUCH_MULT;
		maxV = playerConfig::MOVE_SPEED_BASE * playerConfig::MOVE_SPEED_CROUCH_MULT;
	} else if (keyMap["MOVE_SPRINT"]) {
		playerSpeed *= playerConfig::MOVE_SPEED_RUN_MULT;
		maxV = playerConfig::MOVE_SPEED_BASE * playerConfig::MOVE_SPEED_RUN_MULT;
	}

	playerSpeed = glm::clamp(maxV / playerSpeed, 0.0f, maxV);

	// Determine the movement vector based on key presses
	float reduction = 1.0f;
	glm::vec2 vAddition = glm::vec2(0.0f, 0.0f);
	float vaR = player->viewAngle.x * constants::TO_RAD;
	if (keyMap["MOVE_FORWARD"]) {
		if (!player->touchingFloor) {reduction *= 0.5f;}
		vAddition.x += playerSpeed * sin(vaR) * reduction;
		vAddition.y += playerSpeed * cos(vaR) * reduction;
	}
	if (keyMap["MOVE_BACKWARD"]) {
		if (!player->touchingFloor) {reduction *= 0.5f;}
		vAddition.x -= playerSpeed * sin(vaR) * reduction;
		vAddition.y -= playerSpeed * cos(vaR) * reduction;
	}
	if (keyMap["MOVE_LEFT"]) {
		if (!player->touchingFloor) {reduction *= 0.5f;}
		vAddition.x -= playerSpeed * cos(vaR) * reduction;
		vAddition.y -= playerSpeed * -sin(vaR) * reduction;
	}
	if (keyMap["MOVE_RIGHT"]) {
		if (!player->touchingFloor) {reduction *= 0.5f;}
		vAddition.x += playerSpeed * cos(vaR) * reduction;
		vAddition.y += playerSpeed * -sin(vaR) * reduction;
	}
	if (keyMap["MOVE_JUMP"] && !prevJump) {
		if (player->touchingFloor) {
			player->velocity.z += playerConfig::JUMP_INIT_SPEED;
			player->position.z += 0.025;
			prevJump = true;
		}
	} else {
		prevJump = false;
	}


	if (length(vAddition) > playerSpeed) {
		vAddition = normalize(vAddition) * playerSpeed;
	}
	glm::vec2 vRight = glm::normalize(glm::vec2(player->velocity.y, -player->velocity.x));


	glm::vec2 curVelocity = glm::vec2(player->velocity.x, player->velocity.y);
	if (glm::length(curVelocity) > playerConfig::MAX_AIR_SPEED_XY && glm::length(vAddition) > constants::EPSILON) {
		vAddition = vRight * glm::dot(glm::normalize(vAddition), vRight) * playerSpeed;
	}
	player->velocity.x += vAddition.x; player->velocity.y += vAddition.y;


	//Apply friction.
	if (player->touchingFloor) {
		player->velocity.x *= constants::FLOOR_FRICT_COEFF;
		player->velocity.y *= constants::FLOOR_FRICT_COEFF;
	} else {
		player->velocity *= constants::AIR_FRICT_COEFF;
	}

	player->velocity.z -= stageData.gravity / constants::PHYSICS_FREQUENCY;
	player->position += player->velocity;

	if (isVec3NaN(player->position) || isVec3NaN(player->velocity)) {
		*player = playerCopy; //Revert back.
	}
};



}