#pragma once
#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "ResourceManager.h"
#include "Shader.h"

struct Entity
{
	bool alive;

	GLfloat vertices[12] = {
		// positions
		0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	GLuint indices[6] = {
		0, 1, 3,
		1, 2, 3
	};

	// transformation matrices ????
};

class Game
{
public:
	Game();

	void init();

	void run();

private:
	static const int grid_size = 8;
	sf::Window window;
	sf::Clock clock;
	Entity entities[grid_size][grid_size]; // tamanho definido de 8 por 8 para ver melhor, depois aumenta

	void processInput();
	void update();
	void render();
};

