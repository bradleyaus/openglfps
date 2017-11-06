#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

#include "Shader.hpp"

int CompileShader(std::string shaderPath, const GLuint ShaderID)
{
	// Read shader code from file
	std::string ShaderCode;
	std::ifstream ShaderStream(shaderPath, std::ios::in);
	if (ShaderStream.is_open()) {
		std::string Line = "";
		while (getline(ShaderStream, Line)) {
			ShaderCode += "\n" + Line;
		}
		ShaderStream.close();
	}
	else {
		std::cerr << "Cannot open " << shaderPath << ". Are you in the right directory?" << std::endl;
		return 0;
	}

	// Compile Shader
	char const *SourcePointer = ShaderCode.c_str();
	glShaderSource(ShaderID, 1, &SourcePointer, NULL);
	glCompileShader(ShaderID);

	// Check Shader
	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	printf("compiled shader %s %d %d\n", shaderPath.c_str(), Result, InfoLogLength);
	if (InfoLogLength > 1) {
		char* ShaderErrorMessage = new char[InfoLogLength + 1];
		glGetShaderInfoLog(ShaderID,
			InfoLogLength,
			NULL,
			&ShaderErrorMessage[0]);
		std::cerr << &ShaderErrorMessage[0] << std::endl;
		delete[] ShaderErrorMessage;
		return 0;
	}
	return 1;
}

GLuint LoadShaders(std::string vertex_file_path, std::string fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Compile both shaders. Exit if compile errors.
	if (!CompileShader(vertex_file_path, VertexShaderID)
		|| !CompileShader(fragment_file_path, FragmentShaderID)) {
		return 0;
	}

	// Link the program
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		std::cerr << &ProgramErrorMessage[0] << std::endl;
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
	loadShader(vertexPath, fragmentPath, SHADERMODE_LIGHTING);
}

Shader::Shader(std::string vertexPath, std::string fragmentPath, SHADERMODE shaderMode)
{
	loadShader(vertexPath, fragmentPath, shaderMode);
}

/**
 * Returns the handle for the current shader
 * @param  variable The variable name to retrieve
 * @return          handle (-1 if invalid handle)
 */
int Shader::getHandle(std::string variable)
{
	int handle = glGetUniformLocation(this->getShaderProgram(), variable.c_str());
	if (handle == -1) {
		std::cout << "Error: could not get handle: " << variable << "\n";
		return -1;
	}
	return handle;
}

/**
 * Loads a shader given vertex and fragment paths
 * @param vertexPath   the path to the vertex shader
 * @param fragmentPath the path to the fragment shader
 * @param shaderMode   deprecated
 */
void Shader::loadShader(std::string vertexPath, std::string fragmentPath, SHADERMODE shaderMode)
{
	this->vertexPath = vertexPath;
	this->fragmentPath = fragmentPath;
	this->shaderMode = shaderMode;

	programID = LoadShaders(vertexPath, fragmentPath);
	if (programID == 0) {
		std::cout << "Error: Couldn't load shaders - " << vertexPath << " and " << fragmentPath << std::endl;
		return;
	}
}

/*
* Tries to reload and compile the shader from file
* Reverts back to old if compilation fails
*/
bool Shader::reloadShader()
{
	GLuint tProgramID = programID;
	programID = LoadShaders(vertexPath, fragmentPath);
	if (programID == 0) {
		programID = tProgramID;
		std::cout << "Error: Couldn't reload shaders, reverting - " << vertexPath << " and " << fragmentPath << std::endl;
		return false;
	}
	return true;
}

GLuint Shader::getShaderProgram()
{
	return programID;
}

/*
* Loads the shape's materials into the shader program.
*/
bool Shader::setMaterials(Shape* shape)
{
	glUseProgram(this->programID);

	if (shaderMode == SHADERMODE_DEBUG) {
		int diffuseHandle = glGetUniformLocation(this->programID, "mtl_diffuse");
		glUniform3fv(diffuseHandle, 1, glm::value_ptr(shape->diffuse));
		return true;
	}
	else {

		//setUniform("mtl_ambient", shape->ambient);
		//setUniform("mtl_diffuse", shape->diffuse);
		setUniform("mtl_specular", shape->specular);
		setUniform("shininess", shape->shininess);
		return true;
	}
}

/**
 * Loads all light variables into the shader program
 * @param  lights vector of lights to load
 * @return       success state
 */
bool Shader::setLighting(std::vector<Light*> lights)
{
	glUseProgram(this->programID);
	if (shaderMode == SHADERMODE_LIGHTING) {

		setUniform("numLights", (int) lights.size());

		for (int i = 0; i < lights.size(); i++) {
			Light* light = lights[i];
			setUniform(getArrayTag("position", i), light->position);
			setUniform(getArrayTag("ambient", i), light->ambient);
			setUniform(getArrayTag("diffuse", i), light->diffuse);
			setUniform(getArrayTag("specular", i), light->specular);
			setUniform(getArrayTag("coneDirection", i), light->coneDirection);
			setUniform(getArrayTag("coneAngle", i), light->coneAngle);
			setUniform(getArrayTag("attenuation", i), light->attenuation);
			setUniform(getArrayTag("intensity", i), light->intensity);
				//return true;
			//return false;
		}
	}
	return true;
}

/**
 * Helper function that gets lights[index].tag
 * @param  tag   the variable name
 * @param  index index in the lights array
 * @return lights[index].tag      
 */
std::string Shader::getArrayTag(std::string tag, int index)
{
	return "lights[" + std::to_string(index) + "]." + tag;
}

/** 
*
* SET UNIFORM FUNCTIONS
*
*/

bool Shader::setUniform(std::string variable, glm::mat4 value)
{
	int handle = getHandle(variable);
	if (handle == -1) {
		return false;
	}
	glUniformMatrix4fv(handle, 1, false, glm::value_ptr(value));
	return true;
}

bool Shader::setUniform(std::string variable, int value)
{
	int handle = getHandle(variable);
	if (handle == -1) {
		return false;
	}

	glUniform1i(handle, value);
	return true;
}

bool Shader::setUniform(std::string variable, float value)
{
	int handle = getHandle(variable);
	if (handle == -1) {
		return false;
	}

	glUniform1f(handle, value);
	return true;
}

bool Shader::setUniform(std::string variable, glm::vec3 value)
{
	int handle = getHandle(variable);
	if (handle == -1) {
		return false;
	}

	glUniform3fv(handle, 1, glm::value_ptr(value));
	return true;
}

bool Shader::setUniform(std::string variable, glm::vec4 value)
{
	int handle = getHandle(variable);
	if (handle == -1) {
		return false;
	}

	glUniform4fv(handle, 1, glm::value_ptr(value));
	return true;
}


bool Shader::setCameraPos(glm::vec3 position)
{
	glUseProgram(this->programID);
	return setUniform("cameraPos", position);

}

void Shader::use()
{
	glUseProgram(getShaderProgram());
}
