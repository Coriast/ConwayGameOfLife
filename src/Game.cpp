#include "Game.h"

GLfloat vertices[] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
};

GLuint indices[] = {
	0, 1, 3,
	1, 2, 3
};

Game::Game()
{
	for (auto& v : grid)
		for (auto& e : v)
			e = 0;
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
	unsigned int VBO; // Vertex Buffer Object 
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

	shader.Use();

	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	window.display();
}
