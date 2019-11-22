#version 330 core

layout (location = 0) in vec3 position;

out vec2 UV;

void main()
{
    gl_Position = vec4(position, 1.0);

    UV = vec2(1.0) - vec2((position.x + 1.0) / 2.0, (position.y + 1.0) / 2.0);
}
