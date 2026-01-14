#vertex
#version 300 es

#define MAX_BONES 15

precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in uint aBoneID;

uniform mat4 uTransform;
uniform mat4 uViewTransform;
uniform mat4 uProjection;
uniform mat4 uJointTransforms[MAX_BONES];

out vec3 pNormal;
flat out uint pBoneID;

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
    vec4 worldPosition = uViewTransform * uJointTransforms[int(aBoneID)] * vec4(aPos, 1.0f);
    gl_Position = uProjection * worldPosition;

    mat3 rotationMatrix = extractRotation(uViewTransform) * extractRotation(uJointTransforms[int(aBoneID)]);
    pNormal = rotationMatrix * aNormal;

    pBoneID = aBoneID;
}

#fragment
#version 300 es
precision highp float;

in vec3 pNormal;
flat in uint pBoneID;

uniform vec3 uSunDirection;
uniform vec3 uSunColor;
uniform vec3 uAlbedo;
uniform vec3 uShadowColor;

out vec4 FragColor;

void main()
{
    float dot = clamp(dot(pNormal, -uSunDirection), 0.0, 1.0);
    FragColor = vec4(mix(uShadowColor, uAlbedo, dot), 1.0f);
}
