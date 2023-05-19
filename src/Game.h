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
	glm::mat4 model = glm::mat4(1.0f);
};

class Game
{
public:
	Game(int win_width, int win_height);

	void init();

	void run();

private:
	glm::mat4 projection, view;
	int win_width, win_height;
	static const int grid_size = 10;
	sf::Window window;
	sf::Clock clock;
	Entity entities[grid_size][grid_size]; // tamanho definido de 8 por 8 para ver melhor, depois aumenta

	void processInput();
	void update();
	void render();
};

