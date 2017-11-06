#ifndef _VIEWER_H_
#define _VIEWER_H_

#include "InputState.h"
#include "glm/glm.hpp"

enum MOVEMENT
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class FPSViewer
{

public:
	const float speed = 0.1f;
	const float playerHeight = 3.0f;

	float roll;
	float pitch;
	float yaw;
	
	glm::vec3 cameraPosition;
	glm::vec3 cameraDirection;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;

	FPSViewer(glm::vec3 initialPos);

	void update(InputState &input);
	void processKeyboard(MOVEMENT direction, float maxHeight, bool flyMode);
	glm::mat4 getViewMatrix();
	glm::vec3 getPosition();
	glm::vec3 getDirection();

private:
	void internalCameraUpdate();
};

#endif // VIEWER_H
