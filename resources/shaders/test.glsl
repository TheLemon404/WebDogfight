#vertex
#version 300 es
precision mediump float;
layout(location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#fragment
#version 300 es
precision mediump float;
out vec4 FragColor;
void main()
{
    FragColor = vec4(0.8, 0.3, 0.02, 1.0);
}
