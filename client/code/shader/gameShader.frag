#version 330 core

out vec4 fragColor;

in vec2 UV;

uniform sampler2D scene;
uniform sampler2D blurBloom;
uniform sampler2D lensFlare;

uniform float exposure;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);

    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    float gamma = 2.2;

    vec3 hdrColor = texture(scene, UV).rgb;
    vec3 bloomColor = texture(blurBloom, UV).rgb;
    vec3 flareColor = texture(lensFlare, UV).rgb;

    /*
    hdrColor = rgb2hsv(hdrColor); 

    hdrColor.g *= 1.3;
    hdrColor.g = clamp(hdrColor.g, 0.0, 1.0);

    hdrColor = hsv2rgb(hdrColor);
    */

    hdrColor += bloomColor + flareColor;

    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    result = pow(result, vec3(1.0 / gamma));

    fragColor = vec4(result, 0.0);
    //fragColor = texture(scene, UV);
}
