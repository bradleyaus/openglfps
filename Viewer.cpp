#include <stdio.h>
#include <math.h>

#include "Viewer.h"
#include "glm/gtc/matrix_transform.hpp"

#define DEG2RAD(x) ((x)*M_PI/180.0) 
#define RAD2DEG(x) ((x)*180.0/M_PI) 


FPSViewer::FPSViewer(glm::vec3 initialPos)
{
	cameraPosition = initialPos;
	cameraDirection = glm::vec3(0, 0, -1);	

	roll = 0.0;
	pitch = 0.0;
	yaw = -90.0;
	internalCameraUpdate();
}

/** Update pitch and yaw given input */
void FPSViewer::update(InputState &input)
{
	float x1, y1;
	input.readDeltaAndReset(&x1, &y1);

	yaw += x1;
	pitch -= y1;

	//Stop camera flip
	if (pitch > 89.9f)
		pitch = 89.9f;
	if (pitch < -89.9f)
		pitch = -89.9f;

	internalCameraUpdate();
}
/**
 * Handles a movement in the direction given
 * @param direction MOVEMENT type
 * @param maxHeight the height of the terrain at the camera position
 * @param flyMode   should the camera be locked above the terrain
 */
void FPSViewer::processKeyboard(MOVEMENT direction, float maxHeight, bool flyMode)
{
	float dx = 0;
	float dz = 0;

	float vel = 2;
	switch (direction) {
	case FORWARD:
		cameraPosition += cameraDirection * vel;
		break;
	case BACKWARD:
		cameraPosition -= cameraDirection * vel;
		break;
	case LEFT:
		cameraPosition += cameraRight * vel;
		break;
	case RIGHT:
		cameraPosition -= cameraRight * vel;
		break;
	}

	if (!flyMode) {
		cameraPosition.y = maxHeight + playerHeight;
	}
}

/**
 * Updates the camera direction, right and up vectors given pitch, yaw.
 */
void FPSViewer::internalCameraUpdate()
{

	glm::vec3 newCameraDirection;
	newCameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newCameraDirection.y = sin(glm::radians(pitch));
	newCameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraDirection = glm::normalize(newCameraDirection);

	cameraRight = glm::normalize(glm::cross(cameraDirection, glm::vec3(0, 1, 0)));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));
}

/** Returns the view matrix for the current position */
glm::mat4 FPSViewer::getViewMatrix() 
{
	return glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
}

/**
 * @return cameras current position
 */
glm::vec3 FPSViewer::getPosition()
{
	return cameraPosition;
}

/**
 * @return cameras current position
 */
glm::vec3 FPSViewer::getDirection()
{
	return cameraDirection;

}
