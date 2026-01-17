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
uniform sampler2D uFontTexture;

out vec4 FragColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(uFontTexture, pUV).r);
    FragColor = uColor * sampled;
}
