#pragma once
#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Image.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iomanip>
#include <fstream>
#include "Enum.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "UglyCam.h"
#include "Entity.h"
#include "Icon.h"

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
	sf::Window			window;
	sf::Clock			clock;
	UglyCam				UCam;
	sf::Vector2i		mousePos, mousePosStart;
	glm::vec3			mouseClick;
	bool				rightMousePressed = false, checkMouseClick = false;
	PROGRAM_STATE		actual_state;

	Entity mouseHoveringEntity;
	std::map<std::pair<int, int>, Entity> entities;

	GLuint VAO;

	void processInput();
	void update();
	void render();

	void processMouseMove(float x, float y);
	void updateGeneration(Shader *shader);
};

