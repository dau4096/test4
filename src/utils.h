#ifndef UTILS_H
#define UTILS_H

#include "includes.h"
#include "global.h"
#include "constants.h"
#include <vector>
#include <stdexcept>
#include <C:/Users/User/Documents/code/.cpp/glm/glm.hpp>

using namespace std;


namespace logicFunctions {
	static void LGF_AND(int* A, int* B, int* Q, int* internalState) {*Q = (*A) & (*B);}
	static void LGF_OR(int* A, int* B, int* Q, int* internalState) {*Q = (*A) | (*B);}
	static void LGF_NOT(int* A, int* B, int* Q, int* internalState) {*Q = ~(*A);}
	static void LGF_XOR(int* A, int* B, int* Q, int* internalState) {*Q = (*A) ^ (*B);}

	static void LGF_LATCH(int* A, int* B, int* Q, int* internalState) { //Swap between 1 and 0 with A and B.
		if (((*A) & (*B)) > 0) {
			//internalState remains unchanged; both inputs counteract each other's change.
		} else if ((*A) > 0) {
			*internalState = 1;
		} else if ((*B) > 0) {
			*internalState = 0;
		}
		*Q = *internalState;
	}

	static void LGF_PULSE(int* A, int* B, int* Q, int* internalState) { //If A is 1, return 1 for a single frame.
		if (((*internalState) < 1) && ((*A) == 1)) {*Q = 1;}
		else {*Q = 0;}
		*internalState = *A;
	}

	static void LGF_TOGGLE(int* A, int* B, int* Q, int* internalState) { //Toggles between 1 and 0 if A is 1.
		if ((*A) == 1) {
			*internalState = ((*internalState) < 1) ? 1 : 0;
		}
		*Q = *internalState;
	}

	static void LGF_PASSTHROUGH(int* A, int* B, int* Q, int* internalState) {*Q = *A;}
}


//Utility functions
namespace utils {

	static inline void hideConsole() {
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	}
	static inline void showConsole() {
		ShowWindow(GetConsoleWindow(), SW_SHOW);
	}
	static inline bool isConsoleVisible() {
		return IsWindowVisible(GetConsoleWindow()) != FALSE;
	}


	static inline void print(std::string str) {
		if (isConsoleVisible()) {
			std::cout << str << std::endl;
		}
	}
	static inline void printVec2(glm::vec2 vector, std::string name="") {
		if (isConsoleVisible()) {
			if (name.empty()) {
				std::cout << "(" << vector.x << ", " << vector.y << ")" << std::endl;
			} else {
				std::cout << name << " = (" << vector.x << ", " << vector.y << ")" << std::endl;
			}
		}
	}
	static inline void printVec3(glm::vec3 vector, std::string name="") {
		if (isConsoleVisible()) {
			if (name.empty()) {
				std::cout << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")" << std::endl;
			} else {
				std::cout << name << " = (" << vector.x << ", " << vector.y << ", " << vector.z << ")" << std::endl;
			}
		}
	}
	static inline void printVec4(glm::vec4 vector, std::string name="") {
		if (isConsoleVisible()) {
			if (name.empty()) {
				std::cout << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")" << std::endl;
			} else {
				std::cout << name << " = (" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")" << std::endl;
			}
		}
	}
	static inline void printMat4(glm::mat4 matrix, std::string name="") {
		if (isConsoleVisible()) {
			if (name.empty()) {
				std::cout << "[" << std::endl;
			} else {
				std::cout << name << " = [" << std::endl;
			}
			for (size_t x=0; x<4; x++) {
				std::cout << "	";
				for (size_t y=0; y<4; y++) {
					std::cout << matrix[x][y] << ", ";
				}
				std::cout << std::endl;
			}
			std::cout << "]" << std::endl;
		}
	}
	static inline void raise(std::string err) {
		std::cerr << err << std::endl;
		std::string end;
		std::cin >> end;
	}
	static inline void pause() {
		string pause;
		std::cin >> pause;
	}
	static inline void GLErrorcheck(std::string location = "", bool shouldPause = false) {
		GLenum GLError;
		GLError = glGetError();
		if (GLError != GL_NO_ERROR) {
			if (!utils::isConsoleVisible()) {
				utils::showConsole();
			}
			std::cerr << location << " | OpenGL error; " << GLError << std::endl;
			if (shouldPause) {pause();}
		}
	}

