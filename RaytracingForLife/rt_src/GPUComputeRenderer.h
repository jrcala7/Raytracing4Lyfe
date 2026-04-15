#pragma once

#include <glad/glad.h>
#include <vector>
#include <iostream>

struct ComputeImage {
	GLuint texture;
	GLuint width;
	GLuint height;

	ComputeImage(GLuint w, GLuint h) : width(w), height(h) {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	~ComputeImage() {
		if (texture != 0) {
			glDeleteTextures(1, &texture);
		}
	}

	void Bind(GLuint binding) const {
		glBindImageTexture(binding, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}

	void ReadPixels(std::vector<float>& outData) {
		outData.resize(width * height * 4);
		glBindTexture(GL_TEXTURE_2D, texture);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, outData.data());
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};

class ComputeRenderer {
public:
	ComputeRenderer(GLuint width, GLuint height) 
		: width_(width), height_(height), program_(0), image_(nullptr) {
	}

	~ComputeRenderer() {
		if (program_ != 0) {
			glDeleteProgram(program_);
		}
		if (image_ != nullptr) {
			delete image_;
		}
	}

	/// Load a compute shader program
	bool LoadShaderProgram(GLuint program) {
		if (program_ != 0) {
			glDeleteProgram(program_);
		}
		program_ = program;
		return program_ != 0;
	}

	/// Initialize or recreate the output image
	bool InitializeImage() {
		if (image_ != nullptr) {
			delete image_;
		}
		image_ = new ComputeImage(width_, height_);
		return image_ != nullptr;
	}

	/// Execute the compute shader
	bool Dispatch() {
		if (program_ == 0 || image_ == nullptr) {
			std::cerr << "Compute renderer not properly initialized" << std::endl;
			return false;
		}

		glUseProgram(program_);
		image_->Bind(0);

		// Calculate work group counts (assuming 8x8 local group size)
		GLuint groupsX = (width_ + 7) / 8;
		GLuint groupsY = (height_ + 7) / 8;

		glDispatchCompute(groupsX, groupsY, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		return true;
	}

	/// Read the computed image back to CPU
	bool ReadImage(std::vector<float>& outPixels) {
		if (image_ == nullptr) {
			return false;
		}
		image_->ReadPixels(outPixels);
		return true;
	}

	GLuint GetWidth() const { return width_; }
	GLuint GetHeight() const { return height_; }

private:
	GLuint width_;
	GLuint height_;
	GLuint program_;
	ComputeImage* image_;
};
