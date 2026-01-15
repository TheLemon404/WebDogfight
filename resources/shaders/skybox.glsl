
#vertex
#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uView;
uniform mat4 uProjection;

out vec3 pNormal;

mat3 extractRotation(mat4 transformation) {
    mat3 rotationScaleMatrix = mat3(
            transformation[0].xyz,
            transformation[1].xyz,
            transformation[2].xyz
        );

    float scaleX = length(rotationScaleMatrix[0]);
    if (scaleX > 0.0001) rotationScaleMatrix[0] /= scaleX;

    float scaleY = length(rotationScaleMatrix[1]);
    if (scaleY > 0.0001) rotationScaleMatrix[1] /= scaleY;

    float scaleZ = length(rotationScaleMatrix[2]);
    if (scaleZ > 0.0001) rotationScaleMatrix[2] /= scaleZ;

    return rotationScaleMatrix;
}

void main()
{
    gl_Position = uProjection * uView * vec4(aPos, 1.0f);

    pNormal = aNormal;
}

#fragment
#version 300 es
precision highp float;

in vec3 pNormal;

uniform vec3 uSkyColor;
uniform vec3 uHorizonColor;
uniform vec3 uSunColor;
uniform vec3 uSunDirection;
uniform float uSunRadius;

out vec4 FragColor;

void main()
{
    float lerpFactor = clamp(-pNormal.y, 0.0, 1.0);
    float sunDot = max(dot(normalize(uSunDirection), pNormal), 0.0);
    float sunLerp = pow(sunDot, 15.0) * 2.0;

    vec3 sky = mix(uHorizonColor, uSkyColor, pow(lerpFactor, 0.7));
    FragColor = vec4(mix(sky, uSunColor, sunLerp), 1.0f);
}
