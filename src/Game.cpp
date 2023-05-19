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

	projection = glm::ortho(0.0f, (float)win_width, 0.0f, (float)win_height, 0.1f, 100.0f);
	view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));

	Shader shader = ResourceManager::GetShader("basicShader");
	shader.SetMatrix4("projection", projection, true);
	shader.SetMatrix4("view", view, true);

	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			float cell_width = (float)win_width / (float)grid_size, cell_height = (float)win_height / (float)grid_size;
			glm::mat4 scale = glm::scale(entities[i][j].model, glm::vec3(cell_width-1.f, cell_height-1.f, 1.0f));
			glm::mat4 translate = glm::translate(entities[i][j].model, glm::vec3((cell_width * i) + (cell_width/2.0f), (cell_height * j) + (cell_height/2.0f), 0.0f));
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
			/*
		case sf::Event::MouseButtonPressed:
			interactWithCell(sf::Mouse::getPosition(window), event);
			break;
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Space)
				applyingRules();
				*/
		default:
			break;
		}
	}
}

void Game::update()
{

	projection = glm::ortho(0.0f, (float)win_width, 0.0f, (float)win_height, 0.1f, 100.0f);
}

void Game::render()
{
	Shader shader = ResourceManager::GetShader("basicShader");

	shader.Use();
	shader.SetMatrix4("projection", projection);

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
