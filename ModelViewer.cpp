#include <iostream>
#include <cstdio>

#define TINYOBJLOADER_IMPLEMENTATION 
#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Viewer.h"
#include "Model.h"
#include "Object.h"
#include "HeightMap.h"
#include "Skybox.h"
#include "Font.h"

enum DRAWMODE
{
	DRAWMODE_WIREFRAME,
	DRAWMODE_VERTEXNORMAL,
	DRAWMODE_DIFFUSECOLOUR
};

enum TIMEMODE
{
	TIME_DAY,
	TIME_NIGHT
};

int drawModeCount = 3;
DRAWMODE currentDrawMode = DRAWMODE_WIREFRAME;
SHADERMODE currentShaderMode = SHADERMODE_LIGHTING;
TIMEMODE currentTimeMode = TIME_DAY;

float currentTime = 0;
float timeStep = 0.05;

int winWidth = 1920;

int winHeight = 1080;

Font* font;

std::vector<Shader*> shaders;
std::vector<Model*> models;
std::vector<Light*> lights;
std::vector<Object*> objects;

Shader* currentShader;
Model* selectedModel;

Light* sunLight;
Light* torchLight;
Light* streetLamp1;
Light* streetLamp2;

Object* sun;

int selectedModelIndex = 0;

FPSViewer *camera;
glm::mat4 projection;

InputState Input;

glm::vec3 cameraPos(0, 5, 3);

float currentRot = 0.0;
double fovy = M_PI / 2;

HeightMap* heightMap;
Skybox* skybox;
Skybox* daySkybox;
Skybox* nightSkybox;

bool torchOn = false;
bool flyMode = true;

float currentHeight = 0.0f;

/**
*  Set the projection matrix. Takes into account window aspect ratio, so called
*  when the window is resized. All shaders in the program need to be updated
*/
void setProjection()
{
	glUseProgram(currentShader->getShaderProgram());
	projection = glm::perspective(fovy, double(winWidth) / double(winHeight), 0.01, 2000.0);

	int projHandleF = glGetUniformLocation(currentShader->getShaderProgram(), "projection_matrix");
	if (projHandleF == -1) {
		fprintf(stderr, "Error updating proj matrix\n");
		//exit(1);
	}
	glUniformMatrix4fv(projHandleF, 1, false, glm::value_ptr(projection));
}

/** Keyboard callback
* Closes if ESC is pressed
* R reloads the shaders
* WASD moves the camera
* N toggles between night/day
* T turns torch on off
* F toggles fly and walk mode
*/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Close the application by pressing Esc
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		if (currentShader->reloadShader())
			printf("Reloading shaders\n");
		setProjection();
	}
	else if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		if (currentTimeMode == TIME_DAY) {
			currentTimeMode = TIME_NIGHT;
			skybox = nightSkybox;
			sunLight->intensity = 0.01;
			sunLight->position = glm::vec4(-750, 225, -4, 0);
			sunLight->ambient = glm::vec3(0.5, 0.5, 0.5);
		}
		else {
			currentTimeMode = TIME_DAY;
			skybox = daySkybox;
			sunLight->intensity = 0.8;
			sunLight->position = glm::vec4(-13, 570, 525, 0);
			sunLight->ambient = glm::vec3(1.0, 1.0, 1.0);
		}
	}
	else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		if (torchOn) {
			torchOn = false;
			torchLight->coneAngle = 0;
		}
		else {
			torchOn = true;
			torchLight->coneAngle = 30;
		}
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		flyMode = !flyMode;
	}
	else if (key == GLFW_KEY_W) {
		camera->processKeyboard(FORWARD, currentHeight, flyMode);
	}
	else if (key == GLFW_KEY_S) {
		camera->processKeyboard(BACKWARD, currentHeight, flyMode);
	}
	else if (key == GLFW_KEY_A) {
		camera->processKeyboard(LEFT, currentHeight, flyMode);
	}
	else if (key == GLFW_KEY_D) {
		camera->processKeyboard(RIGHT, currentHeight, flyMode);
	}
}

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void mouse_pos_callback(GLFWwindow* window, double x, double y)
{
	Input.update((float)x, (float)y);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		Input.rMousePressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		Input.rMousePressed = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Input.lMousePressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		Input.lMousePressed = false;
	}

}

void reshape_callback(GLFWwindow *window, int x, int y)
{
	winWidth = x;
	winHeight = y;
	setProjection();
	glViewport(0, 0, x, y);
}

