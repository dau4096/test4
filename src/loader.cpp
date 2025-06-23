#include "includes.h"
#include "global.h"
#include "utils.h"
using namespace std;
using namespace utils;
using namespace glm;
using namespace pugi;


namespace xmlFallbackAttribFunc {

static inline glm::vec3 parseVec3(const std::string& str) {
	std::istringstream ss(str);
	glm::vec3 v;
	ss >> v.x >> v.y >> v.z;
	return v;
};

static inline glm::vec2 parseVec2(const std::string& str) {
	std::istringstream ss(str);
	glm::vec2 v;
	ss >> v.x >> v.y;
	return v;
};


int* managePTR(std::string ptrStr, std::array<int, constants::MAX_FLAGS>* flags) {
	std::string ptrStrUpper = strToUpper(ptrStr);
	if ((ptrStrUpper == "TRUE") || (ptrStrUpper == "ALWAYS")) {
		return &(constants::C_TRUE);
	} else if ((ptrStrUpper == "FALSE") || (ptrStrUpper == "NEVER")) {
		return &(constants::C_FALSE);
	}

	try {
		int flagIndex = std::stoi(ptrStr);
		if ((flagIndex < 0) || (flagIndex > (constants::MAX_FLAGS-1))) {
			raise("Pointer string: [" + ptrStr + "] was not an integer flag index, [0 -> " + std::to_string(constants::MAX_FLAGS) + "] (inclusive).");
		}
		return &((*flags)[flagIndex]);


	} catch (const std::invalid_argument& err) {
		raise("Pointer string: [" + ptrStr + "] was not an integer flag index, [0 -> " + std::to_string(constants::MAX_FLAGS) + "] (inclusive).");
	} catch (const std::out_of_range& err) {
		raise("Pointer string: [" + ptrStr + "] was not an integer flag index, [0 -> " + std::to_string(constants::MAX_FLAGS) + "] (inclusive).");
	}
	return &(constants::C_FALSE);
};



static const std::unordered_map<std::string, int> enumMap = {
	//Logic gates 			Items 					Enemies 				Projectiles
	{"G_INVALID", 0}, 		{"I_INVALID", 0}, 		{"E_INVALID", 0}, 		{"P_INVALID", 0},
	{"G_PASSTHROUGH", 1}, 	{"I_HEALTH_SMALL", 1},	{"E_MELEE", 1}, 		{"P_BALL", 1},
	{"G_AND", 2}, 			{"I_HEALTH_LARGE", 2},	{"E_RANGED", 2}, 		{"P_ROCKET", 2}, 
	{"G_OR", 3}, 			{"I_ENERGY_SMALL", 3},
	{"G_NOT", 4}, 			{"I_ENERGY_LARGE", 4},
	{"G_XOR", 5},
	{"G_LATCH", 6},
	{"G_PULSE", 7},
	{"G_TOGGLE", 8},
};

int assignEnum(const std::string& enumStr) {
	auto it = enumMap.find(enumStr);
	if (it != enumMap.end()) return it->second;
	raise("Unknown Enum: " + enumStr);
	return -1;
}

int currentTextureIndex = 0;
int assignTexture(std::string textureStr, std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames) {
	auto begin = textureNames->begin(), end = textureNames->end();
	auto namePTR = std::find(begin, end, textureStr);

	int idx = -1;

	if (namePTR != end) {
		idx = std::distance(begin, namePTR);
	} else {
		if (currentTextureIndex >= display::TEXTURE_ARRAY_MAX_LAYERS) {
			raise("Maximum texture layers reached. Cannot assign more.");
			return -1;
		}
		textureNames->at(currentTextureIndex) = textureStr;
		idx = currentTextureIndex;
		currentTextureIndex++;
	}
	return idx;
}


//Helper functions;

static inline int getInt(const pugi::xml_node& node, std::string attrName, int defaultValue=0) {
	pugi::xml_attribute attr = node.attribute(attrName);
	if (attr) {
		return attr.as_int();
	}
	return defaultValue;
}

static inline float getFloat(const pugi::xml_node& node, std::string attrName, float defaultValue=0.0f) {
	pugi::xml_attribute attr = node.attribute(attrName);
	if (attr) {
		return attr.as_float();
	}
	return defaultValue;
}

static inline std::string getString(const pugi::xml_node& node, std::string attrName, std::string defaultValue="") {
	pugi::xml_attribute attr = node.attribute(attrName);
	if (attr) {
		return attr.as_string();
	}
	return defaultValue;
}

static inline bool getBool(const pugi::xml_node& node, std::string attrName, bool defaultValue=false) {
	pugi::xml_attribute attr = node.attribute(attrName);
	if (attr) {
		std::string attrValue = utils::strToUpper(attr.as_string());
		if (attrValue == "TRUE" || attrValue == "T") {
			return true;
		} else if (attrValue == "FALSE" || attrValue == "F") {
			return false;
		}
	}
	return defaultValue;
}

static inline glm::vec2 getVec2(const pugi::xml_node& node, std::string attrName, glm::vec2 defaultValue=glm::vec2(0.0f, 0.0f)) {
	pugi::xml_attribute attr = node.attribute(attrName);
	if (attr) {
		return parseVec2(attr.as_string());
	}
	return defaultValue;
}

static inline glm::vec3 getVec3(const pugi::xml_node& node, std::string attrName, glm::vec3 defaultValue=glm::vec3(0.0f, 0.0f, 0.0f)) {
	pugi::xml_attribute attr = node.attribute(attrName);
	if (attr) {
		return parseVec3(attr.as_string());
	}
	return defaultValue;
}

static inline int getEnum(const pugi::xml_node& node, std::string attrName, int defaultValue=0) {
	pugi::xml_attribute attr = node.attribute(attrName);
	if (attr) {
		return assignEnum(utils::strToUpper(attr.as_string()));
	}
	return defaultValue;
}

static inline int* getPTR(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::string attrName,
		int* defaultValue=nullptr
	) {
	pugi::xml_attribute attr = node.attribute(attrName);
	if (attr) {
		return managePTR(attr.as_string(), flags);
	}
	return defaultValue;
}

static inline int getTexture(
		const pugi::xml_node& node,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames,
		std::string attrName,
		const char* defaultValue=display::FALLBACK_TEXTURE_PATH
	) {
	pugi::xml_attribute attr = node.attribute(attrName);
	const char* name;
	if (attr) {
		std::string attrValue = attr.as_string();
		name = attrValue.c_str();
	} else {
		name = defaultValue;
	}
	return assignTexture(name, textureNames);
}

}

