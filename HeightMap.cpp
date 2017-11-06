#include "HeightMap.h"

/**
 * 
 */
HeightMap::HeightMap(std::string path, double step)
{
	int x = 0;
	int y = 0;
	int n = 0;

	uint8_t *data = (uint8_t*)stbi_load(path.c_str(), &x, &y, &n, 1);

	this->width = x;
	this->height = y;
	this->scaleAmount = step;

	/*Initilize vertex and uv vectors of size height*width*/
	vertexData = std::vector<std::vector<glm::vec3>>(height, std::vector<glm::vec3>(width));
	std::vector<std::vector<glm::vec2>> uvData = std::vector<std::vector<glm::vec2>>(height, std::vector<glm::vec2>(width));


	float textureU = (float) width * 0.1f;
	float textureV = (float) height * 0.1f;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			float xScale = (float) j / (width - 1);
			float zScale = (float) i / (height - 1);

 			/*Get data[i][j] 
 			subtract it from 255 to make white 
 			lower areas on the heightmap instead of black*/
			uint8_t val = 255 - *(data + (width * i + j));

			float heightValue = (float)val / 255.0f; //Reduce range from 0-255 to 0-1 for smaller hills etc

			vertexData[i][j] = glm::vec3(
				-0.5f + xScale,
				heightValue * 5.0,
				-0.5f + zScale);

			uvData[i][j] = glm::vec2(textureU * xScale, textureV * zScale);
		}
	}

	stbi_image_free(data);

	/*Calculate the temp normals. There are two triangles for every (height * width) quad*/
	std::vector<std::vector<glm::vec3>> tempNormals[2];

	tempNormals[0] = std::vector<std::vector<glm::vec3>>(height - 1, std::vector<glm::vec3>(width - 1));
	tempNormals[1] = std::vector<std::vector<glm::vec3>>(height - 1, std::vector<glm::vec3>(width - 1));

	for (int i = 0; i < height - 1; i++) {
		for (int j = 0; j < width - 1; j++) {

			glm::vec3 triangle1[] = { vertexData[i][j], vertexData[i + 1][j], vertexData[i + 1][j + 1] };
			glm::vec3 triangle2[] = { vertexData[i + 1][j + 1], vertexData[i][j + 1], vertexData[i][j] };

			glm::vec3 triangleNorm1 = glm::cross(triangle1[0] - triangle1[1], triangle1[1] - triangle1[2]);
			glm::vec3 triangleNorm2 = glm::cross(triangle2[0] - triangle2[1], triangle2[1] - triangle2[2]);

			tempNormals[0][i][j] = glm::normalize(triangleNorm1);
			tempNormals[1][i][j] = glm::normalize(triangleNorm2);
		}
	}

	/*Use the temp normals to calculate the final normals*/
	std::vector<std::vector<glm::vec3>> normalData = std::vector<std::vector<glm::vec3>>(height, std::vector<glm::vec3>(width));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			glm::vec3 cNormal = glm::vec3(0.0f, 0.0f, 0.0f);
			if (j != 0 && i != 0) { //Upper left triangle
				cNormal += tempNormals[0][i - 1][j - 1];
				cNormal += tempNormals[1][i - 1][j - 1];
			}

			if (i != 0 && j != width - 1) { //Upper right triangle
				cNormal += tempNormals[0][i - 1][j];
			}

			if (i != height - 1 && j != width - 1) { //bottom right triangles
				cNormal += tempNormals[0][i][j];
				cNormal += tempNormals[1][i][j];
			}

			if (i != height - 1 && j != 0) {//bottom left triangles
				cNormal += tempNormals[1][i][j - 1];
			}

			/*Normalize the total of all the normals*/
			cNormal = glm::normalize(cNormal);
			normalData[i][j] = cNormal;
		}
	}


	std::vector<vertex> vertices = std::vector<vertex>();

	/*Combine the vertices, uvs and normals that we have calculated*/
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			vertex v;
			v.v = vertexData[i][j];
			v.uv = uvData[i][j];
			v.n = normalData[i][j];

			vertices.push_back(v);
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBOIndices);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * width * height, vertices.data(), GL_STATIC_DRAW);

	//vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), 0);

	//vertex uvs
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));

	//vertex normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	//Generate indices
	glGenBuffers(1, &VBOIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOIndices);

	indices = std::vector<int>();

	/* Calculate vertices */
	int pResetIndex = width * height;
	for (int i = 0; i < height - 1; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < 2; k++) {
				int row = i + (1 - k);
				int index = row * width + j;
				indices.push_back(index);
			}
		}
		indices.push_back(pResetIndex);
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	//Load floor texture
	texture = new Texture();
	texture->loadTextureFromFile("textures/grass1.jpg");

	shape = new Shape();

	shape->diffuse = glm::vec3(1, 1, 1);
	shape->ambient = glm::vec3(0.7, 0.7, 0.7);
	shape->specular = glm::vec3(0.2, 0.2, 0.2);
	shape->shininess = 16.0f;

}

/**
 * Returns the height of the terrain given a worldspace x, z
 * @param  x The worldspace.x
 * @param  z The worldspace.z
 * @return   The height at x, y
 */
float HeightMap::getHeightFromWorldSpace(float x, float z)
{

	/*The idea behind these calculations is that we are just 
	doing the opposite to what we did when calulating vertexData*/

	float xScale = (float) x / width;
	float zScale = (float) z / height;

	xScale -= -0.5f;
	zScale -= -0.5f;

	xScale *= (width - 1);
	zScale *= (height - 1);

	/*Get the closest indexes to the values calculated*/
	int convertedX = glm::round(xScale);
	int convertedZ = glm::round(zScale);

	/*Make sure converted isnt > or arraybounds or < 0*/
	if (convertedX > height - 1) {
		convertedX = height - 1;
	}

	if (convertedZ > width - 1) {
		convertedZ = width - 1;
	}

	if (convertedX < 0) {
		convertedX = 0;
	}

	if (convertedZ < 0) {
		convertedZ = 0;
	}

	return vertexData[convertedZ][convertedX].y * scaleAmount;
}

HeightMap::~HeightMap()
{
	delete texture;
}

void HeightMap::render(Shader* shader)
{
	glUseProgram(shader->getShaderProgram());
	glBindVertexArray(VAO);
	glm::vec3 scale = glm::vec3(width, scaleAmount, height); //Calculte scale amount for heightMap
	glm::mat4 modelMatrix = glm::scale(glm::mat4(1), scale); //Calculate model matrix from scale
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix)); //Transform the normal matrix to fix lighting

	shader->setUniform("model_matrix", modelMatrix);
	shader->setUniform("normal_matrix", normalMatrix);

	//Set index restart
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(width * height);

	//Bind texture
	shader->setMaterials(shape);
	texture->bindTexture();
	shader->setUniform("texMap", 0);

	//draw
	glDrawElements(GL_TRIANGLE_STRIP, width * (height - 1) * 2 + height - 1, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