	std::string readFile(const std::string& filePath);

	static inline std::string getTimestamp() {
		time_t now = time(nullptr);
		struct tm* timeinfo = localtime(&now);

		std::ostringstream oss;
		oss << std::put_time(timeinfo, "%Y%m%d%H%M%S");

		return oss.str();
	}




	static inline std::string strToLower(const std::string& input) {
		std::string result = input;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){return std::tolower(c);});
		return result;
	}

	static inline std::string strToUpper(const std::string& input) {
		std::string result = input;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){return std::toupper(c);});
		return result;
	}

	static inline bool checkIfInUserConfig(const std::string configName) {
		return userConfig.find(configName) != userConfig.end();
	}
	static inline bool configToBool(const std::string configName) {
		if (checkIfInUserConfig(configName)) {
			std::string configValue = userConfig[configName];
			if ((configValue == "TRUE") || (configValue == "T")) {
				return true;
			} else if ((configValue == "FALSE") || (configValue == "F")) {
				return false;
			} else {
				raise("Unknown config value for " + configName + ": " + configValue);
			}
		} else {
			raise("Unknown config name: " + configName);
		}
		return false;
	}
	static inline int configToIntBool(const std::string configName) {return (configToBool(configName)) ? 1 : 0;}
	static inline int configToInt(const std::string configName) {
		if (checkIfInUserConfig(configName)) {
			std::string valueString = userConfig[configName];
			try {
				return std::stoi(valueString);
			} catch (const std::invalid_argument) {
				raise("Unable to convert " + valueString + " for: " + configName + " to an integer.");
			}
		} else {
			raise("Unknown config name: " + configName);
		}
		return 0;
	}
	static inline float configToFloat(const std::string configName) {
		if (checkIfInUserConfig(configName)) {
			std::string valueString = userConfig[configName];
			try {
				return std::stof(valueString);
			} catch (const std::invalid_argument) {
				raise("Unable to convert " + valueString + " for: " + configName + " to a floating-point value.");
			}
		} else {
			raise("Unknown config name: " + configName);
		}
		return 0.0f;
	}

	static inline bool logicToBool(int A) {return (A > 0);}
	static inline int boolToLogic(bool A) {return (A) ? 1 : 0;}

	static inline bool isVec2NaN(glm::vec2 v) {return (std::isnan(v.x) || std::isnan(v.y));}
	static inline bool isVec3NaN(glm::vec3 v) {return (std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z));}


	float determinant(glm::vec2 vecA, glm::vec2 vecB);


	int RNGc(); //Client
	int RNGw(); //World
	void clearRNG(); //Reset both
	


	class LogicGate {
		private:
			std::function<void(int*, int*, int*, int*)> evalGate;
			int* inputA;
			int* inputB;
			int* output;

			void _assignEvalFunction() {
				switch (this->gateType) {
					case G_AND: evalGate = logicFunctions::LGF_AND; break;
					case G_OR: evalGate = logicFunctions::LGF_OR; break;
					case G_NOT: evalGate = logicFunctions::LGF_NOT; break;
					case G_XOR: evalGate = logicFunctions::LGF_XOR; break;
					case G_LATCH: evalGate = logicFunctions::LGF_LATCH; break;
					case G_PULSE: evalGate = logicFunctions::LGF_PULSE; break;
					case G_TOGGLE: evalGate = logicFunctions::LGF_TOGGLE; break;
					default: evalGate = logicFunctions::LGF_PASSTHROUGH; break;
				}
			}

		public:
			GateType gateType;
			int internalState;

			LogicGate() {
				this->gateType = G_INVALID;
				this->evalGate = logicFunctions::LGF_PASSTHROUGH;

				this->inputA = nullptr;
				this->inputB = nullptr;
				this->output = nullptr;

				this->internalState = 0;
			}

			LogicGate(GateType gateType, int* output, int* inputA, int* inputB=nullptr) {
				//Has optional inputB.
				this->gateType = gateType;
				_assignEvalFunction();


				this->inputA = inputA;
				this->inputB = inputB;
				this->output = output;

				this->internalState = 0;
			}

			void evaluateState() {
				if (evalGate) {
					evalGate(this->inputA, this->inputB, this->output, &(this->internalState));
				}
			}
	};


	struct Texture {
		glm::vec2 dimensions;
		int channels;
		unsigned char* data;
		bool valid;

		Texture() : dimensions(0.0f, 0.0f), channels(0), data(nullptr), valid(false) {}

		Texture(glm::vec2 dimensions, int channels, unsigned char* data)
			: dimensions(dimensions), channels(channels), data(data), valid(true) {}
	};






	struct Player {
		glm::vec3 position, prevPosition, velocity;
		glm::vec2 viewAngle;
		bool touchingFloor;
		float height;
		int health, energy;


		Player()
			: position(stageData.playerStartPoint), prevPosition(stageData.playerStartPoint), velocity(0.0f, 0.0f, 0.0f),
			  viewAngle(stageData.playerStartAngle),
			  touchingFloor(false), height(playerConfig::PLAYER_COLLISION_HEIGHT_STAND) {}
	};



	//From original.
	struct CubeStatic {
		glm::vec3 position, dimensions;
		bool collision;
		GLuint textureID;

		CubeStatic() : position(), dimensions(), collision(), textureID() {}

		CubeStatic(glm::vec3 position, glm::vec3 dimensions, bool collision, GLuint textureID)
			: position(position), dimensions(dimensions),
			  collision(collision), textureID(textureID) {}
	};

	struct Quadrilateral {
		std::array<glm::vec3, 4> vertices;
		bool collision;
		GLuint textureID;

		Quadrilateral() : vertices(), collision(), textureID() {}

		Quadrilateral(
			glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D,
			bool collision, GLuint textureID
		) : vertices{A, B, C, D}, collision(collision), textureID(textureID) {}

		Quadrilateral(
			std::array<glm::vec3, 4> verts, bool collision, GLuint textureID
		) : collision(collision), textureID(textureID) {
			for (size_t idx=0; idx<4; idx++) {
				vertices.at(idx) = verts.at(idx);
			}
		}
	};

	struct Triangle {
		std::array<glm::vec3, 3> vertices;
		bool collision;
		GLuint textureID;

		Triangle() : vertices(), collision(), textureID() {}

		Triangle(
			glm::vec3 A, glm::vec3 B, glm::vec3 C,
			bool collision, GLuint textureID
		) : vertices{A, B, C}, collision(collision), textureID(textureID) {}

		Triangle(
			std::array<glm::vec3, 3> verts, bool collision, GLuint textureID
		) : collision(collision), textureID(textureID) {
			for (size_t idx=0; idx<3; idx++) {
				vertices.at(idx) = verts.at(idx);
			}
		}
	};

	struct SpriteDeco {
		glm::vec3 position;
		glm::vec2 dimensions;
		GLuint textureID;

		SpriteDeco() : position(), dimensions(), textureID() {}

		SpriteDeco(glm::vec3 position, glm::vec2 dimensions, GLuint textureID)
			: position(position), dimensions(dimensions),
			  textureID(textureID) {}
	};

	struct SpriteItem {
		glm::vec3 position;
		glm::vec2 dimensions;
		ItemType type;
		GLuint textureID;

		SpriteItem() : position(), dimensions(), type(I_INVALID), textureID() {}

		SpriteItem(glm::vec3 position, glm::vec2 dimensions, ItemType type, GLuint textureID)
			: position(position), dimensions(dimensions),
			  type(type), textureID(textureID) {}
	};

	struct TriggerVolume {
		glm::vec3 position, dimensions;
		int* IOPtr;

		TriggerVolume() : position(), dimensions(), IOPtr() {}

		TriggerVolume(glm::vec3 position, glm::vec3 dimensions, int* ptr)
			: position(position), dimensions(dimensions),
			  IOPtr(ptr) {}
	};

	struct Interactable {
		std::array<glm::vec3, 4> vertices;
		int* IOPtr;
		GLuint textureID;

		Interactable() : vertices(), IOPtr(), textureID() {}

		Interactable(
			glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D,
			int* ptr, GLuint textureID
		) : vertices{A, B, C, D}, IOPtr(ptr), textureID(textureID) {}

		Interactable(
			std::array<glm::vec3, 4> verts, int* ptr, GLuint textureID
		) : IOPtr(ptr), textureID(textureID) {
			for (size_t idx=0; idx<4; idx++) {
				vertices.at(idx) = verts.at(idx);
			}
		}
	};

	struct CubePath {
		glm::vec3 position, dimensions, movement;
		float speed;
		int* IOPtr;
		GLuint textureID;

		CubePath() : position(), dimensions(), movement(), speed(), IOPtr(), textureID() {}

		CubePath(glm::vec3 position, glm::vec3 dimensions, glm::vec3 movement, float speed, int* ptr, GLuint textureID)
			: position(position), dimensions(dimensions), movement(),
			  speed(speed), IOPtr(ptr), textureID(textureID) {}
	};

	struct SpriteHostile {
		glm::vec3 position;
		glm::vec2 dimensions;
		EnemyType type;
		GLuint textureID;

		SpriteHostile() : position(), dimensions(), type(E_INVALID), textureID() {}

		SpriteHostile(glm::vec3 position, glm::vec2 dimensions, EnemyType type, GLuint textureID)
			: position(position), dimensions(dimensions),
			  type(type), textureID(textureID) {}
	};

	struct CubePhysics {
		glm::vec3 position, dimensions;
		float mass;
		GLuint textureID;

		CubePhysics() : position(), dimensions(), mass(), textureID() {}

		CubePhysics(glm::vec3 position, glm::vec3 dimensions, float mass, GLuint textureID)
			: position(position), dimensions(dimensions),
			  mass(mass), textureID(textureID) {}
	};

	struct Light {
		glm::vec3 position, direction, colour;
		float FOV, minZ, maxZ;
		int* IOPtr;

		Light() : position(), direction(), colour(), FOV(), minZ(), maxZ(), IOPtr(nullptr) {}

		Light(
			glm::vec3 position, glm::vec3 direction, glm::vec3 colour,
			float FOV, float minZ, float maxZ, int* ptr
		) : position(position), direction(direction), colour(colour),
			FOV(FOV), minZ(minZ), maxZ(maxZ), IOPtr(ptr) {}
	};

	struct PathNode {
		glm::vec3 position;
		int index;
		std::vector<int> neighbours;

		PathNode() : position(), index(-1), neighbours() {}

		PathNode(glm::vec3 position, int index, std::vector<int>& connections)
			: position(position), index(index) {
				for (int idx : connections) {
					neighbours.push_back(idx);
				}
			}
	};

	struct Projectile {
		glm::vec3 position, velocity;
		ProjectileType type;
		int strength;
		GLuint textureID;

		Projectile() : position(), velocity(), type(P_INVALID), strength(), textureID() {}

		Projectile(glm::vec3 position, glm::vec3 velocity, ProjectileType type, int strength, GLuint textureID)
			: position(position), velocity(velocity), type(type), strength(strength), textureID(textureID) {}
	};

	struct Explosion {
		glm::vec3 position, dimensions;
		float strength;
		bool expended;
		GLuint textureID;

		Explosion() : position(), dimensions(), strength(), expended(true), textureID() {}

		Explosion(glm::vec3 position, glm::vec3 dimensions, int strength, GLuint textureID)
			: position(position), dimensions(dimensions), strength(strength), expended(false), textureID(textureID) {}
	};


	struct DataSet {
		std::vector<CubeStatic> staticCubes;
		std::vector<Quadrilateral> quadrilaterals;
		std::vector<Triangle> triangles;
		std::vector<SpriteDeco> spriteDecos;
		std::vector<SpriteItem> spriteItems;
		std::vector<TriggerVolume> triggerVolumes;
		std::vector<Interactable> interactables;
		std::vector<CubePath> cubePaths;
		std::vector<SpriteHostile> spriteHostiles;
		std::vector<CubePhysics> cubePhysics;
		std::vector<Light> lights;
		std::vector<PathNode> pathNodes;
		std::vector<Projectile> projectiles;
		std::vector<Explosion> explosions;
	};


	struct Ray {
		glm::vec3 position, direction;

		Ray(glm::vec3 position, glm::vec3 direction)
			: position(position), direction(direction) {}
	};
}

#endif