/**
* Renders each frame
*/
int render()
{
	currentShader->use();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*Get camera information*/
	camera->update(Input);
	glm::mat4 viewMatrix = camera->getViewMatrix();
	currentShader->setUniform("view_matrix", viewMatrix);

	glm::vec3 cameraPos = camera->getPosition();
	glm::vec3 cameraDir = camera->getDirection();

	currentHeight = heightMap->getHeightFromWorldSpace(cameraPos.x, cameraPos.z); //Get the terrains current height

	/*RENDER ALL OBJECTS*/
	for (int i = 0; i < objects.size(); i++) {
		objects.at(i)->render();
	}

	/* LIGHTING */
	sun->setPosition(glm::vec3(sunLight->position));
	sunLight->coneAngle = 180;

	torchLight->position = glm::vec4(cameraPos, 1);
	torchLight->coneDirection = glm::vec3(cameraDir);

	currentShader->setLighting(lights);

	/*Render Skybox / terrain*/
	heightMap->render(currentShader);
	skybox->render(viewMatrix, projection);

	currentShader->use();
	setProjection();

	/* Render text */
	font->renderText("T = Toggle torch", glm::vec3(0, winHeight - 25, 0), 0.5f);
	font->renderText("N = Toggle Day/Night", glm::vec3(0, winHeight - 50, 0), 0.5f);
	font->renderText("F = Toggle Fly/walk mode", glm::vec3(0, winHeight - 75, 0), 0.5f);
	font->renderText("WASD = MOVE", glm::vec3(0, winHeight - 100, 0), 0.5f);
	font->renderText("ESC = Exit", glm::vec3(0, winHeight - 125, 0), 0.5f);
	font->renderText("Terrain Height: " + std::to_string(currentHeight), glm::vec3(0, 50, 0), 0.5f);
	font->renderText("Position: X:" + std::to_string(cameraPos.x) + ", Y:" + std::to_string(cameraPos.y) + ", Z:" + std::to_string(cameraPos.z), glm::vec3(0, 0, 0), 0.5f);
	font->renderText("Direction: X:" + std::to_string(cameraDir.x) + ", Y:" + std::to_string(cameraDir.y) + ", Z:" + std::to_string(cameraDir.z), glm::vec3(0, 25, 0), 0.5f);
	
	glFlush();
	return 0;
}

