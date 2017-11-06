#include "Skybox.h"

Skybox::Skybox()
{
	this->width = 50;
	this->width = 50;
}

Skybox::Skybox(int width, int height)
{
	this->width = width;
	this->height = height;
}

void Skybox::setupSkybox(std::vector<std::string> faces)
{
	const char* vertexPath = "shaders/skybox.vert";
	const char* fragmentPath = "shaders/skybox.frag";

	shader = new Shader(vertexPath, fragmentPath);
	
	setupSkybox(shader, faces);
}

/**
 * Sets up vertices and VBO/VAO for faces
 * @param shader shader to use
 * 
 * @param faces vector of strings in the following order:
 * 0 - Right
 * 1 - Left
 * 2 - Top
 * 3 - Bottom
 * 4 - Back
 * 5 - Front
 */
void Skybox::setupSkybox(Shader* shader, std::vector<std::string> faces)
{

	this->shader = shader;
	shader->use();
	

	float vertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	texture = new Texture();
	texture->loadCubeMapFromFile(faces);
	texture->bindTexture();
	shader->setUniform("texMap", 0);
}

Skybox::~Skybox()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	delete texture;
	delete shader;
}

void Skybox::render(glm::mat4 viewMatrix, glm::mat4 projection)
{
	glDepthFunc(GL_LEQUAL);
	shader->use();

	glm::mat4 view = glm::mat4(glm::mat3(viewMatrix)); 
	shader->setUniform("projection_matrix", projection);
	viewMatrix = glm::scale(viewMatrix, (glm::vec3(width, height, height)));
	shader->setUniform("view_matrix", viewMatrix);

	glBindVertexArray(VAO);
	texture->bindTexture();
	
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}
