#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glad/glad.h>
#include <iostream>
#include <cmath>
#include "Shader.h"

class Entity
{
public:
	std::pair<int, int> coord;
	glm::vec3 color;

	Entity();
	Entity(glm::vec3 coordenates, glm::vec3 color);

	void draw(GLuint VAO, Shader *shader);

	void setCoord(glm::vec3 coordenates);
	void setColor(glm::vec3 color);

private:
	GLuint VAO;
};

