/*
* Material de Referência
* -> https://www.gathering4gardner.org/g4g13gift/math/RokickiTomas-GiftExchange-LifeAlgorithms-G4G13.pdf
* -> https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
*/

#include "Game.h"

struct Color
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

	mouseHoveringEntity.setColor(colors.mouseHover);
	
	actual_state = PROGRAM_STATE::idle;

	GLfloat vertices[12] = {
		0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	GLuint indices[6] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO, EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
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
			std::cout << "FrameRate: " << 1.0f / deltaTime << std::endl;
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
		if (checkMouseClick)
		{
			Entity entity(mouseClick, colors.liveCell);

			bool inserted = entities.insert(std::make_pair(entity.coord, entity)).second;

			if (!inserted)
				entities.erase(entity.coord);

			checkMouseClick = false;
		}
	}
	else if (actual_state == PROGRAM_STATE::active)
	{
		//if (tick == 0.0f)
		//{
			updateGeneration(&shader);
			std::cout << " container size:" << entities.size() << std::endl;
		//}
	}
}

void Game::render()
{
	Shader shader = ResourceManager::GetShader(SHADER::basic);
	shader.Use();

	glClearColor(33.0f / 255.0f, 42.0f / 255.0f, 62.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (actual_state == PROGRAM_STATE::idle)
	{
		mouseHoveringEntity.setCoord(mouseClick);
		mouseHoveringEntity.draw(VAO, &shader);
	}
	
	std::map<std::pair<int, int>, Entity>::iterator it;
	for (it = entities.begin(); it != entities.end(); it++)
		it->second.draw(VAO, &shader);
		
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

	//std::cout << mouseClick.x << " \ " << mouseClick.y << " \ " << mouseClick.z << std::endl;
}

void Game::updateGeneration(Shader *shader)
{
	std::map<std::pair<int, int>, Entity> nextGeneration;

	std::map<std::pair<int, int>, int> neighbours;

	// Rodeando pelas células vivas, marcando +1 para cada um de seus vizinhos
	for (std::map<std::pair<int, int>, Entity>::iterator entity = entities.begin(); entity != entities.end(); entity++)
	{
		std::pair<int, int> coord = entity->first;
		neighbours[std::pair<int, int>(coord.first - 1, coord.second - 1)] ++;
		neighbours[std::pair<int, int>(coord.first - 1, coord.second)] ++;
		neighbours[std::pair<int, int>(coord.first - 1, coord.second + 1)] ++;
		neighbours[std::pair<int, int>(coord.first, coord.second - 1)] ++;
		neighbours[std::pair<int, int>(coord.first, coord.second + 1)] ++;
		neighbours[std::pair<int, int>(coord.first + 1, coord.second - 1)] ++;
		neighbours[std::pair<int, int>(coord.first + 1, coord.second)] ++;
		neighbours[std::pair<int, int>(coord.first + 1, coord.second + 1)] ++;
	}

	// Se este vizinho tiver exatamente 3 células vivas a sua volta ele volta a vida
	// ou se ele tiver dois vizinhos porém este Neighbour já for uma célula viva, então ele continua vivo
	for (std::map<std::pair<int, int>, int>::iterator neighbour = neighbours.begin(); neighbour != neighbours.end(); neighbour++)
	{
		if (neighbour->second == 3 || (neighbour->second == 2 && entities.find(neighbour->first) != entities.end()))
		{
			Entity entity(glm::vec3(neighbour->first.first, neighbour->first.second, 0.0f), colors.liveCell);
			nextGeneration.insert(std::make_pair(neighbour->first, entity));
		}
	}
	entities = nextGeneration;
}