using namespace xmlFallbackAttribFunc;



namespace xml {


template<typename T>
std::vector<T> fetchObjectFromXML(
		const pugi::xml_document& doc,
		const std::string& xpath,
		std::function<T(
			const pugi::xml_node&,
			std::array<int, constants::MAX_FLAGS>* flags,
			std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
		)> extractor,
		std::array<int, constants::MAX_FLAGS>* flags=nullptr,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames=nullptr
	)
{
	std::vector<T> result{};
	pugi::xpath_node_set nodeList = doc.select_nodes(xpath.c_str());
	size_t count = static_cast<size_t>(nodeList.size());
	
	for (size_t i=0; i<count; i++) {
		pugi::xml_node node = nodeList[i].node();
		result.push_back(extractor(node, flags, textureNames));
	}
	return result;
}




static inline CubeStatic extractCubeStatic(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	CubeStatic cs = CubeStatic(
		getVec3(node, "position", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "dimensions", glm::vec3(0.0f, 0.0f, 0.0f)),
		getBool(node, "collision", false),
		getTexture(node, textureNames, "texture", initial::FALLBACK_TEXTURE_NAME)
	);
	
	return cs;
}

static inline Quadrilateral extractQuadrilateral(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	Quadrilateral q = Quadrilateral(
		getVec3(node, "vert0", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "vert1", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "vert2", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "vert3", glm::vec3(0.0f, 0.0f, 0.0f)),
		getBool(node, "collision", false),
		getTexture(node, textureNames, "texture", initial::FALLBACK_TEXTURE_NAME)
	);
	
	return q;
}

static inline Triangle extractTriangle(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	Triangle t = Triangle(
		getVec3(node, "vert0", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "vert1", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "vert2", glm::vec3(0.0f, 0.0f, 0.0f)),
		getBool(node, "collision", false),
		getTexture(node, textureNames, "texture", initial::FALLBACK_TEXTURE_NAME)
	);
	
	return t;
}

static inline SpriteDeco extractSpriteDeco(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	SpriteDeco sd = SpriteDeco(
		getVec3(node, "position", glm::vec3(0.0f, 0.0f, 0.0f)),
		glm::vec2(
			getFloat(node, "width", 0.0f),
			getFloat(node, "height", 0.0f)
		),
		getTexture(node, textureNames, "texture", initial::FALLBACK_TEXTURE_NAME)
	);
	
	return sd;
}

static inline SpriteItem extractSpriteItem(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	SpriteItem si = SpriteItem(
		getVec3(node, "position", glm::vec3(0.0f, 0.0f, 0.0f)),
		glm::vec2(
			getFloat(node, "width", 0.0f),
			getFloat(node, "height", 0.0f)
		),
		static_cast<ItemType>(getEnum(node, "type", I_INVALID)),
		getTexture(node, textureNames, "texture", initial::FALLBACK_TEXTURE_NAME)
	);
	
	return si;
}

static inline TriggerVolume extractTriggerVolume(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	TriggerVolume tv = TriggerVolume(
		getVec3(node, "position", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "dimensions", glm::vec3(0.0f, 0.0f, 0.0f)),
		getPTR(node, flags, "flag", nullptr)
	);
	
	return tv;
}

static inline Interactable extractInteractable(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	Interactable i = Interactable(
		getVec3(node, "vert0", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "vert1", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "vert2", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "vert3", glm::vec3(0.0f, 0.0f, 0.0f)),
		getPTR(node, flags, "flag", nullptr),
		getTexture(node, textureNames, "texture", initial::FALLBACK_TEXTURE_NAME)
	);
	
	return i;
}

static inline CubePath extractCubePath(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	CubePath cp = CubePath(
		getVec3(node, "position", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "dimensions", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "movement", glm::vec3(0.0f, 0.0f, 0.0f)),
		getFloat(node, "speed", 0.0f),
		getPTR(node, flags, "flag", nullptr),
		getTexture(node, textureNames, "texture", initial::FALLBACK_TEXTURE_NAME)
	);
	
	return cp;
}

static inline SpriteHostile extractSpriteHostile(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	SpriteHostile sh = SpriteHostile(
		getVec3(node, "position", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "dimensions", glm::vec3(0.0f, 0.0f, 0.0f)),
		static_cast<EnemyType>(getEnum(node, "type", E_INVALID)),
		getTexture(node, textureNames, "texture", initial::FALLBACK_TEXTURE_NAME)
	);
	
	return sh;
}

static inline CubePhysics extractCubePhysics(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	CubePhysics cp = CubePhysics(
		getVec3(node, "position", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "dimensions", glm::vec3(0.0f, 0.0f, 0.0f)),
		getFloat(node, "mass", 0.0f),
		getTexture(node, textureNames, "texture", initial::FALLBACK_TEXTURE_NAME)
	);
	
	return cp;
}

static inline Light extractLight(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	Light l = Light(
		getVec3(node, "position", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "direction", glm::vec3(0.0f, 0.0f, 0.0f)),
		getVec3(node, "colour", glm::vec3(0.0f, 0.0f, 0.0f)),
		getFloat(node, "fov", 0.0f),
		getFloat(node, "minZ", 0.0f),
		getFloat(node, "maxZ", 0.0f),
		getPTR(node, flags, "flag", nullptr)
	);
	
	return l;
}

size_t pathNodeIdx = 0;
static inline PathNode extractPathNode(
		const pugi::xml_node& node,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {

	std::vector<int> blankList;
	PathNode pn = PathNode(
		getVec3(node, "position", glm::vec3(0.0f, 0.0f, 0.0f)),
		pathNodeIdx,
		blankList
	);
	pathNodeIdx++;
	
	return pn;
}





static inline pugi::xml_node getMetaNode(const pugi::xml_document& doc, std::string subNodeName) {
	std::string xpath = "//meta/" + subNodeName;
	pugi::xpath_node_set nodeList = doc.select_nodes(xpath.c_str());
	size_t count = static_cast<size_t>(nodeList.size());
	if (count != 1) {
		utils::print("Unknown " + subNodeName + " metanode found.");
	}
	return nodeList[0].node();
}


static inline float handlePlayerHEString(std::string inputSTR, float maxValue) {
	if (strToUpper(inputSTR) == "MAX") {
		return maxValue;
	} else if (strToUpper(inputSTR) == "MIN") {
		return 1.0f;
	}
	try {
		return std::stof(inputSTR);
	} catch (const std::invalid_argument) {
		raise("Unable to convert " + inputSTR + " to a floating-point value.");
	}
	return 0.0f;
}


void retrieveStageMetaData(const pugi::xml_document& doc, utils::Player* player) {
	*player = utils::Player();

	//Player
	pugi::xml_node playerNode = getMetaNode(doc, "player");
	stageData.playerStartPoint = getVec3(playerNode, "startPoint", initial::PLAYER_START_POSITION);
	stageData.playerStartAngle = getVec2(playerNode, "startAngle", initial::PLAYER_START_VANGLE);

	//Physics
	pugi::xml_node physNode = getMetaNode(doc, "physics");
	stageData.gravity = getFloat(playerNode, "gravity", initial::PHYS_GRAVITY);
	stageData.killPlaneZ = getFloat(playerNode, "killPlaneZ", initial::PHYS_KPZ);
}



void fetchBindingsFromXML(const pugi::xml_document& doc) {
	const char* xpath = "//keybinds/bind";
	pugi::xpath_node_set nodeList = doc.select_nodes(xpath);
	size_t count = static_cast<size_t>(nodeList.size());
	
	std::string functionString, keyString;
	for (size_t i = 0; i < count; ++i) {
		pugi::xml_node node = nodeList[i].node();

		functionString = strToUpper(node.attribute("function").as_string());
		if (userBindings.find(functionString) == userBindings.end()) {
			raise("Unknown binding function: " + functionString);
		}

		keyString = strToUpper(node.attribute("key").as_string());
		if (keyNameToGLFW.find(keyString) != keyNameToGLFW.end()) {
			userBindings.at(functionString) = keyNameToGLFW.at(keyString);
		} else {
			raise("Unknown Key: " + keyString + " for binding function: " + functionString);
		}
	}
}

void fetchConfigsFromXML(const pugi::xml_document& doc) {
	const char* xpath = "//config/option";
	pugi::xpath_node_set nodeList = doc.select_nodes(xpath);
	size_t count = static_cast<size_t>(nodeList.size());
	
	std::string functionString, valueString;
	for (size_t i = 0; i < count; ++i) {
		pugi::xml_node node = nodeList[i].node();

		functionString = strToUpper(node.attribute("function").as_string());
		if (userConfig.find(functionString) == userConfig.end()) {
			raise("Unknown config function: " + functionString);
		}

		valueString = strToUpper(node.attribute("value").as_string());
		if (valueString != "") {
			userConfig.at(functionString) = valueString;
		} else {
			raise("Invalid value for: " + functionString);
		}
	}
}

}



//Has pointer
template<typename T>
static inline void setConfigFromStringOptionsMap(
		std::string configName,
		std::unordered_map<std::string, T>* map,
		const std::string& defaultValue,
		T* outPTR
	) {
	std::string keyString = userConfig[configName];
	auto it = map->find(keyString);
	if (it != map->end()) {
		*outPTR = it->second;
	} else {
		std::cout << ("Invalid config value: " + keyString) << std::endl << "Expected one of:";
		for (const auto& pair : *map) {
			if (pair.first.empty()) {continue; /* Blank option */}
			if constexpr (std::is_same_v<T, glm::ivec2>) {
				std::cout << " for [" << pair.second.x << " x " << pair.second.y << "]";
			} else if constexpr (std::is_same_v<T, glm::ivec3>) {
				std::cout << " for [" << pair.second.x << " x " << pair.second.y << " x " << pair.second.z << "]";
			} else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_arithmetic_v<T>) {
				std::cout << " for " << pair.second;
			} else {
				// Fallback for other types
				std::cout << " (unprintable value type)";
			}
		}

		auto fallback = map->at(defaultValue);
		*outPTR = fallback;
	}

}

