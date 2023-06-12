#include "Entity.h"

void Entity::init()
{
	GLfloat vertices[12] = {
		1.0f,  1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f,  1.0f, 0.0f
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

void Entity::draw(glm::vec3 translate, glm::vec3 color, Shader shader)
{

	if (translate.x < 0)
		translate.x -= 1;

	if (translate.y < 0)
		translate.y -= 1;

	std::modf(translate.x, &translate.x);
	std::modf(translate.y, &translate.y);

	shader.Use();
	glm::mat4 model = glm::translate(glm::mat4(1.0f), translate);
	shader.SetMatrix4("model", model, true);
	shader.SetVector3f("entityColor", color, true);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
