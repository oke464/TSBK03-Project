#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec4 vertexColor; // specify a color output to the fragment shader
out vec3 outNormal;

uniform mat4 model;
uniform mat4 dView;
uniform mat4 dProj;

void main()
{
    gl_Position = dProj * dView * model * vec4(aPos, 1.0); 

    outNormal = aNormal;

    vertexColor = vec4(1, 1, 1, 1.0); 
}