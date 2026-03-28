
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

#define UV_CENTER vec2(0.5)

vec2 uvToPixel(vec2 uv) {
    return uv * vec2(uWidgetResolution);
}

vec2 pixelToUV(vec2 pixel) {
    return pixel / vec2(uWidgetResolution);
}

void main()
{
    vec2 currentWidgetPixel = uvToPixel(pUV);

    float dist = distance(vec2(UV_CENTER.x * float(uWidgetResolution.x), UV_CENTER.y * float(uWidgetResolution.y)), currentWidgetPixel);
    if (dist > float(uRadius)) discard;
    if (dist < float(uRadius - uThickness)) discard;
    FragColor = uColor;
}
