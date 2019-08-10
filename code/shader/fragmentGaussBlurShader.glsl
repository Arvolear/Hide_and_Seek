#version 330 core

in vec2 UV;

uniform sampler2D blurTexture;
uniform bool isHorizontal;
uniform float intensity;

uniform float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

out vec4 fragColor;

void main()
{
    vec2 size = textureSize(blurTexture, 0) / (intensity / 2.0);
    vec3 result = texture(blurTexture, UV).rgb * weight[0];

    if (isHorizontal)
    {
        for (int i = 1; i < 3; i++)
        {
            result += texture(blurTexture, UV + vec2(offset[i], 0.0) / size).rgb * weight[i];
            result += texture(blurTexture, UV - vec2(offset[i], 0.0) / size).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 3; i++)
        {
            result += texture(blurTexture, UV + vec2(0.0, offset[i]) / size).rgb * weight[i];
            result += texture(blurTexture, UV - vec2(0.0, offset[i]) / size).rgb * weight[i];
        } 
    }

    fragColor = vec4(result, 0.0);
}
