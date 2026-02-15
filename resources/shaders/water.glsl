#vertex
#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uView;
uniform mat4 uProjection;

out vec3 pNormal;

void main()
{
    vec4 worldPosition = uView * vec4(aPos, 1.0f);
    gl_Position = uProjection * worldPosition;
    pNormal = aNormal;
}

#fragment
#version 300 es
precision highp float;

in vec3 pNormal;

uniform vec3 uSunDirection;
uniform float uAlpha;
uniform vec3 uAlbedo;
uniform vec3 uShadowColor;
uniform vec3 uFogColor;

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
    float distanceLerp = clamp(linearizeDepth(gl_FragCoord.z) / FALLOFF, 0.0, 1.0);
    vec3 color = mix(uAlbedo, uFogColor, distanceLerp);
    FragColor = vec4(mix(color, uShadowColor, lightingDot), uAlpha);
}
