#vertex
#version 300 es
precision highp float;
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
precision highp float;

in vec2 pPos;
in vec2 pUV;

uniform vec4 uColor;
uniform vec4 uBorderColor;
uniform vec4 uCornerColor;
uniform int uBorder;
uniform int uCornerBorder;
uniform int uCornerLength;

out vec4 FragColor;

void main()
{
    float cornerFactor = 0.9;
    float cornerBorderWidth = 1.0 - (float(uCornerBorder) / 100.0);
    float borderWidth = 1.0 - (float(uBorder) / 100.0);

    FragColor = uColor;

    //border
    if (pPos.x > borderWidth || pPos.x < -borderWidth || pPos.y > borderWidth || pPos.y < -borderWidth) {
        FragColor = uBorderColor;
    }

    //corner border
    if (pPos.x > cornerFactor && (pPos.y > cornerFactor || pPos.y < -cornerFactor) || pPos.x < -cornerFactor && (pPos.y > cornerFactor || pPos.y < -cornerFactor)) {
        if (pPos.x > cornerBorderWidth || pPos.x < -cornerBorderWidth || pPos.y > cornerBorderWidth || pPos.y < -cornerBorderWidth) {
            FragColor = uCornerColor;
        }
    }
}