int main(int argc, char* argv[])
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "Assignment 3 part 2", NULL, NULL);

	/* Set callbacks */
	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, reshape_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//SET MOUSE MODE
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (window == NULL) {
		std::cout << "Error: Couldn't create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Error: Failed to initialize GLEW" << std::endl;
		return -1;
	}

	int width;
	int height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	/*Setup font*/
	font = new Font();
	font->setupFont();

	/*Setup shaders*/
	Shader* debugShader = new Shader("shaders/shader.vert", "shaders/shader.frag", SHADERMODE_DEBUG);
	Shader* lightingShader = new Shader("shaders/light-texture.vert", "shaders/light-texture.frag", SHADERMODE_LIGHTING);

	shaders.push_back(debugShader);
	shaders.push_back(lightingShader);

	/*We want the shader to be set to this for loading textures*/
	currentShader = shaders.at(1);
	currentShaderMode = SHADERMODE_LIGHTING;

	/*SETUP LIGHTS*/
	/*SUN LIGHT */
	sunLight = new Light();
	sunLight->position = glm::vec4(-13, 570, 525, 0);
	sunLight->ambient = glm::vec3(1.0, 1.0, 1.0);
	sunLight->diffuse = glm::vec3(0.95, 0.95, 0.95);
	sunLight->specular = glm::vec3(0.5, 0.5, 0.5);
	lights.push_back(sunLight);

	/* TORCH LIGHT */
	torchLight = new Light();
	torchLight->position = glm::vec4(0.0, 0.0, 0.0, 1.0);
	torchLight->ambient = glm::vec3(0.0, 0.0, 0.0);
	torchLight->diffuse = glm::vec3(1.0, 1.0, 1.0);
	torchLight->specular = glm::vec3(0.2, 0.2, 0.2);
	torchLight->intensity = 0.1;
	torchLight->coneAngle = 0;
	torchLight->attenuation = 0.01;
	lights.push_back(torchLight);

	/*Street LIGHT */
	streetLamp1 = new Light();
	streetLamp1->position = glm::vec4(-31.0, 5, 15, 1.0);
	streetLamp1->ambient = glm::vec3(0.1, 0.1, 0.1);
	streetLamp1->diffuse = glm::vec3(1.0, 1.0, 1.0);
	streetLamp1->intensity = 1;
	streetLamp1->specular = glm::vec3(0.5, 0.5, 0.5);
	streetLamp1->coneAngle = 180;
	streetLamp1->attenuation = 0.005;
	lights.push_back(streetLamp1);

	/*Street LIGHT */
	streetLamp2 = new Light();
	streetLamp2->position = glm::vec4(-6.0, 5, -23, 1.0);
	streetLamp2->ambient = glm::vec3(0.1, 0.1, 0.1);
	streetLamp2->diffuse = glm::vec3(1.0, 1.0, 1.0);
	streetLamp2->intensity = 1;
	streetLamp2->specular = glm::vec3(0.5, 0.5, 0.5);
	streetLamp2->coneAngle = 180;
	streetLamp2->attenuation = 0.005;
	lights.push_back(streetLamp2);

	//Create cameras
	camera = new FPSViewer(cameraPos);

	Model* model = new Model(lightingShader);
	model->loadModel("models/house/House.obj", "models/house/");
	model->createVBO(currentShader->getShaderProgram());
	models.push_back(model);

	model = new Model(lightingShader);
	model->loadModel("models/tree/PineTree03.obj", "models/tree/");
	model->createVBO(currentShader->getShaderProgram());
	models.push_back(model);

	model = new Model(lightingShader);
	model->loadModel("models/cube-simple/cube-simple.obj", "models/cube-simple/");
	model->createVBO(currentShader->getShaderProgram());
	models.push_back(model);

	model = new Model(lightingShader);
	model->loadModel("models/streetlamp/streetlamp.obj", "models/streetlamp/");
	model->createVBO(currentShader->getShaderProgram());
	models.push_back(model);

	//Object instances
	objects.push_back(new Object(models.at(0), glm::vec3(10, 3.4, 10), glm::vec3(1, 1, 1)));
	objects.push_back(new Object(models.at(1), glm::vec3(2.4, 3.4, -11), glm::vec3(2.5, 2.5, 2.5)));
	objects.push_back(new Object(models.at(1), glm::vec3(-54, 13.2, -47), glm::vec3(2.5, 2.5, 2.5)));
	objects.push_back(new Object(models.at(1), glm::vec3(-67, 13.2, -31), glm::vec3(5, 5, 5)));
	objects.push_back(new Object(models.at(3), glm::vec3(-31, 3.4, 15), glm::vec3(1, 1, 1)));
	objects.push_back(new Object(models.at(3), glm::vec3(-6.1, 3.4, -23), glm::vec3(1, 1, 1)));
	sun = new Object(models.at(2), glm::vec3(sunLight->position), glm::vec3(50, 50, 50));
	objects.push_back(sun);

	selectedModel = models.at(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	currentShader = shaders.at(1);
	currentShaderMode = SHADERMODE_LIGHTING;

	setProjection();

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glDepthRange(0, 1);
	glEnable(GL_DEPTH_TEST);

	heightMap = new HeightMap("maps/heightmap6.bmp", 5.0);

	//Load skyboxes
	std::vector<std::string> dayFaces
	{
		"skyboxes/craterlake_rt.jpg",
		"skyboxes/craterlake_lf.jpg",
		"skyboxes/craterlake_up.jpg",
		"skyboxes/craterlake_bn.jpg",
		"skyboxes/craterlake_bk.jpg",
		"skyboxes/craterlake_ft.jpg",
	};

	std::vector<std::string> nightFaces
	{
		"skyboxes/mnight_rt.jpg",
		"skyboxes/mnight_lf.jpg",
		"skyboxes/mnight_up.jpg",
		"skyboxes/mnight_bn.jpg",
		"skyboxes/mnight_bk.jpg",
		"skyboxes/mnight_ft.jpg",
	};


	daySkybox = new Skybox(800, 600);
	nightSkybox = new Skybox(800, 600);
	daySkybox->setupSkybox(dayFaces);
	nightSkybox->setupSkybox(nightFaces);

	skybox = daySkybox;

	/*Start the main loop*/
	while (!glfwWindowShouldClose(window)) {
		float xDist, yDist;
		Input.readAcc(&xDist, &yDist);
		if (Input.rMousePressed) {

			selectedModel->fillScale += yDist;
			if (selectedModel->fillScale <= 0.01) {
				selectedModel->fillScale = 0.01;
			}
		}

		/*Way to get camera to rotate around in a circle*/
		currentRot += 0.01;
		if (currentRot >= 3.14) {
			currentRot = -3.14;
		}

		currentTime += timeStep;

		render();
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	for (int i = 0; i < models.size(); i++) {
		delete models.at(i);
	}

	for (int i = 0; i < lights.size(); i++) {
		delete lights.at(i);
	}

	glfwTerminate();
	return 0;
}