#ifndef _FONT_H_
#define _FONT_H_
#include <string>
#include <cstdio>
#include "glm/glm.hpp"


#ifdef USEFREETYPE //Use FREETYPE
#include <GL/glew.h>

#include <map>
#include "ft2build.h"
#include FT_FREETYPE_H
#include "Shader.hpp"


	struct characterTexture
	{
		GLuint textureID;
		glm::ivec2 size;
		glm::ivec2 offset;
		GLuint advance;
	};

	class Font
	{
	private:
		const int fontSize = 48;
		const GLubyte charLimit = 128;
		const char* vertexPath = "shaders/font.vert";
		const char* fragmentPath = "shaders/font.frag";


		glm::vec4 fontColour;

		Shader* shader;
		FT_Library FTLibrary;
		FT_Face FTFace;
		GLuint VAO;
		GLuint VBO;

		std::map<char, characterTexture> characterTextures;
	
		bool loadedCorrectly;

	public:
		Font();
		bool setupFont();
		void renderText(std::string text, glm::vec3 position, float scale);
	};


#else //FREETYPE library doesn't exist, create skeleton class

class Font
{
private:
	

public:
	void renderText(std::string text, glm::vec3 position, float scale);
	bool setupFont();
	Font();
};

#endif
#endif