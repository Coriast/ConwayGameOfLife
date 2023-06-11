#pragma once
#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iomanip>
#include "Enum.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "UglyCam.h"

struct Entity
{
	bool alive, mouseHover = false;
	float size = 1;
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 color = glm::vec3(0.0f), top, bottom;
};

class Game
{
public:
	Game(int win_width, int win_height);

	void init();

	void run();

private:
	float				deltaTime, lastFrame = 0.0f, tick = 0.0f, frameRateTick;
	glm::mat4			projection;
	int					win_width, win_height;
	static const int	grid_height = 200, grid_width = 500;
	sf::Window			window;
	sf::Clock			clock;
	UglyCam				UCam;
	sf::Vector2i		mousePos, mousePosStart;
	glm::vec3			mouseClick;
	bool				rightMousePressed = false, checkMouseClick = false;
	PROGRAM_STATE		actual_state;

	std::vector<std::vector<Entity>> entities;
	std::vector<glm::vec3> completeGrid;
	std::vector<int> indicesGrid;

	GLuint completeGridVAO;
	GLuint singleCellVAO;

	void processInput();
	void update();
	void render();

	void updateTickOfGrid();
	void processMouseMove(float x, float y);
};

