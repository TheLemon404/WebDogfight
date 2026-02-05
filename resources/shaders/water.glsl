#vertex
#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vec4 worldPosition = uView * vec4(aPos, 1.0f);
    gl_Position = uProjection * worldPosition;
}

#fragment
#version 300 es
precision highp float;

uniform vec3 uSunDirection;
uniform float uAlpha;
uniform vec3 uAlbedo;
uniform vec3 uShadowColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(uAlbedo, uAlpha);
}
