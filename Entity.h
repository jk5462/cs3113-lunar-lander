#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum EntityType { PLAYER, PLATFORM };

class Entity {
public:

	EntityType entityType;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	bool isStatic;
	bool isActive;

	//glm::vec3 movement;
	//float speed;

	float width;
	float height;

	float vertices[12];

	GLuint textureID;

	Entity();

	bool CheckLanding(Entity goal);

	//void CheckCollisionsY(Entity* objects, int objectCount);
	//void CheckCollisionsX(Entity* objects, int objectCount);

	bool CheckCollision(Entity other);

	void Update(float deltaTime, Entity* tiles, int tileCount, Entity goal, Entity* success, Entity* fail);
	void Render(ShaderProgram* program);
};



