#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <cstdio>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Texture.h"

class Shape {
private:
	
	int sIndex;
	int eIndex;

public:
	Shape(std::string path, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);

	Shape(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);

	Shape();

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	Texture texture;

	void setStartIndex(int value);
	void setEndIndex(int value);

	int startIndex();
	int endIndex();

};

#endif