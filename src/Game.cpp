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
			if(i % 2 == 0)
				entities[i][j].alive = 0;
			else 
				entities[i][j].alive = 1;
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

	ResourceManager::LoadShader("data\\shaders\\basic_V.glsl", "data\\shaders\\basic_F.glsl", "basicShader");


	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			glm::mat4 scale = glm::scale(entities[i][j].model, glm::vec3(entities[i][j].scale, entities[i][j].scale, 1.0f));
			glm::mat4 translate = glm::translate(entities[i][j].model, glm::vec3((entities[i][j].scale * i), (entities[i][j].scale * j), 0.0f));
			entities[i][j].model = translate * scale;
		}
	}
	
}

void Game::run()
{
	while (window.isOpen())
	{
		processInput();

		update();

		render();
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
			win_width = event.size.width;
			win_height = event.size.height;
			glViewport(0, 0, win_width, win_height);
			break;
		case sf::Event::MouseWheelScrolled:
				UCam.cameraPos += (event.mouseWheelScroll.delta * 0.5f) * UCam.cameraFront;
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
				mousePosStart = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
			break;
		case sf::Event::MouseMoved:
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				mousePos = sf::Mouse::getPosition(window);
				float difference = (mousePos.x - mousePosStart.x) % 10;
				std::cout << difference << std::endl;
				if (mousePos.x < mousePosStart.x)
				{
					UCam.cameraPos += glm::normalize(glm::cross(UCam.cameraFront, UCam.cameraUp)) * (0.05f * -difference);
				}
				else if (mousePos.x > mousePosStart.x)
				{
					UCam.cameraPos -= glm::normalize(glm::cross(UCam.cameraFront, UCam.cameraUp)) * (0.05f * difference);
				}
				mousePosStart = mousePos;
			}
			break;
		default:
			break;
		}
	}

	
}

void Game::update()
{
	projection = glm::perspective(glm::radians(45.0f), (float)win_width / (float)win_height, 0.1f, 100.0f);

	Shader shader = ResourceManager::GetShader("basicShader");
	shader.SetMatrix4("projection", projection, true);
	shader.SetMatrix4("view", UCam.getView(), true);
}

void Game::render()
{
	Shader shader = ResourceManager::GetShader("basicShader");
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
			glm::vec3 entity_color;
			if (!entities[i][j].alive)
				entity_color = { 128.0f / 255.0f, 85.0f / 255.0f, 140.0f / 255.0f };
			else
				entity_color = { 203.0f / 255.0f, 160.0f / 255.0f, 174.0f / 255.0f };
			shader.SetVector3f("entityColor", entity_color);
			shader.SetMatrix4("model", entities[i][j].model);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			
		}
	
	}	
	glBindVertexArray(0);

	window.display();
}
