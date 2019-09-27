#version 330 core

layout (location = 0) out vec4 fragColor;

in vec4 polyColor;

void main()
{
    fragColor = polyColor;    
}
