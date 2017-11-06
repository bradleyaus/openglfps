#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "glm/glm.hpp"

class Light
{
public:
	glm::vec4 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float coneAngle;
	float attenuation;
	float intensity;
	glm::vec3 coneDirection;

	Light(glm::vec4 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	Light();
};

#endif