#version 330 core

in vec2 UV;

uniform sampler2D blurTexture;
uniform bool isHorizontal;
uniform float intensity;
uniform float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

out vec4 fragColor;

void main()
{
    vec2 offset = intensity / textureSize(blurTexture, 0);
    vec3 result = texture(blurTexture, UV).rgb * weight[0];

    if (isHorizontal)
    {
        for (int i = 1; i < 5; i++)
        {
            result += texture(blurTexture, UV + vec2(offset.x * i, 0.0)).rgb * weight[i];
            result += texture(blurTexture, UV - vec2(offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; i++)
        {
            result += texture(blurTexture, UV + vec2(0.0, offset.y * i)).rgb * weight[i];
            result += texture(blurTexture, UV - vec2(0.0, offset.y * i)).rgb * weight[i];
        } 
    }

    fragColor = vec4(result, 0.0);
}
