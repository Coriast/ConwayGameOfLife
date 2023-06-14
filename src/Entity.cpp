#include "Entity.h"

Entity::Entity() {}

Entity::Entity(glm::vec3 coordenates, glm::vec3 color)
{
	setCoord(coordenates);
	setColor(color);
}

void Entity::init()
{
	/*
	* Atualmente meu VAO é por objeto de Entity só que 
	* como são os mesmos dados eu posso de alguma forma
	* só utilizar o mesmo VAO para todas as chamadas de Draw que eu faço
	* Depois eu altero isso
	*/
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

void Entity::draw(Shader shader)
{
	shader.Use();
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(coord.x, coord.y, 0.0f));
	shader.SetMatrix4("model", model, true);
	shader.SetVector3f("entityColor", color, true);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Entity::setCoord(glm::vec3 coordenates)
{
	coord.x = std::round(coordenates.x);
	coord.y = std::round(coordenates.y);
}

void Entity::setColor(glm::vec3 color)
{
	this->color = color;
}
