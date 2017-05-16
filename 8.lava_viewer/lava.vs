#version 330 core
layout (location = 3) in vec4 position;

out vec4 positionS;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	positionS = position;
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
}

