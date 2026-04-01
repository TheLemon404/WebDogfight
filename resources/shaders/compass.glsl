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

#define ORIENTATION_NOTCH_WIDTH_PIXELS 8
#define PI 3.14159265358979323846

precision mediump float;

in vec2 pPos;
in vec2 pUV;

uniform vec4 uColor;
uniform vec4 uBorderColor;
uniform vec4 uCornerColor;
uniform int uBorder;
uniform int uCornerBorder;
uniform int uCornerLength;
uniform float uCameraRotation;

uniform ivec2 uWidgetResolution;

out vec4 FragColor;

vec2 uvToPixel(vec2 uv) {
    return uv * vec2(uWidgetResolution);
}

vec2 pixelToUV(vec2 pixel) {
    return pixel / vec2(uWidgetResolution);
}

void main()
{
    FragColor = uColor;

    ivec2 currentWidgetPixel = ivec2(uvToPixel(pUV));

    float uvSpaceCameraRotation = (uCameraRotation + PI) / (2.0 * PI);
    float uvPixelWidth = float(ORIENTATION_NOTCH_WIDTH_PIXELS) / float(uWidgetResolution.x);
    float repeatedMainUV = mod(pUV.x * 4.0, 1.0);
    float repeatedSecondaryUV = mod((pUV.x * 4.0) + 0.5, 1.0);
    if (repeatedMainUV > uvSpaceCameraRotation - uvPixelWidth && repeatedMainUV < uvSpaceCameraRotation + uvPixelWidth) {
        FragColor = uCornerColor;
    }
    else if (repeatedSecondaryUV > uvSpaceCameraRotation - (uvPixelWidth / 2.0) && repeatedSecondaryUV < uvSpaceCameraRotation + (uvPixelWidth / 2.0)) {
        FragColor = uCornerColor / 8.0;
    }

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

    float uvXFalloff = pow(abs(pUV.x - 0.5) * 2.0, 4.0);
    FragColor -= vec4(0.0, 0.0, 0.0, uvXFalloff);
}
