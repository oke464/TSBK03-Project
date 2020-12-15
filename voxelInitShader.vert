#version 330
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 7) in mat4 voxMod;

out vec4 vertexColor; // specify a color output to the fragment shader
//out vec3 outNormal;
out vec3 outVoxelPos;
out vec2 outTexCoord;



// Use gl_InstanceID for getting the instnace number

void main()
{
    outTexCoord = aTexCoords;
    // Copy the voxelpositions to a vector for easy management. 
    vec3 voxPos = vec3(voxMod[3][0] , voxMod[3][1], voxMod[3][2]);
    outVoxelPos = voxPos; 

    gl_Position = vec4(aPos, 1.0);    
}