#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma execution_character_set("utf-8")

#include "C:/Users/User/Documents/code/.cpp/stb_image.h"
#include "C:/Users/User/Documents/code/.cpp/stb_image_write.h"
#include "src/includes.h"
#include "src/global.h"
#include "src/loader.h"
#include "src/physics.h"
#include "src/render.h"
#include "src/utils.h"
using namespace std;
using namespace utils;
using namespace glm;


//Images to be used in the UI.
std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS> UIImageNames = {
	"ui-health", "ui-energy"
};

//Symbols to be used in the UI.
std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS> symbolNames = {
	"symbol_0", "symbol_1",
	"symbol_2", "symbol_3",
	"symbol_4", "symbol_5",
	"symbol_6", "symbol_7",
	"symbol_8", "symbol_9",
	"symbol_DASH", "symbol_DOT",
	"symbol_EMARK", "symbol_QMARK",
	"symbol_COMMA", "symbol_QUOTE",
	"symbol_FSLASH", "symbol_COLON",
	"symbol_SEMICOLON", "symbol_AND",
	"symbol_OPNBRACKET", "symbol_CLSBRACKET",
	"symbol_CARET", "symbol_UNKNOWN",
	"symbol_A", "symbol_B",
	"symbol_C", "symbol_D",
	"symbol_E", "symbol_F",
	"symbol_G", "symbol_H",
	"symbol_I", "symbol_J",
	"symbol_K", "symbol_L",
	"symbol_M", "symbol_N",
	"symbol_O", "symbol_P",
	"symbol_Q", "symbol_R",
	"symbol_S", "symbol_T",
	"symbol_U", "symbol_V",
	"symbol_W", "symbol_X",
	"symbol_Y", "symbol_Z"
};




GLFWwindow* Window;
utils::Player player;
std::atomic<bool> runPhysics = true;
GLuint envShader; //Shaders
GLuint textureArrayEnvironment; //Textures



void framebufferSizeCallback(GLFWwindow* Window, int width, int height) {
	glViewport(0, 0, width, height);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	currentWindowResolution = glm::ivec2(width, height);

	verticalFOV = 2 * atan(tan(utils::configToFloat("VIEW_FOV") * 0.5f * constants::TO_RAD) * (float(currentWindowResolution.y) / float(currentWindowResolution.x)));
}



void APIENTRY openGLErrorCallback(
		GLenum source,
		GLenum type, GLuint id,
		GLenum severity,
		GLsizei length, const GLchar* message,
		const void* userParam
	) {
	/*
	Nicely formatted callback from;
	[https://learnopengl.com/In-Practice/Debugging]
	*/
	if(id == 131169 || id == 131185 || id == 131218 || id == 131204) {return;}

	std::cout << "---------------" << std::endl << "Debug message (" << id << ") | " << message << std::endl;

	switch (source)
	{
		case GL_DEBUG_SOURCE_API:             {std::cout << "Source: API"; break;}
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   {std::cout << "Source: Window System"; break;}
		case GL_DEBUG_SOURCE_SHADER_COMPILER: {std::cout << "Source: Shader Compiler"; break;}
		case GL_DEBUG_SOURCE_THIRD_PARTY:     {std::cout << "Source: Third Party"; break;}
		case GL_DEBUG_SOURCE_APPLICATION:     {std::cout << "Source: Application"; break;}
		case GL_DEBUG_SOURCE_OTHER:           {std::cout << "Source: Other"; break;}
	} std::cout << std::endl;

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               {std::cout << "Type: Error"; break;}
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {std::cout << "Type: Deprecated Behaviour"; break;}
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  {std::cout << "Type: Undefined Behaviour"; break;} 
		case GL_DEBUG_TYPE_PORTABILITY:         {std::cout << "Type: Portability"; break;}
		case GL_DEBUG_TYPE_PERFORMANCE:         {std::cout << "Type: Performance"; break;}
		case GL_DEBUG_TYPE_MARKER:              {std::cout << "Type: Marker"; break;}
		case GL_DEBUG_TYPE_PUSH_GROUP:          {std::cout << "Type: Push Group"; break;}
		case GL_DEBUG_TYPE_POP_GROUP:           {std::cout << "Type: Pop Group"; break;}
		case GL_DEBUG_TYPE_OTHER:               {std::cout << "Type: Other"; break;}
	} std::cout << std::endl;
	
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         {std::cout << "Severity: high"; break;}
		case GL_DEBUG_SEVERITY_MEDIUM:       {std::cout << "Severity: medium"; break;}
		case GL_DEBUG_SEVERITY_LOW:          {std::cout << "Severity: low"; break;}
		case GL_DEBUG_SEVERITY_NOTIFICATION: {std::cout << "Severity: notification"; break;}
	} std::cout << std::endl;
	std::cout << std::endl;

	utils::pause();
}



