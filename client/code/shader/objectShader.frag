#version 330 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

struct GBuffer
{
    sampler2D texture_position;
    sampler2D texture_normal;
    sampler2D texture_albedo;
    sampler2D texture_metRoughAO;
};

struct DirLight
{
    vec3 direction;
    vec3 color;

    int isShadow; 
    sampler2D texture_shadow1; // depth
    
    float esmFactor;
    float bias;

    mat4 shadowView;
    mat4 shadowProjection;
};

uniform sampler2D texture_ssao;

in vec2 UV;

uniform GBuffer gBuffer;

#define MAX_DIR_LIGHTS 1
uniform DirLight dirLights[MAX_DIR_LIGHTS];

uniform vec3 viewPos;

const float PI = 3.1415926535;

float calcDirShadow(DirLight light, vec4 shadowCoords)
{
    vec4 projCoords = shadowCoords / shadowCoords.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    currentDepth += light.bias;

    float moment = texture(light.texture_shadow1, projCoords.xy).r;

    if (currentDepth < moment)
    {
        return 1.0;
    }

    if (currentDepth > 1.0)
    {
        return 1.0; 
    }

    float occluder = exp(light.esmFactor * moment);
    float receiver = exp(-light.esmFactor * currentDepth);
    float shadow = smoothstep(0.2, 1.0, occluder * receiver);

    return shadow;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float nDotH = max(dot(N, H), 0.0);
    float nDotH2 = nDotH * nDotH;

    float num = a2;
    float denom = (nDotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float geometrySchlickGGX(float nDotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float num = nDotV;
    float denom = nDotV * (1.0 - k) + k;

    return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float nDotV = max(dot(N, V), 0.0);
    float nDotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(nDotV, roughness);
    float ggx1 = geometrySchlickGGX(nDotL, roughness);

    return ggx1 * ggx2;
}

vec4 calcDirLights()
{
    float gamma = 2.2;

    vec3 fragPos = texture(gBuffer.texture_position, UV).rgb;
    vec3 fragNorm = texture(gBuffer.texture_normal, UV).rgb;
    vec3 fragAlbedo = pow(texture(gBuffer.texture_albedo, UV).rgb, vec3(gamma));
    float fragMetal = texture(gBuffer.texture_metRoughAO, UV).r;
    float fragRough = texture(gBuffer.texture_metRoughAO, UV).g;
    float fragAO = texture(gBuffer.texture_metRoughAO, UV).b;

    //fragMetal = 0.5;
    //fragRough = 0.5;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, fragAlbedo, fragMetal);

    vec3 L0 = vec3(0.0);

    for (int i = 0; i < MAX_DIR_LIGHTS; i++)
    {
        vec3 lightDir = normalize(-dirLights[i].direction);

        /* PBR */
        vec3 halfDir = normalize(viewDir + lightDir);
        vec3 radiance = dirLights[i].color;

        // cook-torrance brdf
        float NDF = distributionGGX(fragNorm, halfDir, fragRough);
        float geom = geometrySmith(fragNorm, viewDir, lightDir, fragRough);
        vec3 fresnel = fresnelSchlick(max(dot(halfDir, viewDir), 0.0), F0);

        vec3 kS = fresnel;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - fragMetal;

        vec3 numerator = NDF * geom * fresnel;
        float denominator = 4.0 * max(dot(fragNorm, viewDir), 0.0) * max(dot(fragNorm, lightDir), 0.0);
        vec3 specular = numerator / max(denominator, 0.001);

        float nDotL = max(dot(fragNorm, lightDir), 0.0);

        vec3 L00 = (kD * fragAlbedo / PI + specular) * radiance * nDotL;

        if (dirLights[i].isShadow == 1)
        {
            // calc shadow coords
            vec4 dirShadowCoords = dirLights[i].shadowProjection * dirLights[i].shadowView * vec4(fragPos, 1.0);

            float shadow = calcDirShadow(dirLights[i], dirShadowCoords);
            L00 *= shadow;
        }

        L0 += L00;
    }

    float ssao = texture(texture_ssao, UV).r;

    vec3 ambient = vec3(0.03) * fragAlbedo * ssao; // * fragAO;
    vec3 res = ambient + L0;

    return vec4(res, 1.0);
}

void main()
{
    vec4 result = vec4(0.0f);

    /* dir light */
    result = calcDirLights();

    /* bloom */
    fragColor = result;

    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0)
    {
        brightColor = vec4(fragColor.rgb, 1.0);
    }
    else
    {
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
