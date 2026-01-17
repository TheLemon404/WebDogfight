#vertex
#version 300 es
precision mediump float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uTransform;
uniform mat4 uProjection;

out vec2 pPos;
out vec2 pUV;

void main()
{
    vec4 screenPosition = uTransform * vec4(aPos, 1.0f);
    gl_Position = uProjection * screenPosition;

    pPos = aPos.xy;
    pUV = aUV;
}

#fragment
#version 300 es
precision mediump float;

in vec2 pPos;
in vec2 pUV;

uniform vec4 uColor;
uniform vec4 uBorderColor;
uniform vec4 uCornerColor;
uniform int uBorder;
uniform int uCornerBorder;
uniform int uCornerLength;

uniform ivec2 uWidgetResolution;

out vec4 FragColor;

void main()
{
    FragColor = uColor;

    ivec2 currentWidgetPixel = ivec2(int(pUV.x * float(uWidgetResolution.x)), int(pUV.y * float(uWidgetResolution.y)));

    //border
    if (currentWidgetPixel.x < uBorder || currentWidgetPixel.x >= uWidgetResolution.x - uBorder || currentWidgetPixel.y < uBorder || currentWidgetPixel.y >= uWidgetResolution.y - uBorder) {
        FragColor = uBorderColor;
    }

    //corner border
    if (currentWidgetPixel.x < uCornerLength && (currentWidgetPixel.y < uCornerLength || currentWidgetPixel.y > uWidgetResolution.y - uCornerLength) || currentWidgetPixel.x > uWidgetResolution.x - uCornerLength && (currentWidgetPixel.y < uCornerLength || currentWidgetPixel.y > uWidgetResolution.y - uCornerLength)) {
        if (currentWidgetPixel.x < uCornerBorder || currentWidgetPixel.x >= uWidgetResolution.x - uCornerBorder || currentWidgetPixel.y < uCornerBorder || currentWidgetPixel.y >= uWidgetResolution.y - uCornerBorder) {
            FragColor = uCornerColor;
        }
    }
}