std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS> textureNames;
void prepareOpenGL() {
	//OpenGL setup;

	//Textures
	textureArrayEnvironment = render::createTexture2DArray(textureNames, "textures-env", true);


	//Environment shader
	envShader = render::createShaderProgram("environment", false);



	glViewport(0, 0, currentWindowResolution.x, currentWindowResolution.y);
	glDisable(GL_DEPTH_TEST);

	verticalFOV = 2.0f * atan(tan(utils::configToFloat("VIEW_FOV") * 0.5f * constants::TO_RAD) * (float(currentWindowResolution.y) / float(currentWindowResolution.x)));
	

	//Debug settings
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLErrorCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	utils::GLErrorcheck("Initialisation", true); //Old basic debugging
}


/* //Must be redone.
inline void renderingGeneric(const std::string& shaderName="") {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	if (!shaderName.empty()) {
		utils::GLErrorcheck(shaderName, true);
	}
}
*/

void renderFrame(double blendingAlpha) {
	//Update resolution
	glViewport(0, 0, currentWindowResolution.x, currentWindowResolution.y);

	//Environment Shader.
	/* //Must be redone.
	glUseProgram(envShader);
	glBindImageTexture(0, renderedFrameID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(1, positionMapID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(2, normalMapID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glBindTextureUnit(0, textureArrayEnvironment);
	glBindTextureUnit(1, skyboxTextureID);

	//Uniforms
	render::bindCommonUniforms(envShader, &player);
	render::bindUniformValue(envShader, "useMipMapping", utils::configToBool("VIEW_MIPMAPPING"));
	render::bindUniformValue(envShader, "allowTransparency", utils::configToBool("VIEW_ALLOW_TRANSPARENCY"));

	renderingGeneric("Environment Shader");
	*/
}



//Data must be synced between updateSSBOs() and the physics thread.
utils::DataSet stateA, stateB;
utils::DataSet* physicsData = &stateA;
utils::DataSet* graphicsData = &stateB;
std::mutex stateSwapMutex;

//Non-synced data.
std::vector<utils::LogicGate> logicGates;
std::array<int, constants::MAX_FLAGS> flags;


double tickStart;
void physicsLoop(bool* physicsReady) {
	double maxTickTime = 1.0f/constants::PHYSICS_FREQUENCY;

	tick = 0;
	while (runPhysics) {
		tickStart = glfwGetTime();
		*physicsReady = false;
		player.prevPosition = player.position;

		//Update logic states.
		for (int index=0; index<logicGates.size(); index++) {
			LogicGate gate = logicGates[index];
			if (gate.gateType == G_INVALID) {continue;}
			gate.evaluateState();
			logicGates[index] = gate;
		}
		physics::playerMove(&player);


		//Update states;
		{
			std::lock_guard<std::mutex> lock(stateSwapMutex);
			std::swap(physicsData, graphicsData);
		}

		float dt = glfwGetTime() - tickStart;
		tickrate = floor(1.0f / dt);
		if constexpr (dev::SHOW_PHYSICS_TICKRATE) {
			std::cout << "Tickrate: " << tickrate << "Hz" << std::endl;
		}
		if constexpr (dev::SHOW_PHYSICS_DT) {
			std::cout << "Tick #" << tick << " took " << std::setprecision(6) << (dt * 1e6f) << "µs / Hypothetical tickrate: " << static_cast<int>(1.0f / dt) << endl;
		}

		*physicsReady = true;
		while (glfwGetTime() - tickStart < maxTickTime) {std::this_thread::yield();}
		tick++;
	}
}



