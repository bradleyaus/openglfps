#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "glm/glm.hpp"
#include "Model.h"

class Object 
{
private:
	glm::vec3 position;
	glm::vec3 scale;
	Model* objectModel;
public:
	Object(Model* objectModel, glm::vec3 position, glm::vec3 scale = glm::vec3(1, 1, 1));
	void render();
	void setPosition(glm::vec3 position);
};


#endif