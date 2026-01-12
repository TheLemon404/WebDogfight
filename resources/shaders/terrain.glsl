#vertex
#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uTransform;
uniform mat4 uView;
uniform mat4 uProjection;

out vec2 pUV;
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
    vec4 worldPosition = uTransform * vec4(aPos, 1.0f);
    gl_Position = uProjection * uView * worldPosition;

    mat3 rotationMatrix = extractRotation(uTransform);
    pNormal = rotationMatrix * aNormal;

    pUV = aUV;
}

#fragment
#version 300 es
precision highp float;

in vec2 pUV;
in vec3 pNormal;

uniform vec3 uSunDirection;
uniform vec3 uSunColor;
uniform vec3 uAlbedo;
uniform vec3 uShadowColor;
uniform int uResolution;

out vec4 FragColor;

void main()
{
    int iX = int(pUV.x * float(uResolution));
    int iY = int(pUV.y * float(uResolution));
    int checkerX = iX % 2;
    int checkerY = iY % 2;

    float val = float(bool(checkerX) ^^ bool(checkerY));

    float dot = clamp(dot(pNormal, -uSunDirection), 0.0, 1.0);
    FragColor = vec4(mix(uShadowColor, vec3(val), dot), 1.0f);
}
