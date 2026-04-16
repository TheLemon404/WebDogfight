
#vertex
#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 4) in mat4 aTransform;

#define MAX_PARTICLES 25

uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uAlbedos[MAX_PARTICLES];

out vec3 pNormal;
out vec3 pAlbedo;

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
    vec4 worldPosition = uView * aTransform * vec4(aPos, 1.0f);
    gl_Position = uProjection * worldPosition;

    mat3 rotationMatrix = extractRotation(aTransform);
    pNormal = rotationMatrix * aNormal;
    pAlbedo = uAlbedos[gl_InstanceID];
}

#fragment
#version 300 es
precision highp float;

in vec3 pNormal;
in vec3 pAlbedo;

uniform vec3 uSunDirection;
uniform float uAlpha;
uniform vec3 uAlbedo;

out vec4 FragColor;

void main()
{
    float dot = clamp(dot(pNormal, -uSunDirection), 0.0, 1.0);
    FragColor = vec4(pAlbedo, uAlpha);
}
