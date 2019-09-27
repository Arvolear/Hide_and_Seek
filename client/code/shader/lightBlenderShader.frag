#version 330 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in vec2 UV;

uniform sampler2D scatterTexture;

void main()
{
    fragColor = texture(scatterTexture, UV);
    brightColor = texture(scatterTexture, UV);
}
