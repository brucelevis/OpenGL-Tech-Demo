#include "InstancingDemo.h"

InstancingDemo::InstancingDemo()
	: amount(3000), initalizeScene(true)
{
	modelMatrices = new Matrix4[amount];
}

InstancingDemo::~InstancingDemo()
{
	delete[] modelMatrices;

	if (grassTextureID)
		glDeleteTextures(1, &grassTextureID);
	if (groundTextureID)
		glDeleteTextures(1, &groundTextureID);
}

struct GrassBlade
{
	GLuint grassVAO, grassVBO;

	GLfloat quadVertices[20] = {
		// Positions			// Texture Coords
		-1.0f,	1.0f, 0.0f,		0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,		0.0f, 1.0f,
		 1.0f,	1.0f, 0.0f,		1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,		1.0f, 1.0f,
	};
}; GrassBlade grassBlade;

void InstancingDemo::InitalizeScene()
{
	if (initalizeScene)
	{
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		shaderInstancing = ResourceManager::GetShader("Instancing");
		shaderInstancing->Use();
		glUniform1i(glGetUniformLocation(shaderInstancing->Program, "texture1"), 1);
		shaderDirtGround = ResourceManager::GetShader("EnviromentObject");
		shaderDirtGround->Use();
		glUniform1i(glGetUniformLocation(shaderDirtGround->Program, "diffuseTexture"), 0);

		modelRock = ResourceManager::GetModel("Rock");

		grassTextureID = ResourceManager::LoadTexture("Resources/grass.png", true);
		groundTextureID = ResourceManager::LoadTexture("Resources/brickwall.jpg");

		srand(glfwGetTime());
		GLfloat radius = 5.0f;
		GLfloat offset = 25.0f;
		Matrix4 model = Matrix4();
		Matrix4 translate = Matrix4();
		Matrix4 rotate = Matrix4();
		Matrix4 scale = Matrix4();
		GLfloat scaleValue = 0.0f;
		for (GLuint i = 0; i < amount; i++)
		{
			if (i % 2 == 0)
			{
				model = Matrix4();
				rotate = Matrix4();
				translate = Matrix4();
				GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
				GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
				GLfloat x = sin(angle) * radius + displacement;
				displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
				GLfloat z = cos(angle) * radius + displacement;
				translate = translate.translate(vector3(x, 0.0f, z));
			}
			else
				rotate = rotate.rotate(MathHelper::DegressToRadians(90.0f), vector3(0.0f, 1.0f, 0.0f));

			model = rotate * translate;

			modelMatrices[i] = model;
		}

		SetUpBuffers(grassBlade.grassVAO, modelMatrices, grassBlade.grassVBO, sizeof(grassBlade.quadVertices), grassBlade.quadVertices);

		srand(glfwGetTime());
		for (GLuint i = 0; i < amount; i++)
		{
			model = Matrix4();
			GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
			GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
			GLfloat x = sin(angle) * radius + displacement;
			displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
			GLfloat z = cos(angle) * radius + displacement;
			translate = translate.translate(vector3(x, -1.0f, z));

			scaleValue = (rand() % 20) / 100.0f + 0.05f;
			scale = scale.scale(vector3(scaleValue, scaleValue, scaleValue));

			model = scale * translate;

			modelMatrices[i] = model;
		}

		for (GLuint i = 0; i < modelRock->meshes.size(); i++)
			SetUpBuffers(modelRock->meshes[i].VAO, modelMatrices);

		initalizeScene = false;
	}
}

void InstancingDemo::SetUpBuffers(GLuint &vao, Matrix4 *matrices, GLuint vbo, int sizeOfVertices, GLfloat *vertices)
{
	GLuint buffer;
	glBindVertexArray(vao);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(Matrix4), &matrices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (sizeOfVertices != NULL && vertices != NULL)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeOfVertices, vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}

	// Vertex Attributes
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)(sizeof(vector4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)(2 * sizeof(vector4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)(3 * sizeof(vector4)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	// Clean up
	glBindVertexArray(0);
	glDeleteBuffers(1, &buffer);
}

void InstancingDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderDirtGround->Use();
	glUniform1i(glGetUniformLocation(shaderDirtGround->Program, "diffuseTexture"), 0);

	Matrix4 projection = Matrix4();
	projection = projection.perspectiveProjection(camera.zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 1.0f, 3000.0f);
	Matrix4 view;
	view = camera.GetViewMatrix();
	Matrix4 model = Matrix4();
	Matrix4 scale = Matrix4();
	Matrix4 translate = Matrix4();

	shaderInstancing->Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderInstancing->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderInstancing->Program, "view"), 1, GL_FALSE, &view.data[0]);

	// Render the grass quads.
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTextureID);
	glBindVertexArray(grassBlade.grassVAO);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, amount);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Render the rocks.
	glBindTexture(GL_TEXTURE_2D, modelRock->textures_loaded[0].id);
	for (GLuint i = 0; i < modelRock->meshes.size(); i++)
	{
		glBindVertexArray(modelRock->meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, modelRock->meshes[i].vertices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Render the plane.
	shaderDirtGround->Use();
	model = Matrix4();
	translate = translate.translate(vector3(0.0f, -0.95f, 0.0f));
	scale = scale.scale(vector3(31.0f, 0.0f, 31.0f));
	model = scale * translate;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundTextureID);
	glUniformMatrix4fv(glGetUniformLocation(shaderDirtGround->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderDirtGround->Program, "view"), 1, GL_FALSE, &view.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderDirtGround->Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderPlane(1.0f, 25.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
}