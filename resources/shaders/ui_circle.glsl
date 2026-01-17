
#vertex
#version 300 es
precision mediump float;
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
precision mediump float;

in vec2 pUV;

uniform vec4 uColor;
uniform int uRadius;
uniform int uThickness;
uniform ivec2 uWidgetResolution;

out vec4 FragColor;

#define UI_CENTER vec2(0.5)

void main()
{
    vec2 currentWidgetPixel = vec2(pUV.x * float(uWidgetResolution.x), pUV.y * float(uWidgetResolution.y));

    float dist = distance(vec2(UI_CENTER.x * float(uWidgetResolution.x), UI_CENTER.y * float(uWidgetResolution.y)), currentWidgetPixel);
    if (dist > float(uRadius)) discard;
    if (dist < float(uRadius - uThickness)) discard;
    FragColor = uColor;
}
