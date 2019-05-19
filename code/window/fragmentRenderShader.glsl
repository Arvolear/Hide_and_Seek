#version 330 core

in vec2 UV;

uniform sampler2D finalTexture;

out vec4 color;

void main()
{
    color = texture(finalTexture, UV);
}
