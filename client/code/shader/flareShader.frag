#version 330 core

in vec2 UV;

uniform sampler2D baseTexture;
uniform sampler2D flareGradientTexture;
uniform sampler2D lensDirtTexture;

float colorOffset = 0.05;

int ghostCount = 3;
float ghostSpacing = 0.3;
float ghostThreshold = 0.92;

float haloRadius = 0.7;
float haloThickness = 0.04;
float haloThreshold = 0.92;
float haloAspectRatio = 0.6;

float chromaticAberration = 0.02;

out vec4 fragColor;

vec3 ApplyThreshold(vec3 rgb, float threshold)
{
	return max(rgb - vec3(threshold), vec3(0.0));
}

vec3 SampleSceneColor(vec2 _uv)
{
	vec2 offset = normalize(vec2(0.5) - _uv) * chromaticAberration;

	return vec3(
		texture(baseTexture, _uv + offset).r,
		texture(baseTexture, _uv).g,
		texture(baseTexture, _uv - offset).b
		);
}

vec3 SampleGhosts(vec2 _uv, float _threshold)
{
    vec3 ret = vec3(0.0);
    vec2 ghostVec = (vec2(0.5) - _uv) * ghostSpacing;
    
    for (int i = 0; i < ghostCount; ++i) 
    {
        // sample scene color
        vec2 suv = fract(_uv + ghostVec * vec2(i));
        vec3 s = SampleSceneColor(suv);
        s = ApplyThreshold(s, _threshold);

        // tint/weight
        float distanceToCenter = distance(suv, vec2(0.5)) + colorOffset;
        s *= texture(flareGradientTexture, vec2(distanceToCenter, 0.5)).rgb;

        ret += s;
    }

    return ret;
}

vec3 SampleHalo(vec2 _uv, float _radius, float _aspectRatio, float _threshold)
{
    vec2 haloVec = vec2(0.5) - _uv;

    haloVec.x /= _aspectRatio;
    haloVec = normalize(haloVec);
    haloVec.x *= _aspectRatio;
    vec2 wuv = (_uv - vec2(0.5, 0.0)) / vec2(_aspectRatio, 1.0) + vec2(0.5, 0.0);
    float dist = distance(wuv, vec2(0.5));
    vec3 haloWeight = texture(flareGradientTexture, vec2(dist, 0.5)).rgb;

    haloVec *= _radius;

    return ApplyThreshold(SampleSceneColor(_uv + haloVec), _threshold) * haloWeight;
}

void main()
{
    vec3 ret = vec3(0.0);

    ret += SampleGhosts(UV, ghostThreshold);
    ret += SampleHalo(UV, haloRadius, haloAspectRatio, haloThreshold);

    fragColor = vec4(ret, 1.0) * texture(lensDirtTexture, UV);
}
