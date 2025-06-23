#pragma once

#include "includes.h"
#include <C:/Users/User/Documents/code/.cpp/glm/glm.hpp>



enum GateType {
	G_INVALID,		// N/A
	G_PASSTHROUGH,	// =
	G_AND,			// &
	G_OR, 			// |
	G_NOT,			// ~
	G_XOR,			// ^
	G_LATCH,		// 2 inputs, turns on with input A and off with input B.
	G_PULSE,		// 1 input, turns on for 1 frame of the input, then off after.
	G_TOGGLE		// 1 input, turns on and off with that input.
};

enum ItemType {
	I_INVALID,
	I_HEALTH_SMALL,
	I_HEALTH_LARGE,
	I_ENERGY_SMALL,
	I_ENERGY_LARGE,
};

enum EnemyType {
	E_INVALID,
	E_MELEE,
	E_RANGED,
};

enum ProjectileType {
	P_INVALID,
	P_BALL,
	P_ROCKET
};



inline const std::unordered_map<std::string, int> keyNameToGLFW = {
	//Main Keys
	{"KEY_SPACE", GLFW_KEY_SPACE},
	{"KEY_APOSTROPHE", GLFW_KEY_APOSTROPHE},
	{"KEY_COMMA", GLFW_KEY_COMMA},
	{"KEY_MINUS", GLFW_KEY_MINUS},
	{"KEY_PERIOD", GLFW_KEY_PERIOD},
	{"KEY_SLASH", GLFW_KEY_SLASH},
	{"KEY_0", GLFW_KEY_0},
	{"KEY_1", GLFW_KEY_1},
	{"KEY_2", GLFW_KEY_2},
	{"KEY_3", GLFW_KEY_3},
	{"KEY_4", GLFW_KEY_4},
	{"KEY_5", GLFW_KEY_5},
	{"KEY_6", GLFW_KEY_6},
	{"KEY_7", GLFW_KEY_7},
	{"KEY_8", GLFW_KEY_8},
	{"KEY_9", GLFW_KEY_9},
	{"KEY_SEMICOLON", GLFW_KEY_SEMICOLON},
	{"KEY_EQUAL", GLFW_KEY_EQUAL},
	{"KEY_A", GLFW_KEY_A},
	{"KEY_B", GLFW_KEY_B},
	{"KEY_C", GLFW_KEY_C},
	{"KEY_D", GLFW_KEY_D},
	{"KEY_E", GLFW_KEY_E},
	{"KEY_F", GLFW_KEY_F},
	{"KEY_G", GLFW_KEY_G},
	{"KEY_H", GLFW_KEY_H},
	{"KEY_I", GLFW_KEY_I},
	{"KEY_J", GLFW_KEY_J},
	{"KEY_K", GLFW_KEY_K},
	{"KEY_L", GLFW_KEY_L},
	{"KEY_M", GLFW_KEY_M},
	{"KEY_N", GLFW_KEY_N},
	{"KEY_O", GLFW_KEY_O},
	{"KEY_P", GLFW_KEY_P},
	{"KEY_Q", GLFW_KEY_Q},
	{"KEY_R", GLFW_KEY_R},
	{"KEY_S", GLFW_KEY_S},
	{"KEY_T", GLFW_KEY_T},
	{"KEY_U", GLFW_KEY_U},
	{"KEY_V", GLFW_KEY_V},
	{"KEY_W", GLFW_KEY_W},
	{"KEY_X", GLFW_KEY_X},
	{"KEY_Y", GLFW_KEY_Y},
	{"KEY_Z", GLFW_KEY_Z},
	{"KEY_LEFT_BRACKET", GLFW_KEY_LEFT_BRACKET},
	{"KEY_BACKSLASH", GLFW_KEY_BACKSLASH},
	{"KEY_RIGHT_BRACKET", GLFW_KEY_RIGHT_BRACKET},
	{"KEY_GRAVE_ACCENT", GLFW_KEY_GRAVE_ACCENT},
	{"KEY_WORLD_1", GLFW_KEY_WORLD_1},
	{"KEY_WORLD_2", GLFW_KEY_WORLD_2},


	//Functional Keys
	{"KEY_ESCAPE", GLFW_KEY_ESCAPE},
	{"KEY_ENTER", GLFW_KEY_ENTER},
	{"KEY_TAB", GLFW_KEY_TAB},
	{"KEY_BACKSPACE", GLFW_KEY_BACKSPACE},
	{"KEY_INSERT", GLFW_KEY_INSERT},
	{"KEY_DELETE", GLFW_KEY_DELETE},
	{"KEY_RIGHT", GLFW_KEY_RIGHT},
	{"KEY_LEFT", GLFW_KEY_LEFT},
	{"KEY_DOWN", GLFW_KEY_DOWN},
	{"KEY_UP", GLFW_KEY_UP},
	{"KEY_PAGE_UP", GLFW_KEY_PAGE_UP},
	{"KEY_PAGE_DOWN", GLFW_KEY_PAGE_DOWN},
	{"KEY_HOME", GLFW_KEY_HOME},
	{"KEY_END", GLFW_KEY_END},
	{"KEY_CAPS_LOCK", GLFW_KEY_CAPS_LOCK},
	{"KEY_SCROLL_LOCK", GLFW_KEY_SCROLL_LOCK},
	{"KEY_NUM_LOCK", GLFW_KEY_NUM_LOCK},
	{"KEY_PRINT_SCREEN", GLFW_KEY_PRINT_SCREEN},
	{"KEY_PAUSE", GLFW_KEY_PAUSE},
	{"KEY_F1", GLFW_KEY_F1},
	{"KEY_F2", GLFW_KEY_F2},
	{"KEY_F3", GLFW_KEY_F3},
	{"KEY_F4", GLFW_KEY_F4},
	{"KEY_F5", GLFW_KEY_F5},
	{"KEY_F6", GLFW_KEY_F6},
	{"KEY_F7", GLFW_KEY_F7},
	{"KEY_F8", GLFW_KEY_F8},
	{"KEY_F9", GLFW_KEY_F9},
	{"KEY_F10", GLFW_KEY_F10},
	{"KEY_F11", GLFW_KEY_F11},
	{"KEY_F12", GLFW_KEY_F12},


	//Keypad Keys
	{"KEY_KP_0", GLFW_KEY_KP_0},
	{"KEY_KP_1", GLFW_KEY_KP_1},
	{"KEY_KP_2", GLFW_KEY_KP_2},
	{"KEY_KP_3", GLFW_KEY_KP_3},
	{"KEY_KP_4", GLFW_KEY_KP_4},
	{"KEY_KP_5", GLFW_KEY_KP_5},
	{"KEY_KP_6", GLFW_KEY_KP_6},
	{"KEY_KP_7", GLFW_KEY_KP_7},
	{"KEY_KP_8", GLFW_KEY_KP_8},
	{"KEY_KP_9", GLFW_KEY_KP_9},
	{"KEY_KP_DECIMAL", GLFW_KEY_KP_DECIMAL},
	{"KEY_KP_DIVIDE", GLFW_KEY_KP_DIVIDE},
	{"KEY_KP_MULTIPLY", GLFW_KEY_KP_MULTIPLY},
	{"KEY_KP_SUBTRACT", GLFW_KEY_KP_SUBTRACT},
	{"KEY_KP_ADD", GLFW_KEY_KP_ADD},
	{"KEY_KP_ENTER", GLFW_KEY_KP_ENTER},
	{"KEY_KP_EQUAL", GLFW_KEY_KP_EQUAL},


	//Modifier Keys
	{"KEY_LEFT_SHIFT", GLFW_KEY_LEFT_SHIFT},
	{"KEY_LEFT_CONTROL", GLFW_KEY_LEFT_CONTROL},
	{"KEY_LEFT_ALT", GLFW_KEY_LEFT_ALT},
	{"KEY_LEFT_SUPER", GLFW_KEY_LEFT_SUPER},
	{"KEY_RIGHT_SHIFT", GLFW_KEY_RIGHT_SHIFT},
	{"KEY_RIGHT_CONTROL", GLFW_KEY_RIGHT_CONTROL},
	{"KEY_RIGHT_ALT", GLFW_KEY_RIGHT_ALT},
	{"KEY_RIGHT_SUPER", GLFW_KEY_RIGHT_SUPER},
	{"KEY_MENU", GLFW_KEY_MENU}
};




