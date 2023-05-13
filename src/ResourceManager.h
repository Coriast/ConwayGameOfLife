#pragma once
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include "Shader.h"

class ResourceManager
{
public:
	static std::map<std::string, Shader> shaders;

	static Shader	LoadShader(const char* vShaderFile, const char* fShaderFile, std::string name);

	static Shader	GetShader(std::string name);

private:
	ResourceManager() {}

	static Shader	LoadShaderFromFile(const char* vShaderFile, const char* fShaderFile);
};

