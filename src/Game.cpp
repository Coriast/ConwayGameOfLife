#include "Game.h"

Game::Game()
{
	for (size_t i = 0; i < grid_size; i++)
	{
		for (size_t j = 0; j < grid_size; j++)
		{
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

	window.create(sf::VideoMode(800, 600), "Conway", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);
	window.setActive(true);

	gladLoadGLLoader((GLADloadproc)sf::Context::getFunction);

	glViewport(0, 0, 800, 600);

	ResourceManager::LoadShader("data\\shaders\\basic_V.glsl", "data\\shaders\\basic_F.glsl", "basicShader");
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
			glViewport(0, 0, event.size.width, event.size.height);
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

}

void Game::render()
{
	Shader shader = ResourceManager::GetShader("basicShader");

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// criando um Buffer de memória
	GLuint VBO[grid_size*grid_size]; // Vertex Buffer Object 
	glGenBuffers((grid_size*grid_size), &(*VBO));

	GLuint EBO[grid_size*grid_size];
	glGenBuffers((grid_size * grid_size), &(*EBO));

	glBindVertexArray(VAO);

	int k = 0;
	for (size_t i = 0; i < grid_size; i++)
	{
		for (size_t j = 0; j < grid_size; j++)
		{
			// Linkando este ID para um Buffer do tipo GL_ARRAY_BUFFER
			glBindBuffer(GL_ARRAY_BUFFER, VBO[k]);
			// Copiando os dados do nosso Array para o Buffer
			glBufferData(GL_ARRAY_BUFFER, sizeof(entities[i][j].vertices), entities[i][j].vertices, GL_STATIC_DRAW);

			// Bind Indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[k]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(entities[i][j].indices), entities[i][j].indices, GL_STATIC_DRAW);

			// Configurando como vai ser a leitura dos dados salvos no buffer
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			// Enviando o index referente ao { layout (location = 0) }
			glEnableVertexAttribArray(0);

			glm::vec3 entity_color(entities[i][j].alive);
			shader.SetVector3f("entityColor", entity_color, true);
			
			k++;
		}
	}

	shader.Use();

	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	window.display();
}