double cursorXPos, cursorYPos, cursorXPosPrev, cursorYPosPrev;
void handleInputs() {
	glfwPollEvents();

	//Get inputs for this frame
	for (auto &pair : userBindings) {
		std::string functionName = pair.first;
		int keyEnum = pair.second;
		if (keyEnum == -1) {
			std::cout << functionName  << " was not bound to a key!" << std::endl;;
			continue;
		}

		int keyState = glfwGetKey(Window, keyEnum);
		if (keyState == GLFW_PRESS) {
			keyMap[functionName] = true;

		} else if (keyState == GLFW_RELEASE) {
			keyMap[functionName] = false;
		}
	}


	//Meta controls
	if (keyMap["META_FREECURSOR"]) {
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	} else {
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwGetCursorPos(Window, &cursorXPos, &cursorYPos);
	}


	//Crouch changes physical height
	if (keyMap["MOVE_CROUCH"]) {
		player.height = playerConfig::PLAYER_COLLISION_HEIGHT_CROUCH;
		player.touchingFloor = false;
	} else {
		player.height = playerConfig::PLAYER_COLLISION_HEIGHT_STAND;
	}



	double cursorXDelta = cursorXPos - cursorXPosPrev;
	double cursorYDelta = cursorYPos - cursorYPosPrev;
	player.viewAngle.x += cursorXDelta * (utils::configToFloat("TURN_SPEED_MOUSE"));
	player.viewAngle.x = fmodf(player.viewAngle.x + 540.0f, 360.0f) - 180.0f;
	double dY = cursorYDelta * (utils::configToFloat("TURN_SPEED_MOUSE"));
	player.viewAngle.y = glm::clamp(float(player.viewAngle.y+dY), -89.0f, 89.0f);
}


std::thread physicsThread;
inline void stopPhysics() {
	runPhysics = false;
	if (physicsThread.joinable()) {
		physicsThread.join();
	}
}

int main() {
	try { //Catch exceptions
	SetConsoleOutputCP(65001); //CP_UTF8

	loader::loadBindings();
	loader::loadStage(
		userConfig["META_STAGE_NAME"], &player,
		&logicGates, &flags,
		&textureNames
	);


	currentWindowResolution = display::INITIAL_SCREEN_RESOLUTION;


	Window = render::initializeWindow(currentWindowResolution.x, currentWindowResolution.y, "test4/C++-Remake");
	glfwSetFramebufferSizeCallback(Window, framebufferSizeCallback);
	glfwGetCursorPos(Window, &cursorXPos, &cursorYPos);
	glEnable(GL_BLEND);
	bool vsync = utils::configToBool("VIEW_VSYNC");
	if (vsync) {
		glfwSwapInterval(1);
	}

	cursorXPosPrev = cursorXPos;
	cursorYPosPrev = cursorYPos;
	utils::GLErrorcheck("Window Creation", true);

	prepareOpenGL();
	*graphicsData = *physicsData;
	double maxFrameTime = 1.0f/utils::configToFloat("VIEW_MAX_FREQ");


	//Threads;
	bool physicsReady;
	physicsThread = std::thread(physicsLoop, &physicsReady);
	tickStart = glfwGetTime();

	frame = 0;
	while (!glfwWindowShouldClose(Window)) {
		double frameStart = glfwGetTime();
		double blendingAlpha = (frameStart - tickStart) * constants::PHYSICS_FREQUENCY;

		handleInputs();
		if (keyMap["META_EXIT"]) {break; /* Quit Immediately */}



		utils::DataSet* localGraphicsData = nullptr;
		{
			std::lock_guard<std::mutex> lock(stateSwapMutex);
			localGraphicsData = graphicsData;
		}
		renderFrame(blendingAlpha);
		glfwSwapBuffers(Window);


		float dt = glfwGetTime() - frameStart;
		if (utils::configToBool("META_SHOW_DT_CONSOLE")) {
			std::cout << "Frame #" << frame << " took " << std::setprecision(2) << (dt * 1e3f) << "ms / Hypothetical framerate: " << static_cast<int>(1.0f / dt) << endl;
		}
		if (!vsync) {
			while (glfwGetTime() - frameStart < maxFrameTime) {std::this_thread::yield();}
		}
		framerate = floor(1.0f / (glfwGetTime() - frameStart));
		if (utils::configToBool("META_SHOW_FRAMERATE_CONSOLE")) {
			std::cout << "Framerate: " << framerate << "Hz" << std::endl;
		}

		cursorXPosPrev = cursorXPos;
		cursorYPosPrev = cursorYPos;
		frame++;
	}

	//Cleanup OpenGL.
	glDeleteTextures(1, &textureArrayEnvironment);

	stopPhysics();
	glfwDestroyWindow(Window);
	glfwTerminate();
	return 0;


	//Catch exceptions.
	} catch (const std::exception& e) {
		stopPhysics();
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		std::cerr << "An exception was thrown: " << e.what() << std::endl;
		pause();
		return -1;
	} catch (...) {
		stopPhysics();
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		std::cerr << "An unspecified exception was thrown." << std::endl;
		pause();
		return -1;
	}
}
