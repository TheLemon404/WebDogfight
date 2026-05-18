#vertex
#version 300 es
precision mediump float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

out vec2 pUV;

void main()
{
    gl_Position = vec4(aPos, 1.0f);

    pUV = aUV;
}

#fragment
#version 300 es
precision mediump float;

in vec2 pUV;

uniform sampler2D uFrameBufferTexture;

out vec4 FragColor;

void main()
{
    FragColor = texture(uFrameBufferTexture, pUV);
}
