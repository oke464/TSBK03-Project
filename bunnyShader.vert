#version 330
layout (location = 0) in vec3 aPos;

out vec4 vertexColor; // specify a color output to the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0); 
    vertexColor = vec4(1,1,1, 1.0); 
}