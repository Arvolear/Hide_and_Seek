#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 view;
uniform mat4 projection;

out vec4 polyColor;

void main()
{
    polyColor = vec4(color, 1.0f);

    vec4 pos = projection * view * vec4(position, 1.0f);    

    gl_Position = pos.xyww;
}
