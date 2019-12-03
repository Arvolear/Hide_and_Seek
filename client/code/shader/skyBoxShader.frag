#version 330 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in vec3 textureCoords;

uniform float alpha;
uniform sampler2D depthTexture;
uniform samplerCube skybox;

void main()
{
    float gamma = 2.2;

    vec2 UV = gl_FragCoord.xy / textureSize(depthTexture, 0);

    if (texture(depthTexture, UV).x != 0.0)
    {
        discard;
    }

    fragColor = vec4(pow(texture(skybox, textureCoords).rgb, vec3(gamma)), alpha);    

    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0)
    {
        brightColor = vec4(fragColor.rgb, alpha);
    }
    else
    {
        brightColor = vec4(0.0, 0.0, 0.0, alpha);
    }
}
