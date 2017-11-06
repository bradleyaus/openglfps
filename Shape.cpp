#include "Shape.h"

Shape::Shape(std::string path, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;

	sIndex = 0;
	eIndex = 0;

	texture.loadTextureFromFile(path);

}

Shape::Shape(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;

	sIndex = 0;
	eIndex = 0;
}

Shape::Shape()
{
	sIndex = 0;
	eIndex = 0;
}

void Shape::setStartIndex(int value)
{
	this->sIndex = value;
}

void Shape::setEndIndex(int value)
{
	this->eIndex = value;
}

int Shape::startIndex()
{
	return sIndex;
}

int Shape::endIndex()
{
	return eIndex;
}

