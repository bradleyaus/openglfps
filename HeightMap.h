#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>

#include "VBO.h"
#include "Shader.hpp"
#include "Texture.h"

class HeightMap
{
private:
	int width;
	int height;
	double scaleAmount;

	GLuint VAO;
	GLuint VBO;
	GLuint VBOIndices;

	std::vector<std::vector<glm::vec3>> vertexData;
	std::vector<vertex> vertices;
	std::vector<int> indices;

	Texture* texture;
	Shape* shape;

public:
	HeightMap(std::string path, double step);
	float getHeightFromWorldSpace(float x, float z);
	~HeightMap();
	void render(Shader * shader);
	
};