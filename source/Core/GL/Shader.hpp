#pragma once

#include <fstream>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glad.h"
#include "../types.h"

class Shader {
	public:
		enum class Type {
			VERTEX,
			FRAGMENT
		};
		Shader(std::string const file, u32 shaderType) {
            DEBUG(Debug::Type::SHADER, "Opening shader: %s\n", file.c_str());
			Shdr = glCreateShader(shaderType);
			shdrType = (shaderType == GL_VERTEX_SHADER ? Type::VERTEX : Type::FRAGMENT);

			std::ifstream ifile(file.c_str());
			if (ifile.good()) {
				ifile.seekg(0, ifile.end);
				size_t fsize = (int)ifile.tellg();
				ifile.seekg(0, ifile.beg);

				source = new char[fsize + 1]{0};
				ifile.read(source, fsize);
				ifile.close();

				glShaderSource(Shdr, 1, &source, 0);
				glCompileShader(Shdr);
			}
			else {
				ERROR(Debug::Type::SHADER, "Unable to open file.\n");
			}
			glShaderSource(Shdr, 1, &source, 0);
			glCompileShader(Shdr);
		}

		~Shader() {
			delete[] source;
			glDeleteShader(Shdr);
		}

		bool HasErrors() {
			int success = 0;
			GLint maxLength = 0;
			glGetShaderiv(Shdr, GL_COMPILE_STATUS, &success);
			glGetShaderiv(Shdr, GL_INFO_LOG_LENGTH, &maxLength);
			if (success == GL_FALSE) {
				GLchar msg[512];
				glGetShaderInfoLog(Shdr, maxLength, NULL, msg);
				std::ofstream ofs(shdrType == Type::VERTEX ? SD("/vertLog.txt") : SD("fragLog.txt"));
				if (ofs) {
					ofs.write(msg, maxLength);
					ofs.close();
				}
			}

			return (success == GL_FALSE);
		}

		void Install(GLuint &prog) {
			if (!prog) {
				DEBUG(Debug::Type::SHADER, "Program is null!\n");
				return;
			}
			glAttachShader(prog, Shdr);
		}

	private:
		char *source;
		Type shdrType;
		GLuint Shdr;
};