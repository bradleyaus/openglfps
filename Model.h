#ifndef _MODEL_H_
#define _MODEL_H_

#define VALS_PER_VERT 3
#define VALS_PER_TEXCOORDINATE 2

#include "MapType.h"
#include "Shape.h"
#include "Shader.hpp"

#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <cmath>

class Model
{

private:
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;
	std::vector<Shape> storedShapes;
	std::unordered_map<MapType, int> vertexMap;


	GLuint vaoHandle;
	unsigned int* buffer;
	unsigned int bufferSize;

	glm::vec3 maximums;
	glm::vec3 minimums;

	bool foundNormals;
	bool foundTexCoords;

	bool VBOCreated;

	Shader* shader;

public:

	Model();

	Model(Shader* shader);
	~Model();
	bool loadModel(std::string path, std::string modelDir = "models/");
	bool createVBO(unsigned int programID);
	bool render(glm::vec3 position, glm::vec3 scale);
	glm::vec3 getCenter();
	double getRadius();
	GLfloat fillScale;
};

#endif 