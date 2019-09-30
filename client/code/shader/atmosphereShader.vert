#version 330 core

layout (location = 0) in vec3 position;

out vec3 vPos;

void main()
{
    gl_Position = vec4(position, 1.0);

    vPos = position;
}
