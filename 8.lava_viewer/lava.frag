#version 330 core

in vec4 positionS;
out vec4 color;

void main()
{
	color = vec4(1.0f, 1.0f*100.0f/positionS.w, 0.0f, 1.0f);
}
