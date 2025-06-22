#ifndef RENDER_H
#define RENDER_H

#include "includes.h"
#include "global.h"
#include "utils.h"
#include <array>



namespace render {
	GLFWwindow* initializeWindow(int width, int height, const char* title);
	GLuint createShaderProgram(std::string name, bool hasVertexSource=true);


	
	GLuint createShaderStorageBufferObject(int binding, size_t bufferSize=0) {
		GLuint SSBO;
		glGenBuffers(1, &SSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, SSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		return SSBO;
	}

	template<typename TGPU, typename TCPU>
	void updateShaderStorageBufferObject(
			GLuint SSBO, utils::Player* player,
			std::vector<TCPU>* dataSetIn,
			std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>* symbolNames //Only used for TOs
		) {

		size_t singleItemSize = sizeof(TGPU);
		size_t size = dataSetIn->size();
		std::vector<TGPU> dataSet;

		for (size_t index=0; index<size; index++) {
			dataSet.push_back(TGPU(dataSetIn->data() + index, player, symbolNames));
		}

		if (size > 0 && !dataSet.empty()) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, singleItemSize * size, dataSet.data());
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	}

	template<typename TGPU, typename TCPU>
	void updateShaderStorageBufferObject(
			GLuint SSBO, utils::Player* player,
			std::vector<TCPU>* dataSetIn
		) {

		size_t singleItemSize = sizeof(TGPU);
		size_t size = dataSetIn->size();
		std::vector<TGPU> dataSet;

		for (size_t index=0; index<size; index++) {
			dataSet.push_back(TGPU(dataSetIn->data() + index, player));
		}

		if (size > 0 && !dataSet.empty()) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, singleItemSize * size, dataSet.data());
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	}



	//Uniforms; [Many overloads]
	static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, bool value) {
		GLuint location = glGetUniformLocation(shaderProgram, uniformName);
		if (location >= 0) {
			glUniform1i(location, value);
		}
	}
	static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, size_t value) {
		GLuint location = glGetUniformLocation(shaderProgram, uniformName);
		if (location >= 0) {
			glUniform1i(location, value);
		}
	}
	static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, int value) {
		GLuint location = glGetUniformLocation(shaderProgram, uniformName);
		if (location >= 0) {
			glUniform1i(location, value);
		}
	}
	static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, float value) {
		GLuint location = glGetUniformLocation(shaderProgram, uniformName);
		if (location >= 0) {
			glUniform1f(location, value);
		}
	}
	static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec2 value) {
		GLuint location = glGetUniformLocation(shaderProgram, uniformName);
		if (location >= 0) {
			glUniform2i(location, value.x, value.y);
		}
	}
	static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec2 value) {
		GLuint location = glGetUniformLocation(shaderProgram, uniformName);
		if (location >= 0) {
			glUniform2f(location, value.x, value.y);
		}
	}
	static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec3 value) {
		GLuint location = glGetUniformLocation(shaderProgram, uniformName);
		if (location >= 0) {
			glUniform3i(location, value.x, value.y, value.z);
		}
	}
	static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec3 value) {
		GLuint location = glGetUniformLocation(shaderProgram, uniformName);
		if (location >= 0) {
			glUniform3f(location, value.x, value.y, value.z);
		}
	}
	static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec4 value) {
		GLuint location = glGetUniformLocation(shaderProgram, uniformName);
		if (location >= 0) {
			glUniform4i(location, value.x, value.y, value.z, value.w);
		}
	}
	static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec4 value) {
		GLuint location = glGetUniformLocation(shaderProgram, uniformName);
		if (location >= 0) {
			glUniform4f(location, value.x, value.y, value.z, value.w);
		}
	}
	static void bindCommonUniforms(GLuint shaderProgram, utils::Player* player) {
		//Applies value if shader has uniform of matching name.

		//Camera Data
		bindUniformValue(shaderProgram, "verticalFOV", verticalFOV);

		//Debug
		bindUniformValue(shaderProgram, "debugMode", utils::configToInt("META_DEBUG_MODE"));

		//Other

		//Resolutions
		bindUniformValue(shaderProgram, "screenResolution", currentWindowResolution);
		bindUniformValue(shaderProgram, "textureResolution", display::TEXTURE_RESOLUTION);
	}



	GLuint createGLImage2D(int width, int height, GLint internalFormat=GL_RGBA32F, GLint samplingType=GL_NEAREST, GLint edgeSampling=GL_REPEAT);
	GLuint loadGLTexture2D(const std::string textureName, std::string subFolder="textures-env", int expectedWidth=-1, int expectedHeight=-1);
	GLuint createTexture2DArray(std::array<std::string, display::TEXTURE_ARRAY_MAX_LAYERS>& textureNames, std::string subFolder="textures-env", bool hasMipMap=false);
}

#endif
