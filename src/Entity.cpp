#include "Entity.h"

Entity::Entity() {}

Entity::Entity(glm::vec3 coordenates, glm::vec3 color)
{
	setCoord(coordenates);
	setColor(color);
}

void Entity::draw(GLuint VAO, Shader *shader)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(coord.first, coord.second, 0.0f));

	shader->SetMatrix4("model", model, true);
	shader->SetVector3f("entityColor", color, true);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Entity::setCoord(glm::vec3 coordenates)
{
	coord.first = std::round(coordenates.x);
	coord.second = std::round(coordenates.y);
}

void Entity::setColor(glm::vec3 color)
{
	this->color = color;
}