namespace constants {
	static int C_TRUE = 1;
	static int C_FALSE = 0;


	//Mathematical Constants
	constexpr float PI = 3.14159265358979f;
	constexpr float EXP = 2.71828182845905f;

	constexpr float TO_RAD = 0.01745329251994f;
	constexpr float TO_DEG = 57.2957795130824f;

	constexpr float EPSILON = 1e-4f;



	//Sim Constants
	constexpr float PHYSICS_FREQUENCY = 45.0f;
	constexpr float FLOOR_FRICT_COEFF = 0.75f;
	constexpr float AIR_FRICT_COEFF = 0.975f;


	//Invalid returns for vectors and floats.
	constexpr float INVALID = 1e30f;
	constexpr glm::vec2 INVALIDv2 = glm::vec2(INVALID, INVALID);
	constexpr glm::vec3 INVALIDv3 = glm::vec3(INVALID, INVALID, INVALID);
	constexpr glm::vec4 INVALIDv4 = glm::vec4(INVALID, INVALID, INVALID, INVALID);


	//Maximum quantities of each type.
	constexpr int MAX_FLAGS = 256;
}

namespace display {
	//Resolutions
	constexpr glm::ivec2 INITIAL_SCREEN_RESOLUTION = glm::ivec2(960, 540);


