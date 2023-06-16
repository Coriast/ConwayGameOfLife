#include "ResourceManager.h"

std::map<SHADER, Shader>	ResourceManager::shaders;

const char *ResourceManager::basicF =
		"#version 330 core \n\
		out vec4 fragColor; \n\
		uniform vec3 entityColor; \n\
		void main() \n\
		{ \n\
			fragColor = vec4(entityColor, 1.0f); \n\
		} \n";

const char *ResourceManager::basicV =
		"#version 330 core \n\
		layout(location = 0) in vec3 aPos; \n\
		uniform mat4 model; \n\
		uniform mat4 view; \n\
		uniform mat4 projection; \n\
		void main() \n\
		{ \n\
			gl_Position = projection * view * model * vec4(aPos, 1.0); \n\
		}\n";

Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, SHADER name)
{
	shaders[name] = LoadShaderFromFile(vShaderFile, fShaderFile);
	return shaders[name];
}

Shader ResourceManager::LoadShader(SHADER name)
{
	shaders[name] = LoadShaderFromFile();
	return shaders[name];
}

Shader ResourceManager::GetShader(SHADER name)
{
	return shaders[name];
}

Shader ResourceManager::LoadShaderFromFile(const char* vShaderFile, const char* fShaderFile)
{
	std::string vertexCode, fragmentCode;

	try
	{
		std::ifstream		vertexShaderF(vShaderFile);
		std::ifstream		fragmentShaderF(fShaderFile);
		std::stringstream	vShaderStream, fShaderStream;

		vShaderStream << vertexShaderF.rdbuf();
		fShaderStream << fragmentShaderF.rdbuf();

		vertexShaderF.close(); fragmentShaderF.close();

		vertexCode		= vShaderStream.str();
		fragmentCode	= fShaderStream.str();
	}
	catch (std::exception ex)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files \n" 
			<< "Vertex Path: " << vShaderFile << "\n" 
			<< "Fragment Path: " << fShaderFile << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	Shader shader;
	shader.Compile(vShaderCode, fShaderCode);
	return shader;
}

Shader ResourceManager::LoadShaderFromFile()
{
	Shader shader;
	shader.Compile(basicV, basicF);
	return shader;
}
