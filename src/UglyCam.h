#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class UglyCam
{
public:
	glm::vec3 cameraPos		= glm::vec3(0.0f, 0.0f, 10.0f);
	glm::vec3 cameraUp		= glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraFront	= glm::vec3(0.0f, 0.0f, -1.0f);

	glm::mat4 getView();
};

