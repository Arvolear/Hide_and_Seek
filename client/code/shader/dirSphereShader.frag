#version 330 core

layout (location = 0) out vec4 fragColor;

in vec4 polyColor;

uniform sampler2D depthTexture;

void main()
{
    vec2 UV = gl_FragCoord.xy / textureSize(depthTexture, 0);

    if (texture(depthTexture, UV).x != 0.0)
    {
        discard;
    }

    fragColor = polyColor;    
}
