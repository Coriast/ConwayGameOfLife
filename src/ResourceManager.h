#pragma once
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include "Enum.h"
#include "Shader.h"

class ResourceManager
{
public:
	static const char* basicF;

	static const char* basicV;

	static std::map<SHADER, Shader> shaders;

	static Shader	LoadShader(const char* vShaderFile, const char* fShaderFile, SHADER name);

	static Shader	LoadShader(SHADER name);

	static Shader	GetShader(SHADER name);

private:
	ResourceManager() {}

	static Shader	LoadShaderFromFile(const char* vShaderFile, const char* fShaderFile);

	static Shader	LoadShaderFromFile();
};

