#vertex
#version 300 es
precision mediump float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uTransform;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vec4 worldPosition = uTransform * vec4(aPos, 1.0f);
    gl_Position = uProjection * uView * worldPosition;
}

#fragment
#version 300 es
precision mediump float;
out vec4 FragColor;
void main()
{
    FragColor = vec4(0.8, 0.3, 0.02, 1.0);
}
