
#vertex
#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uTransform;
uniform mat4 uProjection;

out vec2 pUV;

void main()
{
    vec4 screenPosition = uTransform * vec4(aPos, 1.0f);
    gl_Position = uProjection * screenPosition;
    pUV = aUV;
}

#fragment
#version 300 es
precision highp float;

in vec2 pUV;

uniform vec4 uColor;
uniform float uRadius;
uniform float uThickness;

out vec4 FragColor;

#define UI_CENTER vec2(0.5)

void main()
{
    float dist = distance(UI_CENTER, pUV);
    if (dist > uRadius) discard;
    if (dist < uRadius - uThickness) discard;
    FragColor = uColor;
}
