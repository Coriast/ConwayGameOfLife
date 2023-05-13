#include "ResourceManager.h"

std::map<std::string, Shader>	ResourceManager::shaders;

Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, std::string name)
{
	shaders[name] = LoadShaderFromFile(vShaderFile, fShaderFile);
	return shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
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
