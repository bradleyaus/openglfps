#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

#include "external_code/stb_image.h"
#include <string>
#include <cstdio>
#include <vector>

class Texture
{
private:
	GLuint textureID;
	GLuint textureUnit;
	GLenum textureTarget;
public:
	Texture(int textureUnit = 0);
	void loadTextureFromFile(std::string path);
	void loadCubeMapFromFile(std::vector<std::string> faces);

	void bindTexture();
	GLuint getTextureID();
};

#endif