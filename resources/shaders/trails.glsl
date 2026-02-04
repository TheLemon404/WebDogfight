
#vertex
#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uView;
uniform mat4 uProjection;

out vec3 pNormal;
out vec2 pUV;

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
    vec4 worldPosition = uView * vec4(aPos, 1.0f);
    gl_Position = uProjection * worldPosition;

    pNormal = aNormal;

    pUV = aUV;
}

#fragment
#version 300 es
precision highp float;

in vec3 pNormal;
in vec2 pUV;

uniform vec3 uSunDirection;
uniform float uAlpha;
uniform vec3 uAlbedo;
uniform vec3 uShadowColor;

out vec4 FragColor;

void main()
{
    float dot = clamp(dot(pNormal, -uSunDirection), 0.0, 1.0);
    float alphaUVFactor = (1.0f - (abs(pUV.y - 0.4f) * 2.0f)) * (1.0f - (abs(pUV.x - 0.5f) * 2.0f));
    FragColor = vec4(mix(uShadowColor, uAlbedo, dot), uAlpha * alphaUVFactor);
}