//No pointer
template<typename T>
static inline void setConfigFromStringOptionsMap(
		std::string configName,
		std::unordered_map<std::string, T>* map,
		const std::string& defaultValue
	) {
	std::string keyString = userConfig[configName];
	auto it = map->find(keyString);
	if (it != map->end()) {
		userConfig[configName] = std::to_string(it->second);
	} else {
		std::cout << ("Invalid config value: " + keyString) << std::endl << "Expected one of:";
		for (const auto& pair : *map) {
			if (pair.first.empty()) {continue; /* Blank option */}
			if constexpr (std::is_same_v<T, glm::ivec2>) {
				std::cout << " for [" << pair.second.x << " x " << pair.second.y << "]";
			} else if constexpr (std::is_same_v<T, glm::ivec3>) {
				std::cout << " for [" << pair.second.x << " x " << pair.second.y << " x " << pair.second.z << "]";
			} else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_arithmetic_v<T>) {
				std::cout << " for " << pair.second;
			} else {
				// Fallback for other types
				std::cout << " (unprintable value type)";
			}
		}

		auto fallback = map->at(defaultValue);
		userConfig[configName] = std::to_string(fallback);
	}

}


namespace loader {


void loadStage(
		const std::string& stageName, utils::Player* player,
		utils::DataSet* dataSet,
		std::vector<utils::LogicGate>* logicGates,
		std::array<int, constants::MAX_FLAGS>* flags,
		std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* textureNames
	) {
	std::string filePath = "stages/" + stageName + ".xml";
	std::string XMLSrc = utils::readFile(filePath);


	pugi::xml_document doc;
	pugi::xml_parse_result parseResult = doc.load_string(XMLSrc.c_str());
	if (!parseResult) {
		throw std::runtime_error("Failed to parse XML: " + std::string(parseResult.description()));
	}
	
	dataSet->staticCubes = xml::fetchObjectFromXML<utils::CubeStatic>(doc, "//environment/staticCube", xml::extractCubeStatic, flags, textureNames);
	dataSet->quadrilaterals = xml::fetchObjectFromXML<utils::Quadrilateral>(doc, "//environment/quad", xml::extractQuadrilateral, flags, textureNames);
	dataSet->triangles = xml::fetchObjectFromXML<utils::Triangle>(doc, "//environment/tri", xml::extractTriangle, flags, textureNames);
	dataSet->spriteDecos = xml::fetchObjectFromXML<utils::SpriteDeco>(doc, "//object/sprite", xml::extractSpriteDeco, flags, textureNames);
	dataSet->spriteItems = xml::fetchObjectFromXML<utils::SpriteItem>(doc, "//object/item", xml::extractSpriteItem, flags, textureNames);
	dataSet->triggerVolumes = xml::fetchObjectFromXML<utils::TriggerVolume>(doc, "//environment/trigger", xml::extractTriggerVolume, flags, textureNames);
	dataSet->interactables = xml::fetchObjectFromXML<utils::Interactable>(doc, "//environment/switch", xml::extractInteractable, flags, textureNames);
	dataSet->cubePaths = xml::fetchObjectFromXML<utils::CubePath>(doc, "//environment/cubeMove", xml::extractCubePath, flags, textureNames);
	dataSet->spriteHostiles = xml::fetchObjectFromXML<utils::SpriteHostile>(doc, "//object/enemy", xml::extractSpriteHostile, flags, textureNames);
	dataSet->cubePhysics = xml::fetchObjectFromXML<utils::CubePhysics>(doc, "//object/cubePhys", xml::extractCubePhysics, flags, textureNames);
	dataSet->pathNodes = xml::fetchObjectFromXML<utils::PathNode>(doc, "//environment/node", xml::extractPathNode, flags, textureNames);


	stageData.name = stageName;
	stageData.filePath = filePath;
	xml::retrieveStageMetaData(doc, player);
}


void loadBindings() {
	const std::string filePath = "userConfig.xml";
	std::string XMLSrc = utils::readFile(filePath);

	pugi::xml_document doc;
	pugi::xml_parse_result parseResult = doc.load_string(XMLSrc.c_str());
	if (!parseResult) {
		throw std::runtime_error("Failed to parse XML: " + std::string(parseResult.description()));
	}

	xml::fetchBindingsFromXML(doc);
	xml::fetchConfigsFromXML(doc);


	if (utils::configToBool("META_SHOW_CONSOLE")) {
		utils::showConsole();
	} else {
		utils::hideConsole();
	}
}

}