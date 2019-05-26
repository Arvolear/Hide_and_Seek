#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;

out vec3 textureCoords;

void main()
{
    textureCoords = position;

    vec4 pos = projection * view * vec4(position, 1.0);

    gl_Position = pos.xyww;
}
