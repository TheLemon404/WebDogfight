#vertex
#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uView;
uniform mat4 uProjection;

out vec2 pUV;
out vec3 pNormal;
out vec3 pPos;

void main()
{
    vec4 viewPosition = uView * vec4(aPos, 1.0);
    gl_Position = uProjection * viewPosition;

    pPos = aPos;
    pNormal = aNormal;
    pUV = aUV;
}

#fragment
#version 300 es
precision highp float;

in vec2 pUV;
in vec3 pNormal;
in vec3 pPos;

uniform vec3 uSunDirection;
uniform float uAlpha;
uniform vec3 uAlbedo;
uniform vec3 uShadowColor;

uniform vec3 uTopColor;
uniform vec3 uMiddleColor;
uniform vec3 uSlopeColor;
uniform vec3 uBaseColor;

uniform vec3 uFogColor;

uniform sampler2D uDiscolorationMap;
uniform sampler2D uHeightmap;

out vec4 FragColor;

#define NEAR 10.0
#define FAR 100000.0
#define FALLOFF 60000.0

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
    float lightingDot = clamp(dot(pNormal, -uSunDirection), 0.0, 1.0);

    vec3 baseAlbedo = mix(uBaseColor, uMiddleColor, pow(texture(uHeightmap, pUV).r, 0.3));
    vec3 heightAlbedo = mix(baseAlbedo, uTopColor, pow(texture(uHeightmap, pUV).r, 4.0));
    vec3 finalAlbedo = mix(heightAlbedo, uSlopeColor, max(1.0 - abs(dot(pNormal, vec3(0.0, 1.0, 0.0))), 0.0));
    vec3 color = mix(uShadowColor, finalAlbedo, lightingDot);

    float distanceLerp = clamp(linearizeDepth(gl_FragCoord.z) / FALLOFF, 0.0, 1.0);

    FragColor = vec4(mix(color, uFogColor, distanceLerp), uAlpha);
}
