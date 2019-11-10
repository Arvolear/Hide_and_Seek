#version 330 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in vec2 UV;

uniform sampler2D depthTexture;
uniform sampler2D dome;

void main()
{
    if (texture(depthTexture, UV).x != 0.0)
    {
        discard;
    }

    fragColor = texture(dome, UV);
    brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
