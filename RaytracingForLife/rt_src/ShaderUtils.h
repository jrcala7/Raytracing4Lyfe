#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderUtils {
public:
	/// Load and compile a compute shader from file
	/// Returns the program handle, or 0 on failure
	static GLuint LoadComputeShader(const std::string& filepath) {
		std::string source = ReadFile(filepath);
		if (source.empty()) {
			std::cerr << "Failed to read shader file: " << filepath << std::endl;
			return 0;
		}

		GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		// Check for compile errors
		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			std::cerr << "Compute shader compilation failed:\n" << infoLog << std::endl;
			glDeleteShader(shader);
			return 0;
		}

		// Create program and link
		GLuint program = glCreateProgram();
		glAttachShader(program, shader);
		glLinkProgram(program);

		// Check for linking errors
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, nullptr, infoLog);
			std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
			glDeleteProgram(program);
			glDeleteShader(shader);
			return 0;
		}

		glDeleteShader(shader);
		std::cout << "Compute shader loaded successfully: " << filepath << std::endl;
		return program;
	}

	/// Load and compile shader from source string
	static GLuint LoadComputeShaderFromSource(const std::string& source) {
		GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			std::cerr << "Compute shader compilation failed:\n" << infoLog << std::endl;
			glDeleteShader(shader);
			return 0;
		}

		GLuint program = glCreateProgram();
		glAttachShader(program, shader);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, nullptr, infoLog);
			std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
			glDeleteProgram(program);
			glDeleteShader(shader);
			return 0;
		}

		glDeleteShader(shader);
		return program;
	}

private:
	static std::string ReadFile(const std::string& filepath) {
		std::ifstream file(filepath);
		if (!file.is_open()) {
			return "";
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
};
