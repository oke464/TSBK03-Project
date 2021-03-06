#version 330
out vec4 FragColor;
in vec3 outNormal;

in vec2 outTexCoord;
uniform sampler2D texUnit;
in vec4 vertexColor; 

void main()
{
    vec4 col = texture(texUnit, outTexCoord);
    FragColor = col;
} 