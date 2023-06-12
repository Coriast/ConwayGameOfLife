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
	void init();

	void draw(glm::vec3 translate, glm::vec3 color, Shader shader);

private:
	GLuint VAO;
};

