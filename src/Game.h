#pragma once
#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "ResourceManager.h"
#include "Shader.h"

class Game
{
public:
	Game();

	void init();

	void run();

private:
	sf::Window window;
	bool grid[8][8]; // tamanho definido de 8 por 8 para ver melhor, depois aumenta

	void processInput();
	void update();
	void render();
};

