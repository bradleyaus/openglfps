#ifndef SHADER_HPP
#define SHADER_HPP

/**************************************************
 * Simple function to read GLSL shader source from a file,
 * Then compile it and link to create a shader program ready for use.
 * Returns the ID of the shader program (assigned by OpenGL)
 * or 0 if error.
**************************************************/

GLuint LoadShaders(const char * vertex_file_path,
                   const char * fragment_file_path);
enum SHADERMODE
{
	SHADERMODE_DEBUG,
	SHADERMODE_LIGHTING
};

#include "Shape.h"
#include "Light.h"
#include <string>

class Shader
{
private:
	GLuint programID;
	std::string vertexPath;
	std::string fragmentPath;

	int getHandle(std::string variable);

	
	
public:
	SHADERMODE shaderMode;
	Shader(std::string vertexPath, std::string fragmentPath, SHADERMODE shaderMode);
	void loadShader(std::string vertexPath, std::string fragmentPath, SHADERMODE shaderMode);
	Shader(std::string vertexPath, std::string fragmentPath);

	bool reloadShader();
	GLuint getShaderProgram();
	bool setMaterials(Shape* shape);
	bool setLighting(Light* light);
	bool setLighting(std::vector<Light*> lights);
	bool setCameraPos(glm::vec3 position);
	std::string getArrayTag(std::string tag, int index);
	bool setUniform(std::string variable, glm::mat4 value);
	bool setUniform(std::string variable, int value);
	bool setUniform(std::string variable, float value);
	bool setUniform(std::string variable, glm::vec3 value);
	bool setUniform(std::string variable, glm::vec4 value);
	void use();
};

#endif
