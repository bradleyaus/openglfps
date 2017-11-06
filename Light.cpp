#include "Light.h"

Light::Light(glm::vec4 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->position = position;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

Light::Light()
{
	position = glm::vec4(0, 0, 0, 0);
	ambient = glm::vec3(0.8f, 0.8f, 0.8f);
	diffuse = glm::vec3(0.1f, 0.1f, 0.1f);
	specular = glm::vec3(0.1f, 0.1f, 0.1f);
	attenuation = 0.01;
	coneAngle = 1.0;
	intensity = 0.8;
}
