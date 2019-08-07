#version 330 core

out vec4 fragColor;

in vec2 UV;

uniform sampler2D scene;
uniform sampler2D blurBloom;

uniform float exposure;

void main()
{
    float gamma = 1.0;

    vec3 hdrColor = texture(scene, UV).rgb;
    vec3 bloomColor = texture(blurBloom, UV).rgb;

    hdrColor += bloomColor;

    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    result = pow(result, vec3(1.0 / gamma));

    fragColor = vec4(result, 0.0);
    //fragColor = texture(scene, UV);
}
