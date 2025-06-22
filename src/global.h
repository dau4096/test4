#pragma once
#include "includes.h"
#include "constants.h"

inline std::unordered_map<std::string, int> userBindings = {
	{"MOVE_FORWARD", -1},
	{"MOVE_BACKWARD", -1},
	{"MOVE_LEFT", -1},
	{"MOVE_RIGHT", -1},
	{"MOVE_JUMP", -1},
	{"MOVE_CROUCH", -1},
	{"MOVE_SPRINT", -1},

	{"USE_INTERACT", -1},
	{"USE_HEADLAMP", -1},
	{"USE_VIEWZOOM", -1},

	{"META_SCREENSHOT", -1},
	{"META_RELOAD_STAGE", -1},
	{"META_RELOAD_ENV", -1},
	{"META_EXIT", -1},
	{"META_FREECURSOR", -1},
};


inline std::unordered_map<std::string, bool> keyMap = []() {
	std::unordered_map<std::string, bool> tmp;
	for (const auto& pair : userBindings) {
		tmp[pair.first] = false;
	}
	return tmp;
}();


inline std::unordered_map<std::string, std::string> userConfig = {
	{"TURN_SPEED_MOUSE", ""},
	{"TURN_SPEED_KEYBOARD", ""},

	{"VIEW_VSYNC", ""},
	{"VIEW_MAX_FREQ", ""},
	{"VIEW_FOV", ""},

	{"META_DEBUG_MODE", ""},
	{"META_SHOW_FRAMERATE_CONSOLE", ""},
	{"META_SHOW_DT_CONSOLE", ""},
	{"META_SHOW_CONSOLE", ""},
	{"META_STAGE_NAME", ""}
};


struct StageData {
	std::string name;
	std::string filePath;

	glm::vec3 playerStartPoint;
	glm::vec2 playerStartAngle;

	float gravity;


	StageData()
		: name("<NONE>"), filePath(""),
		  playerStartPoint(0.0f, 0.0f, 0.0f), playerStartAngle(0.0f, 0.0f), gravity(0.486f) {}
};

inline StageData stageData;


//Graphics
inline glm::ivec2 currentWindowResolution;
inline float verticalFOV;


//Other
inline float framerate;
inline float tickrate;
inline size_t frame;
inline size_t tick;