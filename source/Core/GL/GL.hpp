#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#include "glad.h"
#include "Shader.hpp"
#include "../../Core/types.h"

#define BYTE2FLOAT(x) ((float)x/255.0f)

typedef enum {
    PIX_ORDER_RGB,
    PIX_ORDER_BGR
} PixelOrder;

struct RGBA {
	RGBA() {
		R = G = B = A = 0xFF;
	}

	RGBA(u8 r, u8 g, u8 b, u8 a) {
		R = r;
		G = g;
		B = b;
		A = a;
	}

	RGBA(u32 color) {
		R = color >> 24;
		G = (color >> 16) & 0xFF;
		B = (color >> 8) & 0xFF;
		A = color & 0xFF;
	}

	operator u32 &() const {
		return *((u32 *)this);
	}

	u8 R;
	u8 G;
	u8 B;
	u8 A;
};

typedef struct Rect {
    float x;
    float y;
    float w;
    float h;
} Rect;

typedef struct Texture {
    GLuint id;
    GLint internal_format;
    GLenum format;
    GLenum type;
	float w;
	float h;
    bool hasAlpha;
} Texture;

class GL {
    public:
        static void Init(Rect viewport) {
            //GLFW init
			DEBUG(Debug::Type::GL, "Init.\n");
            if (!glfwInit()) {
				ERROR(Debug::Type::GL, "ERROR - Failed Init!\n");
            }
            
            //GL attributes
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    
            //Create window
            Window = glfwCreateWindow(viewport.w, viewport.h, "DeltaLaunch", nullptr, nullptr);
			if (!Window) {
				ERROR(Debug::Type::GL, "Failed to create window\n");
				Quit();
			}
            
            //Create contex
            glfwSwapInterval(1);
            glfwMakeContextCurrent(Window);
            gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

			DEBUG(Debug::Type::GL, "GL Vendor: %s\n", glGetString(GL_VENDOR));
			DEBUG(Debug::Type::GL, "GL Renderer: %s\n", glGetString(GL_RENDERER));
			DEBUG(Debug::Type::GL, "GL Version: %s\n", glGetString(GL_VERSION));
            
            //Update viewport
            glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0.0f, viewport.w, viewport.h, 0.0f, 0.0f, 1.0f);
            glfwSetTime(0.0);

