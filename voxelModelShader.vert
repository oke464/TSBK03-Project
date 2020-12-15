#version 410
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 7) in mat4 voxMod;

out vec4 vertexColor; // specify a color output to the fragment shader

uniform mat4 dView;
uniform mat4 dProj;

uniform sampler2D voxPosTex;



// Use gl_InstanceID for getting the instnace number

void main()
{
    // Get the color on position of the instance number
    vec4 texPos = texelFetch(voxPosTex, ivec2(1, gl_InstanceID), 0);

    vec3 voxPos = vec3(voxMod[3][0] , voxMod[3][1], voxMod[3][2]);


    // Only draw boxes if position is active
    if(texPos.a == 1)
    {
        gl_Position = dProj * dView * voxMod * vec4(aPos, 1.0); 
        vertexColor = texPos;   
           
    }
    //gl_Position = dProj * dView * voxMod * vec4(aPos, 1.0); 
    
}