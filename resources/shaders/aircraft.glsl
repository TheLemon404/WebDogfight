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
out vec2 pUV;
out vec4 pWorldPos;
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
    pWorldPos = uViewTransform * uJointTransforms[int(aBoneID)] * vec4(aPos, 1.0f);
    gl_Position = uProjection * pWorldPos;

    mat3 rotationMatrix = extractRotation(uTransform) * extractRotation(uJointTransforms[int(aBoneID)]);
    pNormal = rotationMatrix * aNormal;

    pBoneID = aBoneID;
    pUV = aUV;
}

#fragment
#version 300 es
precision highp float;

in vec3 pNormal;
in vec2 pUV;
out vec4 pWorldPos;
flat in uint pBoneID;

uniform vec3 uSunDirection;
uniform float uAlpha;
uniform vec3 uAlbedo;
uniform vec3 uShadowColor;
uniform vec3 uCameraPosition;

uniform sampler2D uAlbedoTexture;
uniform sampler2D uRoughnessTexture;
uniform sampler2D uEmmissionTexture;

out vec4 FragColor;

void main()
{
    float diffuse = clamp(dot(pNormal, -uSunDirection), 0.0, 1.0);
    vec3 albedo = texture(uAlbedoTexture, pUV).rgb;
    float alpha = texture(uAlbedoTexture, pUV).a;

    vec3 viewDir = normalize(uCameraPosition - pWorldPos.xyz);
    vec3 reflectDir = normalize(reflect(uSunDirection, pNormal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    spec *= 1.0 - texture(uRoughnessTexture, pUV).r;

    vec4 nonEmmissive = vec4(mix(uShadowColor * albedo, albedo, diffuse + spec), alpha);
    FragColor = mix(nonEmmissive, vec4(albedo, alpha), texture(uEmmissionTexture, pUV).r);
}
