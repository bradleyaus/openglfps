#include "Object.h"

Object::Object(Model* objectModel, glm::vec3 position, glm::vec3 scale)
{
	this->objectModel = objectModel;
	this->position = position;
	this->scale = scale;
}

void Object::render()
{
	objectModel->render(position, scale);
}

void Object::setPosition(glm::vec3 position)
{
	this -> position = position;
}
