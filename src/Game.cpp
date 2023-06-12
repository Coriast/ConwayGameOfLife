/*
* Revisar Grid -> https://stackoverflow.com/questions/1482394/another-game-of-life-question-infinite-grid
* -> https://en.wikipedia.org/wiki/Hashlife
*/

#include "Game.h"

static struct Color
{
	glm::vec3 mouseHover = { 57.0f / 255.0f, 72.0f / 255.0f, 103.0f / 255.0f };
	glm::vec3 deadCell = { 33.0f / 255.0f, 42.0f / 255.0f, 62.0f / 255.0f };
	glm::vec3 liveCell = { 241.0f / 255.0f, 246.0f / 255.0f, 249.0f / 255.0f };
};

Color colors;

Game::Game(int win_width, int win_height)
{
	this->win_width = win_width;
	this->win_height = win_height;
}

void Game::init()
{
	sf::ContextSettings settings;
	settings.majorVersion	= 3;
	settings.minorVersion	= 3;
	settings.attributeFlags = sf::ContextSettings::Attribute::Core;

	window.create(sf::VideoMode(win_width, win_height), "Conway", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);
	window.setActive(true);

	gladLoadGLLoader((GLADloadproc)sf::Context::getFunction);

	glViewport(0, 0, win_width, win_height);

	ResourceManager::LoadShader("./data/shaders/basic_V.glsl", "./data/shaders/basic_F.glsl", SHADER::basic); 
	Shader shader = ResourceManager::GetShader(SHADER::basic);

	entity.init();
	
	actual_state = PROGRAM_STATE::idle;
}

void Game::run()
{
	while (window.isOpen())
	{
		if (tick >= 0.10f)
			tick = 0.0f;
			
		processInput();

		update();

		render();
		
		float currentFrame = clock.getElapsedTime().asSeconds();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		tick += deltaTime;
		frameRateTick += deltaTime;

		if (frameRateTick >= 1.0f)
		{
			frameRateTick = 0.0f;
			//std::cout << "FrameRate: " << 1.0f / deltaTime << std::endl;
		}
	}
}

// Metodos Privados

void Game::processInput()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::Resized:
			win_width = event.size.width; win_height = event.size.height;
			glViewport(0, 0, win_width, win_height);
			break;
		case sf::Event::MouseWheelScrolled:
			if(UCam.cameraPos.z <= 2000.0f || UCam.cameraPos.z > 0.0f)
				UCam.cameraPos += UCam.cameraFront * (event.mouseWheelScroll.delta * (UCam.cameraPos.z * 2.0f)) * deltaTime;
			break;
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Right)
			{
				mousePosStart = sf::Mouse::getPosition(window);
				rightMousePressed = true;
			}
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				checkMouseClick = true;
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Right)
				rightMousePressed = false;
			break;
		case sf::Event::MouseMoved:
		{
			processMouseMove(event.mouseMove.x, event.mouseMove.y);

			if (rightMousePressed)
			{
				mousePos = sf::Mouse::getPosition(window);
				sf::Vector2i cameraVelocity = (mousePos - mousePosStart);

				glm::vec3 cameraRight = glm::normalize(glm::cross(UCam.cameraFront, UCam.cameraUp));
				if (mousePos.x < mousePosStart.x)
				{
					UCam.cameraPos += cameraRight * (-cameraVelocity.x * deltaTime);
				}
				else if (mousePos.x > mousePosStart.x)
				{
					UCam.cameraPos -= cameraRight * (cameraVelocity.x * deltaTime);
				}

				if (mousePos.y < mousePosStart.y)
				{
					UCam.cameraPos += glm::normalize(glm::cross(UCam.cameraFront, cameraRight)) * (-cameraVelocity.y * deltaTime);
				}
				else if (mousePos.y > mousePosStart.y)
				{
					UCam.cameraPos -= glm::normalize(glm::cross(UCam.cameraFront, cameraRight)) * (cameraVelocity.y * deltaTime);
				}

				mousePosStart = mousePos;
			}
			break;
		}
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::A)
				actual_state = PROGRAM_STATE::active;
			if (event.key.code == sf::Keyboard::I)
				actual_state = PROGRAM_STATE::idle;
			break;
		default:
			break;
		}
	}

	
}

void Game::update()
{
	projection = glm::perspective(glm::radians(45.0f), (float)win_width / (float)win_height, 0.1f, 2000.0f);

	Shader shader = ResourceManager::GetShader(SHADER::basic);
	shader.SetMatrix4("projection", projection, true);
	shader.SetMatrix4("view", UCam.getView(), true);

	if (actual_state == PROGRAM_STATE::idle)
	{

	}
	else if (actual_state == PROGRAM_STATE::active)
	{
		if (tick == 0.0f)
		{

		}
	}
}

void Game::render()
{
	Shader shader = ResourceManager::GetShader(SHADER::basic);
	shader.Use();

	glClearColor(33.0f / 255.0f, 42.0f / 255.0f, 62.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if(actual_state == PROGRAM_STATE::idle)
		entity.draw(mouseClick, colors.mouseHover, ResourceManager::GetShader(SHADER::basic));

	glBindVertexArray(0);
	window.display();	
}

void Game::processMouseMove(float x, float y)
{
	// coordenadas em Screen Space
	glm::vec4 CalcMouse = glm::vec4(x, y, -1.0f, 1.0f);

	// Transformadas para Clip Space [-1, 1]
	CalcMouse.x = (2.0f * CalcMouse.x) / win_width - 1.0f;
	CalcMouse.y = 1.0f - (2.0f * CalcMouse.y) / win_height;

	CalcMouse = glm::inverse(projection) * CalcMouse;

	CalcMouse = glm::vec4(CalcMouse.x, CalcMouse.y, -1.0f, 0.0f);

	CalcMouse = glm::inverse(UCam.getView()) * CalcMouse;

	mouseClick = (glm::normalize(glm::vec3(CalcMouse.x, CalcMouse.y, CalcMouse.z)));

	// Matemática daqui -> https://antongerdelan.net/opengl/raycasting.html
	// Neste caso já sei antecipadamente que quero testar o rayCast no Z = 0
	glm::vec3 O = UCam.cameraPos;
	glm::vec3 D = mouseClick;
	glm::vec3 n = glm::vec3(0.0f, 0.0f, 1.0f);
	float d = 0; // mantendo variável para caso for usar em outros casos
	// Queremos checar no plano 0, utilizando apenas a distância do 
	// nosso ponto de origem até a normal do nosso plano em Z = 0
	float t = -((O * n + d) / (mouseClick * n)).z;
	glm::vec3 rayCast = O + D * t;

	mouseClick = rayCast;

	std::cout << mouseClick.x << " \ " << mouseClick.y << " \ " << mouseClick.z << std::endl;
}
