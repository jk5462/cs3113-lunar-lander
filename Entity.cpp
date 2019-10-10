#include "Entity.h"

Entity::Entity()
{
	entityType = PLATFORM;
	position = glm::vec3(0);
	width = 1;
	height = 1;

	vertices[0] = -0.5;
	vertices[1] = -0.5;

	vertices[2] = 0.5;
	vertices[3] = -0.5;

	vertices[4] = 0.5;
	vertices[5] = 0.5;

	vertices[6] = -0.5;
	vertices[7] = -0.5;

	vertices[8] = 0.5;
	vertices[9] = 0.5;

	vertices[10] = -0.5;
	vertices[11] = 0.5;

	isStatic = true;
	isActive = true;
}

bool Entity::CheckLanding(Entity goal)
{
	if (CheckCollision(goal)) {
		float xdist = fabs(position.x - goal.position.x);

		if (xdist < 0.5)
		{
			return true;
		}
	}

	return false;
}

bool Entity::CheckCollision(Entity other)
{
	float xdist = fabs(position.x - other.position.x) - ((width + other.width) / 2.0f);
	float ydist = fabs(position.y - other.position.y) - ((height + other.height) / 2.0f);

	if (xdist < 0 && ydist < 0)
	{
		return true;
	}

	return false;
}

void Entity::Update(float deltaTime, Entity* tiles, int tileCount, Entity goal, Entity* success, Entity* fail)
{
	if (isStatic == false) {
		velocity += acceleration * deltaTime;

		
		position.y += velocity.y * deltaTime; // Move on Y

		position.x += velocity.x * deltaTime; // Move on X
		

		for (int i = 0; i < tileCount; i++) {
			if (CheckCollision(tiles[i])) {
				velocity = glm::vec3(0, 0, 0);
				acceleration = glm::vec3(0, 0, 0);
				isStatic = true;
				//Fail
				fail->isActive = true;
			}
		}

		if (position.x > 4.5 || position.x < -4.5) {
			velocity = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			isStatic = true;
			//Fail
			fail->isActive = true;
		}

		if (CheckLanding(goal)) {
			velocity = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			isStatic = true;
			//Success
			success->isActive = true;
		}
	}
}

void Entity::Render(ShaderProgram* program) {
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	program->SetModelMatrix(modelMatrix);

	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	glBindTexture(GL_TEXTURE_2D, textureID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

