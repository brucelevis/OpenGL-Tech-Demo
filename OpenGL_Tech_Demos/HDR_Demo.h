#ifndef HRD_DEMO_H
#define HDR_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include <vector>
#include "camera.h"
#include "shader.h"
#include <SOIL\SOIL.h>

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class HDR_DEMO
{
public:
	HDR_DEMO();
	~HDR_DEMO();

	GLfloat exposure = 0.05f;
	bool renderLights;
	bool initalizeScene;

	void InitalizeScene(GLsizei screenWidth, GLsizei screenHeight);
	void UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, bool resized);

private:
	GLboolean hrd = true; // NOTE(Darren): Do i need this??
	GLboolean bloom = true;
	GLuint woodTexture;
	std::vector<vector3> lightPositions;
	std::vector<glm::vec3> lightColors;
	Shader *shaderHDR, *shaderBloom, *shaderBlur, *shaderLightBox;
	GLuint hdrFBO;
	GLuint colorBuffers[2];
	GLuint rboDepth;
	GLuint pingpongFBO[2];
	GLuint pingpongColorbuffers[2];

	void SetupBuffers(GLsizei screenWidth, GLsizei screenHeight);
};

#endif