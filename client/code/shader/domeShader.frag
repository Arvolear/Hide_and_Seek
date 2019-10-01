#version 330 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in vec2 UV;

uniform sampler2D dome;

void main()
{
    fragColor = texture(dome, UV);
    brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
