#vertex
#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uTransform;
uniform mat4 uProjection;

out vec3 pNormal;

void main()
{
    vec4 screenPosition = uTransform * vec4(aPos, 1.0f);
    gl_Position = uProjection * screenPosition;
}

#fragment
#version 300 es
precision highp float;

uniform vec4 uColor;

out vec4 FragColor;

void main()
{
    FragColor = uColor;
}
