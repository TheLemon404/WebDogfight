
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
uniform mat4 uTransform;

out vec4 FragColor;

#define ABSORBTION 1.0f

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

float rayAABBEnter(const vec3 b_min, const vec3 b_max, vec3 rayOrigin, vec3 rayDirection)
{
    float tmin = 0.0, tmax = 1e30f;

    for (int axis = 0; axis < 3; axis++)
    {
        if (abs(rayDirection[axis]) < 1e-8f) {
            if (rayOrigin[axis] < b_min[axis] || rayOrigin[axis] > b_max[axis]) {
                return 1e30f;
            }
            continue;
        }

        float t1 = (b_min[axis] - rayOrigin[axis]) / rayDirection[axis];
        float t2 = (b_max[axis] - rayOrigin[axis]) / rayDirection[axis];

        float dmin = min(t1, t2);
        float dmax = max(t1, t2);

        tmin = max(dmin, tmin);
        tmax = min(dmax, tmax);
    }

    if (tmax >= tmin && tmin >= 0.0) return tmin;

    return 1e30f;
}

float rayAABBExit(const vec3 b_min, const vec3 b_max, vec3 rayOrigin, vec3 rayDirection)
{
    float tmin = 0.0, tmax = 1e30f;

    for (int axis = 0; axis < 3; axis++)
    {
        if (abs(rayDirection[axis]) < 1e-8f) {
            if (rayOrigin[axis] < b_min[axis] || rayOrigin[axis] > b_max[axis]) {
                return 1e30f;
            }
            continue;
        }

        float t1 = (b_min[axis] - rayOrigin[axis]) / rayDirection[axis];
        float t2 = (b_max[axis] - rayOrigin[axis]) / rayDirection[axis];

        float dmin = min(t1, t2);
        float dmax = max(t1, t2);

        tmin = max(dmin, tmin);
        tmax = min(dmax, tmax);
    }

    if (tmax >= tmin && tmin >= 0.0) return tmax;

    return 1e30f;
}

vec4 traverseVolume(vec3 rayOrigin, vec3 rayDirection) {
    mat4 invTransform = inverse(uTransform);
    vec3 boxSpaceRayOrigin = (invTransform * vec4(rayOrigin, 1.0)).xyz;
    vec3 boxSpaceRayDirection = normalize((invTransform * vec4(rayDirection, 0.0)).xyz);

    vec3 cubeMin = vec3(-1.0);
    vec3 cubeMax = vec3(1.0);

    float entryT = rayAABBEnter(cubeMin, cubeMax, boxSpaceRayOrigin, boxSpaceRayDirection);
    float exitT = rayAABBExit(cubeMin, cubeMax, boxSpaceRayOrigin, boxSpaceRayDirection);

    vec3 boxSpaceEnterPosition = boxSpaceRayOrigin + boxSpaceRayDirection * entryT;
    vec3 boxSpaceExitPosition = boxSpaceRayOrigin + boxSpaceRayDirection * exitT;

    if (entryT == 1e30f) {
        return vec4(rayDirection, 1.0);
    }

    float dist = distance(boxSpaceEnterPosition, boxSpaceExitPosition);

    return vec4(1.0 - exp(-dist * ABSORBTION));
}

void main()
{
    float dot = clamp(dot(pNormal, -uSunDirection), 0.0, 1.0);
    FragColor = traverseVolume(extractPosition(uView), getRayWorldDirection());
}
