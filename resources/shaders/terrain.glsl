#vertex
#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uViewTransform;
uniform mat4 uTransform;
uniform mat4 uProjection;

uniform sampler2D uHeightmap;

out vec2 pUV;
out vec3 pPos;

#define HEIGHT_CONSTANT 10000.0

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
    float height = textureLod(uHeightmap, aUV, 0.0).r * HEIGHT_CONSTANT;
    vec3 worldPosition = aPos + vec3(0.0, height, 0.0);
    vec4 viewPosition = uViewTransform * vec4(worldPosition, 1.0);
    gl_Position = uProjection * viewPosition;

    pUV = aUV;

    pPos = worldPosition;
}

#fragment
#version 300 es
precision highp float;

in vec2 pUV;
in vec3 pPos;

uniform vec3 uSunDirection;
uniform vec3 uSunColor;
uniform vec3 uAlbedo;
uniform vec3 uShadowColor;
uniform int uResolution;

uniform vec3 cameraPosition;

out vec4 FragColor;

void main()
{
    int iX = int(pUV.x * float(uResolution));
    int iY = int(pUV.y * float(uResolution));
    int checkerX = iX % 2;
    int checkerY = iY % 2;

    float val = float(bool(checkerX) ^^ bool(checkerY));

    vec3 normal = normalize(cross(dFdx(pPos), dFdy(pPos)));
    float dot = clamp(dot(normal, -uSunDirection), 0.0, 1.0);
    FragColor = vec4(mix(uShadowColor, uAlbedo, dot), 1.0f);
}
