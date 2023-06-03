#include "Game.h"

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

Game::Game(int win_width, int win_height)
{
	this->win_width = win_width;
	this->win_height = win_height;

	for (size_t i = 0; i < grid_size; i++)
	{
		for (size_t j = 0; j < grid_size; j++)
		{
			entities[i][j].alive = 0;
		}
	}
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

	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			glm::mat4 translate = glm::translate(entities[i][j].model, glm::vec3((entities[i][j].size * i), (entities[i][j].size * j), 0.0f));
			entities[i][j].model = translate;
		}
	}
	
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
			UCam.cameraPos += UCam.cameraFront * (event.mouseWheelScroll.delta * (UCam.cameraPos.z * 2.0f)) * deltaTime;
			break;
			/*
				Todo esse modelo de arrastar com o mouse vai ter que mudar
				e muito ainda, talvez eu consiga pensar no futuro em como realmente prender o
				cursor do mouse no grid e conseguir mover a câmera por uma velocidade fixa
				para trazer a ilusão de que está realmente arrastando o grid para os lados
				por enquanto fica desse jeito errado para facilitar a visualização
			*/
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

			processMouseMove(event.mouseMove.x, event.mouseMove.y);

			if (rightMousePressed) // sf::Mouse::isButtonPressed(sf::Mouse::Right) 
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
		for (int i = 0; i < grid_size; i++)
		{
			for (int j = 0; j < grid_size; j++)
			{
				glm::vec3 cellBottom = entities[i][j].model * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
				glm::vec3 cellTop = entities[i][j].model * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);

				if (mouseClick.x > cellBottom.x && mouseClick.x < cellTop.x /*Horizontal*/ && /*Vertical*/ mouseClick.y > cellBottom.y && mouseClick.y < cellTop.y)
				{
					if (checkMouseClick)
					{
						entities[i][j].alive = !entities[i][j].alive;
						checkMouseClick = false;
					}
						
					if(!entities[i][j].mouseHover)
						entities[i][j].mouseHover = true;
				}
				else
				{
					entities[i][j].mouseHover = false;
				}
			}
		}
		
	}
	else if (actual_state == PROGRAM_STATE::active)
	{
		if(tick == 0.0f)
			updateTickOfGrid();
	}
}

void Game::render()
{
	Shader shader = ResourceManager::GetShader(SHADER::basic);
	shader.Use();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// criando um Buffer de memória
	GLuint VBO; // Vertex Buffer Object 
	glGenBuffers(1, &VBO);

	GLuint EBO;
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// Linkando este ID para um Buffer do tipo GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Copiando os dados do nosso Array para o Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Configurando como vai ser a leitura dos dados salvos no buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Enviando o index referente ao { layout (location = 0) }
	glEnableVertexAttribArray(0);


	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			if (!entities[i][j].alive && entities[i][j].mouseHover)
				entities[i][j].color = { 57.0f / 255.0f, 72.0f / 255.0f, 103.0f / 255.0f };
			else if (entities[i][j].alive)
				entities[i][j].color = {241.0f / 255.0f, 246.0f / 255.0f, 249.0f / 255.0f};
			else if (!entities[i][j].alive)
				entities[i][j].color = { 33.0f / 255.0f, 42.0f / 255.0f, 62.0f / 255.0f };

			shader.SetVector3f("entityColor", entities[i][j].color);
			shader.SetMatrix4("model", entities[i][j].model);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			
		}
	
	}	
	glBindVertexArray(0);

	window.display();
}

void Game::updateTickOfGrid()
{
	Entity entitiesNextTick[grid_size][grid_size];
	
	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			entitiesNextTick[i][j].alive = entities[i][j].alive;
		}
	}

	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			if (i == 0 || j == 0 || i == (grid_size-1) || j == (grid_size-1))
			{
				entitiesNextTick[i][j].alive = 0;
				continue;
			}

			int neighbors[8] = { entities[i - 1][j].alive, entities[i + 1][j].alive, entities[i][j - 1].alive, entities[i][j + 1].alive,
							entities[i - 1][j - 1].alive, entities[i - 1][j + 1].alive, entities[i + 1][j + 1].alive, entities[i + 1][j - 1].alive };
			
			int alive_neighbours = 0;
			for (int k = 0; k < 8; k++)
			{
				if (neighbors[k] == 1)
					alive_neighbours++;
			}

			if (alive_neighbours == 3)
			{
				if (entities[i][j].alive == 0)
					entitiesNextTick[i][j].alive = 1;
			}

			if (alive_neighbours < 2 || alive_neighbours > 3)
			{
				if (entities[i][j].alive == 1)
					entitiesNextTick[i][j].alive = 0;
			}
		}
	}

	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			entities[i][j].alive = entitiesNextTick[i][j].alive;
		}
	}
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
