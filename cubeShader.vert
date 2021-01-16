#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 vertexColor; 
out vec3 outNormal;
out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0); 

    outTexCoord = aTexCoords;
    outNormal = aNormal;
    vertexColor = vec4(1, 1, 1, 1.0); 
}