			//Create shader Program
			shaderProg = glCreateProgram();
        }
        
        static void Quit() {
            glDeleteProgram(shaderProg);
            glfwDestroyWindow(Window);
            glfwTerminate();
        }

		static void AddShader(std::string shdr, u32 type) {
			Shader sh = Shader(shdr, type);
			if (sh.HasErrors()) {
				ERROR(Debug::Type::GL, "Error in compiling shader.\n");
			}
			else sh.Install(shaderProg);
		}

		static void LinkShaderProgram() {
			glLinkProgram(shaderProg);

			GLint success;
			glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
			if (success == GL_FALSE) {
				char buf[512];
				glGetProgramInfoLog(shaderProg, sizeof(buf), nullptr, buf);
				std::ofstream shdrLog(SD("/shaderprogramLog.txt"));
				if (shdrLog.good()) {
					shdrLog.write(buf, sizeof(buf));
					shdrLog.close();
				}
			}
		}

		static void DestroyShaders() {
			glDeleteProgram(shaderProg);
			shaderProg = glCreateProgram();
		}

		static void PollEvents() {
			glfwPollEvents();
		}
        
        static void Rectangle(Rect pos, u8 r, u8 g, u8 b, u8 a) {
            glColor4ub(r, g, b, a);
            if(a) {
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquation(GL_FUNC_ADD);
            }else {
                glDisable(GL_BLEND);
            }
            glRecti(pos.x, pos.y, pos.x + pos.w, pos.y + pos.h);
        }
        
        static void Blit(unsigned char *dst, unsigned char* src, int size) {
            //alpha blending
            for(int i = 0; i < size; i+=4) {
                float srca = BYTE2FLOAT(src[i+3]);
                dst[i] = (u8)((BYTE2FLOAT(src[i]) * srca) + (BYTE2FLOAT(dst[i]) * (1.0 - srca))) * 255;
                dst[i+1] = (u8)((BYTE2FLOAT(src[i+1]) * srca) + (BYTE2FLOAT(dst[i+1]) * (1.0 - srca))) * 255;
                dst[i+2] = (u8)((BYTE2FLOAT(src[i+2]) * srca) + (BYTE2FLOAT(dst[i+2]) * (1.0 - srca))) * 255;
                dst[i+3] = (u8)255;
            }
        }
        
        static Texture *SurfToTex(unsigned char *img, int w, int h, int channels) {
			DEBUG(Debug::Type::GL, "SurfToTex() [%d x %d][%d channel]\n", w, h, channels);
            Texture *tex = new Texture;
            
			switch (channels) {
				case 1:
					tex->internal_format = GL_ALPHA8;
					tex->format = GL_ALPHA;
					tex->hasAlpha = true;
					break;
				case 2:
					tex->internal_format = GL_LUMINANCE_ALPHA;
					tex->format = GL_LUMINANCE_ALPHA;
					tex->hasAlpha = false;
					break;
				case 3:
					tex->internal_format = GL_RGB8;
					tex->format = GL_RGB;
					tex->hasAlpha = false;
					break;
				default:
				case 4:
					tex->internal_format = GL_RGBA8;
					tex->format = GL_RGBA;
					tex->hasAlpha = true;
					break;
			}

            tex->type = GL_UNSIGNED_BYTE;
			tex->w = w;
			tex->h = h;
            
            glGenTextures(1, &tex->id);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tex->id);

            glTexImage2D(GL_TEXTURE_2D, 0, tex->internal_format, tex->w, tex->h, 0, tex->format, tex->type, img);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);

            return tex;
        }
        
		static void RenderTexture(Texture *tex, Rect pos, Rect *clip, bool useShader = false) {
			if (!tex->id) {
				ERROR(Debug::Type::GL, "Null texture!\n");
				return;
			}
			GLfloat minu = 0, maxu = 1, minv = 0, maxv = 1;
			if (clip != NULL) {
				minu = (GLfloat)clip->x / pos.w;
				maxu = (GLfloat)(clip->x + clip->w) / tex->w;
				minv = (GLfloat)clip->y / pos.h;
				maxv = (GLfloat)(clip->y + clip->h) / tex->h;
				pos.w = clip->w;
				pos.h = clip->h;
			}

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tex->id);
			glColor4ub(255, 255, 255, 255);
            if(useShader) {
                glUseProgram(shaderProg);
                glUniform1i(glGetUniformLocation(shaderProg, "tex0"), 0);
            }
            if(tex->hasAlpha) {
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquation(GL_FUNC_ADD);
            } else {
                glDisable(GL_BLEND);
            }
			glBegin(GL_QUADS);
				glTexCoord2f(minu, minv); glVertex2i(pos.x, pos.y);
				glTexCoord2f(minu, maxv); glVertex2i(pos.x, pos.y + pos.h);
				glTexCoord2f(maxu, maxv); glVertex2i(pos.x + pos.w, pos.y + pos.h);
				glTexCoord2f(maxu, minv); glVertex2i(pos.x + pos.w, pos.y);
			glEnd();
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);
        }
        
        static void FreeTexture(Texture *tex) {
			DEBUG(Debug::Type::GL, "FreeTexture()\n");
            if(tex->id) {
                glDeleteTextures(1, &tex->id);
            }
        }
        
        static void Render() {
            glfwSwapBuffers(Window);
        }
        
        static void Clear() {
            glColor4ub(255, 255, 255, 255);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        
        static GLFWwindow *Window;
        static GLuint shaderProg;
};