	//Texture Standardisation
	constexpr glm::ivec2 TEXTURE_RESOLUTION = glm::ivec2(128, 128);
	constexpr int TEXTURE_ARRAY_MAX_LAYERS = 64;
	constexpr const char* FALLBACK_TEXTURE_PATH = "src/textures-env/fallback-general.png";
}

namespace initial {
	//Textures
	constexpr const char* FALLBACK_TEXTURE_NAME = "fallback-general";

	//Player
	constexpr glm::vec3 PLAYER_START_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
	constexpr glm::vec2 PLAYER_START_VANGLE = glm::vec2(0.0f, 0.0f);

	//Physics
	constexpr float PHYS_GRAVITY = 0.486f;
	constexpr float PHYS_KPZ = -16.0f;
}

namespace playerConfig {
	//Physics speed values
	constexpr float MOVE_SPEED_BASE = 0.05f;
	constexpr float MOVE_SPEED_CROUCH_MULT = 0.5f;
	constexpr float MOVE_SPEED_RUN_MULT = 2.0f;
	constexpr float JUMP_INIT_SPEED = 0.25f;
	constexpr float MAX_AIR_SPEED_XY = MOVE_SPEED_BASE * MOVE_SPEED_RUN_MULT * 2.5f;


	//Physics Collision Values
	constexpr glm::vec2 PLAYER_COLLISION_RECT = glm::vec2(0.25f, 0.25f);
	constexpr float PLAYER_COLLISION_HEIGHT_STAND = 1.75f;
	constexpr float PLAYER_COLLISION_HEIGHT_CROUCH = 1.0f;
	constexpr float PLAYER_INTERACT_RAY_DIST = 2.0f;


	//Player Initial Values
	constexpr int PLAYER_MAX_HEALTH = 128;
	constexpr int PLAYER_MAX_ENERGY = 64;

	constexpr int HEAL_SMALL = 32;
	constexpr int HEAL_LARGE = 96;
	constexpr int ENERGY_SMALL = 16;
	constexpr int ENERGY_LARGE = 32;
}

namespace dev {
	//Assorted DEV/DEBUG constants
	constexpr bool SHOW_PHYSICS_TICKRATE = false;
	constexpr bool SHOW_PHYSICS_DT = false;
}