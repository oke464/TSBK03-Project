#version 410
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 7) in mat4 voxMod;

out vec4 vertexColor; // specify a color output to the fragment shader

uniform mat4 dView;
uniform mat4 dProj;

uniform sampler2D voxPosTex;

out vec2 outTexCoord;


// Use gl_InstanceID for getting the instnace number

void main()
{
    outTexCoord = aTexCoords;

    // Get the color on position of the instance number
    // Texture sampled from is amount of voxels in width and 1 in height,
    // so we should be able to sample at constant y=0, i.e first value.
    vec4 texPos = texelFetch(voxPosTex, ivec2(gl_InstanceID, 0), 0);


    vec3 voxPos = vec3(voxMod[3][0] , voxMod[3][1], voxMod[3][2]);


    // Only draw boxes if position is active
    if(texPos.a == 1)
    {
        gl_Position = dProj * dView * voxMod * vec4(aPos, 1.0); 
        //vertexColor = vec4(voxPos.x, voxPos.y, -voxPos.z, 1.0) / 10;   
        vertexColor = vec4(texPos.xy, -texPos.z, 1.0);
           
    }
    
    //gl_Position = dProj * dView * voxMod * vec4(aPos, 1.0); 
    //vertexColor = vec4(texPos.xy, -texPos.z, 1.0);
}