#version 330 core

out vec4 fragColor;

uniform vec3 entityColor;

void main()
{
    fragColor = vec4(entityColor, 1.0f);
}