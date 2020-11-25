#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 3) in mat4 instanceMatrix;
  
out vec4 vertexColor; // specify a color output to the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 col;

void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0); 
    vertexColor = vec4(aCol, 1.0); 
}