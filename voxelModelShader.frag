#version 410
out vec4 FragColor;
in vec4 vertexColor;


in vec2 outTexCoord;
uniform sampler2D texFBOY;
void main()
{
    
    //FragColor = texture(texFBOY, outTexCoord); // To display texture to see that it works here
    FragColor = vertexColor;//vertexColor;//texture(texFBOY, outTexCoord);
    // -------------------------------------------------
} 