#include "Texture.h"


Texture::Texture(int textureUnit)
{
	this->textureUnit = textureUnit;
}

/*
* Loads a texture from the given path into the shapes textureID
* If the texture doesn't exist then loads default white
*/
void Texture::loadTextureFromFile(std::string path)
{

	fprintf(stderr, "Loading texture %s\n", path.c_str());

	textureTarget = GL_TEXTURE_2D;
	int x;
	int y;
	int n;
	if (path == "") {
		path = "models/default.jpg";
	}
	unsigned char *data = stbi_load(path.c_str(), &x, &y, &n, 0);
	if (n == 3) {

		glGenTextures(1, &textureID);
		glBindTexture(textureTarget, textureID);

		glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(textureTarget, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

		glGenerateMipmap(textureTarget);
		//glBindTexture(textureTarget, 0);
	}
	else {
		fprintf(stderr, "Image pixels are not RGB. Defaulting to white texture\n");
		loadTextureFromFile("models/default.jpg");
	}
}

/**
* Expects in the order:
* 0 - Right
* 1 - Left
* 2 - Top
* 3 - Bottom
* 4 - Back
* 5 - Front
*/
void Texture::loadCubeMapFromFile(std::vector<std::string> faces)
{
	fprintf(stderr, "Loading cubemap\n");
	textureTarget = GL_TEXTURE_CUBE_MAP;

	glGenTextures(1, &textureID);
	glBindTexture(textureTarget, textureID);

	for (int i = 0; i < faces.size(); i++) {
		fprintf(stderr, "\tloading %s\n", faces.at(i).c_str());
		int x;
		int y;
		int n;
		unsigned char *data = stbi_load(faces.at(i).c_str(), &x, &y, &n, 0);

		if (n == 3) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			fprintf(stderr, "Error loading cubemap image %s\n", faces.at(i).c_str());
			exit(1);
		}
	}
	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

void Texture::bindTexture()
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(textureTarget, textureID);
}

GLuint Texture::getTextureID()
{
	return textureID;
}
