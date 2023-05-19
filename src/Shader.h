#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glad/glad.h>

class Shader
{
public:
	unsigned int ID;
	
	Shader() {}

	Shader& Use();

	void Compile(const char* vertexSRC, const char* fragmentSRC);

	void SetFloat(const char* name, float value, bool useShader = false);
	void SetInteger(const char* name, int value, bool useShader = false);
	void SetVector2f(const char* name, glm::vec2& value, bool useShader = false);
	void SetVector3f(const char* name, glm::vec3& value, bool useShader = false);
	void SetVector4f(const char* name, glm::vec4& value, bool useShader = false);
	void SetMatrix4(const char* name, glm::mat4 matrix, bool useShader = false);

private:
	void checkCompileErrors(unsigned int object, std::string type);
};

