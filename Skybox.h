#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>

#include "Texture.h"
#include "Shader.hpp"
#include <string>
#include <cstdio>
#include <vector>

class Skybox
{
private:
	Texture* texture;
	Shader* shader;
	GLuint VBO;
	GLuint VAO;

	int width;
	int height;
	

public:
	Skybox();
	Skybox(int width, int height);
	void setupSkybox(std::vector<std::string> faces);
	void setupSkybox(Shader * shader, std::vector<std::string> faces);
	~Skybox();
	void render(glm::mat4 viewMatrix, glm::mat4 projection);

};

#endif