/*
* Revisar Grid -> https://stackoverflow.com/questions/1482394/another-game-of-life-question-infinite-grid
* -> https://en.wikipedia.org/wiki/Hashlife
*/

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

	for (size_t i = 0; i < grid_width; i++)
	{
		std::vector<Entity> entity_line;
		for (size_t j = 0; j < grid_height; j++)
		{
			Entity entity;
			entity.alive = 0;
			entity_line.push_back(entity);
		}
		entities.push_back(entity_line);
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
	Shader shader = ResourceManager::GetShader(SHADER::basic);

	int k = 0;
	for (int i = 0; i < grid_width; i++)
	{
		for (int j = 0; j < grid_height; j++)
		{
			glm::vec3 vert1, vert2, vert3, vert4;
			glm::mat4 translate = glm::translate(entities[i][j].model, glm::vec3((entities[i][j].size * i), (entities[i][j].size * j), 0.0f));
			entities[i][j].model = translate;
			entities[i][j].bottom = entities[i][j].model * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
			entities[i][j].top = entities[i][j].model * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
			completeGrid.push_back(translate * glm::vec4(vertices[0], vertices[1], vertices[2], 1.0f));
			completeGrid.push_back(translate * glm::vec4(vertices[3], vertices[4], vertices[5], 1.0f));
			completeGrid.push_back(translate * glm::vec4(vertices[6], vertices[7], vertices[8], 1.0f));
			completeGrid.push_back(translate * glm::vec4(vertices[9], vertices[10], vertices[11], 1.0f));
			
			indicesGrid.push_back(indices[0] + k);
			indicesGrid.push_back(indices[1] + k);
			indicesGrid.push_back(indices[2] + k);
			indicesGrid.push_back(indices[3] + k);
			indicesGrid.push_back(indices[4] + k);
			indicesGrid.push_back(indices[5] + k);
			k += 4;
		}
	}
	
	actual_state = PROGRAM_STATE::idle;

	// Inicialização dos dados para renderizar
	GLuint completeGridVBO, completeGridEBO;

	glGenVertexArrays(1, &completeGridVAO);
	glBindVertexArray(completeGridVAO);

	// criando um Buffer de memória
	glGenBuffers(1, &completeGridVBO);
	glGenBuffers(1, &completeGridEBO);

	//=========================================//
	// Linkando este ID para um Buffer do tipo GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, completeGridVBO);
	// Copiando os dados do nosso Array para o Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * completeGrid.size(), &(completeGrid[0].x), GL_STATIC_DRAW);

	// Bind Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, completeGridEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indicesGrid.size(), &indicesGrid[0], GL_STATIC_DRAW);

	glBindVertexArray(completeGridVAO);
	// Configurando como vai ser a leitura dos dados salvos no buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Enviando o index referente ao { layout (location = 0) }
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	
	//=====================================//
	GLuint singleCellVBO, singleCellEBO;

	glGenVertexArrays(1, &singleCellVAO);
	glBindVertexArray(singleCellVAO);

	glGenBuffers(1, &singleCellVBO);
	glGenBuffers(1, &singleCellEBO);


	glBindBuffer(GL_ARRAY_BUFFER, singleCellVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, singleCellEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
			UCam.cameraPos += UCam.cameraFront * (event.mouseWheelScroll.delta * (UCam.cameraPos.z * 2.0f)) * deltaTime;
			break;
			/*
				Todo esse modelo de arrastar com o mouse vai ter que mudar
				talvez eu consiga pensar no futuro em como realmente prender o
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
		for (int i = 0; i < grid_width; i++)
		{
			for (int j = 0; j < grid_height; j++)
			{
				if (mouseClick.x > entities[i][j].bottom.x && mouseClick.x < entities[i][j].top.x /*Horizontal*/ && /*Vertical*/ mouseClick.y > entities[i][j].bottom.y && mouseClick.y < entities[i][j].top.y)
				{
					if (checkMouseClick)
					{
						entities[i][j].alive = !entities[i][j].alive;
						checkMouseClick = false;
					}
						
					if (!entities[i][j].mouseHover)
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(completeGridVAO);
	glm::vec3 deadColor = glm::vec3(33.0f / 255.0f, 42.0f / 255.0f, 62.0f / 255.0f);
	shader.SetVector3f("entityColor", deadColor);
	shader.SetMatrix4("model", glm::mat4(1.0f));
	glDrawElements(GL_TRIANGLES, indicesGrid.size(), GL_UNSIGNED_INT, 0); // 

	
	glBindVertexArray(singleCellVAO);
	for (int i = 0; i < grid_width; i++)
	{
		for (int j = 0; j < grid_height; j++)
		{
			if (!entities[i][j].alive && !entities[i][j].mouseHover)
				continue;

			if (!entities[i][j].alive && entities[i][j].mouseHover)
				entities[i][j].color = { 57.0f / 255.0f, 72.0f / 255.0f, 103.0f / 255.0f };
			else if (entities[i][j].alive)
				entities[i][j].color = { 241.0f / 255.0f, 246.0f / 255.0f, 249.0f / 255.0f };

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
	std::vector<std::vector<Entity>> entitiesNextTick;
	
	entitiesNextTick = entities;

	for (int i = 0; i < grid_width; i++)
	{
		for (int j = 0; j < grid_height; j++)
		{
			if (i == 0 || j == 0 || i == (grid_width-1) || j == (grid_height-1))
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

	entities = entitiesNextTick;
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
