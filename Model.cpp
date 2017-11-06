#include "Model.h"

Model::Model()
{
	VBOCreated = false;
	maximums = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);
	minimums = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	foundNormals = false;
	foundTexCoords = false;
	fillScale = 1;
}

Model::Model(Shader * shader)
{
	this->shader = shader;
	Model();
}

Model::~Model()
{
	if (VBOCreated) {
		glDeleteVertexArrays(1, &vaoHandle);
		glDeleteBuffers(bufferSize, buffer);
	}
}

/**
* Creates the buffer objects needed for the model
*/
bool Model::createVBO(unsigned int currentProgramID)
{

	glUseProgram(currentProgramID);
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	int vertLoc = glGetAttribLocation(currentProgramID, "i_vertex");
	int texLoc = glGetAttribLocation(currentProgramID, "i_texCoordinates");
	int normalLoc = glGetAttribLocation(currentProgramID, "i_normals");

	bufferSize = 2;
	unsigned int currentIndex = 0;

	if (foundNormals) {
		bufferSize++;
	}
	if (foundTexCoords) {
		bufferSize++;
	}

	buffer = new unsigned int[bufferSize];
	glGenBuffers(bufferSize, buffer);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[currentIndex]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), &positions.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);
	currentIndex++;

	if (foundNormals) { //Only load normals if they exist in this obj file
		glBindBuffer(GL_ARRAY_BUFFER, buffer[currentIndex]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), &normals.front(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(normalLoc);
		glVertexAttribPointer(normalLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);
		currentIndex++;
	}

	if (foundTexCoords) { //Only load tex coords if they exist in the obj file
		glBindBuffer(GL_ARRAY_BUFFER, buffer[currentIndex]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(texLoc);
		glVertexAttribPointer(texLoc, VALS_PER_TEXCOORDINATE, GL_FLOAT, GL_FALSE, 0, 0);
		currentIndex++;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[currentIndex]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	VBOCreated = true;
	return true;
}

bool Model::render(glm::vec3 position, glm::vec3 scale)
{
	
	glUseProgram(shader->getShaderProgram());

	glBindVertexArray(vaoHandle);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1), position);
	modelMatrix = glm::scale(modelMatrix, scale);
	
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix)); //Transform the normal matrix to fix lighting normals after model translation/scale

	shader->setUniform("model_matrix", modelMatrix);
	shader->setUniform("normal_matrix", normalMatrix);

	for (int i = 0; i < storedShapes.size(); i++) { //Draw every shape in the model
		Shape* shape = &storedShapes.at(i);
		shader->setMaterials(shape); //Set the materials for the current shape
		int startIndex = shape->startIndex();

		if (shape->texture.getTextureID() != 0 && shader->shaderMode != SHADERMODE_DEBUG) { //Set textures only if in texture/lighting mode
			//Get uniform handles for the texture and the model
			shape->texture.bindTexture();
			shader->setUniform("texMap", 0);
			
		}
		/*Draw using the indicies for the current shape*/
		glDrawElements(GL_TRIANGLES, (shape->endIndex() - shape->startIndex()), GL_UNSIGNED_INT, (GLvoid*)(startIndex * sizeof(unsigned int)));

	}

	glBindVertexArray(0);
	return true;
}

/*
* Returns a vec3 which is the center point of the model
*/
glm::vec3 Model::getCenter()
{
	glm::vec3 center;

	center.x = (maximums.x + minimums.x) / 2;
	center.y = (maximums.y + minimums.y) / 2;
	center.z = (maximums.z + minimums.z) / 2;

	return center;
}

/*
* Returns a double which is the radius of the models bounding circle
*/
double Model::getRadius()
{
	glm::vec3 distance = maximums - minimums;
	return glm::max(distance.x, glm::max(distance.y, distance.z)) /2;
}

/**
* Loads a model using tinyobj and converts it into internal format
*/
bool Model::loadModel(std::string path, std::string modelDir)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shape_t;
	std::vector<tinyobj::material_t> materials;

	int uniqueCount = 0;

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shape_t, &materials, &err, path.c_str(), modelDir.c_str());

	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
		return false;
	}

	for (int j = 0; j < shape_t.size(); j++) { //For every shape
		tinyobj::shape_t shape = shape_t.at(j);
		tinyobj::material_t material = materials.at(shape.mesh.material_ids.at(0));

		/*Setup the shape material*/
		glm::vec3 ambient;
		ambient.x = material.ambient[0];
		ambient.y = material.ambient[1];
		ambient.z = material.ambient[2];

		glm::vec3 diffuse;
		diffuse.x = material.diffuse[0];
		diffuse.y = material.diffuse[1];
		diffuse.z = material.diffuse[2];

		glm::vec3 specular;
		specular.x = material.specular[0];
		specular.y = material.specular[1];
		specular.z = material.specular[2];

		Shape currentShape = Shape(modelDir + material.diffuse_texname, ambient, diffuse, specular, (float)material.shininess);
		if (indices.size() == 0) {
			currentShape.setStartIndex(0);
		}
		else {
			currentShape.setStartIndex(indices.size());
		}

		for (size_t i = 0; i < shape.mesh.indices.size(); i++) {

			tinyobj::index_t index0 = shape.mesh.indices[i + 0];

			MapType vertex = MapType(index0);

			float x = attrib.vertices.at(3 * index0.vertex_index + 0);
			float y = attrib.vertices.at(3 * index0.vertex_index + 1);
			float z = attrib.vertices.at(3 * index0.vertex_index + 2);

			//Store the minimium and maximum points
			if (x < minimums[0]) {
				minimums[0] = x;
			}
			if (y < minimums[1]) {
				minimums[1] = y;
			}
			if (z < minimums[2]) {
				minimums[2] = z;
			}

			if (x > maximums[0]) {
				maximums[0] = x;
			}
			if (y > maximums[1]) {
				maximums[1] = y;
			}
			if (z > maximums[2]) {
				maximums[2] = z;
			}

			//Check if we found a unique vertex,normal,uv combination
			auto search = vertexMap.find(vertex);
			if (search != vertexMap.end()) {
				indices.push_back(search->second);
			}
			else {

				positions.push_back(x);
				positions.push_back(y);
				positions.push_back(z);

				if (index0.normal_index != -1) {
					normals.push_back(attrib.normals.at(3 * index0.normal_index + 0));
					normals.push_back(attrib.normals.at(3 * index0.normal_index + 1));
					normals.push_back(attrib.normals.at(3 * index0.normal_index + 2));
					foundNormals = true;
				}
				else {
					normals.push_back(0);
					normals.push_back(0);
					normals.push_back(0);
				}
				if (index0.texcoord_index != -1) {
					uvs.push_back(attrib.texcoords.at(2 * index0.texcoord_index + 0));
					uvs.push_back(1 - attrib.texcoords.at(2 * index0.texcoord_index + 1)); //1-v fixes texture bug
					foundTexCoords = true;
				}
				else {
					uvs.push_back(0);
					uvs.push_back(0);
				}
				indices.push_back(uniqueCount);
				vertexMap[vertex] = uniqueCount;
				uniqueCount++;
			}
		}
		currentShape.setEndIndex(indices.size());
		storedShapes.push_back(currentShape);
	}
	return true;
}
