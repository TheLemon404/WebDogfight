
#vertex
#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uTransform;
uniform mat4 uViewTransform;
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
    vec4 worldPosition = uViewTransform * vec4(aPos, 1.0f);
    gl_Position = uProjection * worldPosition;

    mat3 rotationMatrix = extractRotation(uTransform);
    pNormal = rotationMatrix * aNormal;
}

#fragment
#version 300 es
precision highp float;

in vec3 pNormal;

uniform vec3 uSunDirection;
uniform float uAlpha;
uniform vec3 uAlbedo;
uniform vec3 uShadowColor;
uniform vec2 uScreenResolution;
uniform mat4 uProjection;
uniform mat4 uView;

out vec4 FragColor;

vec3 extractPosition(mat4 viewMatrix) {
    return inverse(viewMatrix)[3].xyz;
}

vec3 getRayWorldDirection() {
    vec2 screenUV = gl_FragCoord.xy / uScreenResolution;
    vec4 clip = vec4(screenUV * 2.0 - 1.0, 1.0, 1.0);
    vec4 viewRay = inverse(uProjection) * clip;
    viewRay /= viewRay.w;
    vec4 worldDirection = inverse(uView) * vec4(viewRay.xyz, 0.0);
    return normalize(worldDirection.xyz);
}

vec4 traverseVolume(vec3 rayOrigin, vec3 rayDirection) {
    return vec4(rayDirection, 1.0);
}

void main()
{
    float dot = clamp(dot(pNormal, -uSunDirection), 0.0, 1.0);
    FragColor = traverseVolume(extractPosition(uView), getRayWorldDirection());
}
