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

#define G_FORCE_PASS_OUT 100.0f

precision mediump float;

in vec2 pUV;

uniform sampler2D uFrameBufferTexture;
uniform mediump sampler3D uLUT;
uniform float uGForceSum;

out vec4 FragColor;

void main()
{
    float gForceVignette = (max(uGForceSum - G_FORCE_PASS_OUT, 0.0) * distance(pUV, vec2(0.5f))) / 120.0f;
    vec4 inputColor = max(texture(uFrameBufferTexture, pUV) - vec4(gForceVignette, gForceVignette, gForceVignette, 0.0), vec4(0.0, 0.0, 0.0, 1.0));
    vec3 mappedColor = texture(uLUT, inputColor.rgb).rgb;

    FragColor = vec4(mappedColor, inputColor.a);
}
