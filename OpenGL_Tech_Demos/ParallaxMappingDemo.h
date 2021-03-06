#ifndef PARALLAX_MAPPING_DEMO_H
#define PARALLAX_MAPPING_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include "Math\vector3.h"
#include "Math\matrix4.h"
#include "Math\MathHelper.h"
#include "camera.h"

class ParallaxMappingDemo
{
public:
	ParallaxMappingDemo();
	~ParallaxMappingDemo();

	bool initalizeScene;
	bool enableParallax;
	bool pauseLight;
	float heightScale;

	void Initalize(vector3 &cameraPos);
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	vector3 lightPos;
	Shader *shaderParallax, *shaderLightBox;
	GLuint diffuseMap_SquareBricksID, normalMap_SquareBricksID, displacementMap_SquareBricksID;
	GLuint diffuseMap_GreyStonewallID, normalMap_GreyStonewallID, displacementMap_GreyStonewallID;
	GLuint diffuseMap_PebblesID, normalMap_PebblesID, displacementMap_PebblesID;
};

#endif