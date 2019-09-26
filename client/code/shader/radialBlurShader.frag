#version 330 core

in vec2 UV;

uniform sampler2D blurTexture;
uniform vec2 center;
uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;

#define NUM_SAMPLES 100

out vec4 fragColor;

void main()
{
    vec2 texCoord = UV;

    vec2 deltaTexCoord = UV - center;
    deltaTexCoord *= 1.0 / (NUM_SAMPLES * density);

    vec4 color = texture(blurTexture, texCoord);
    float illuminationDecay = 1.0;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        texCoord -= deltaTexCoord;
        
        vec4 sampleColor = texture(blurTexture, texCoord);
        sampleColor *= illuminationDecay * weight;

        color += sampleColor;
        illuminationDecay *= decay;
    }

    fragColor = color * exposure;
}
