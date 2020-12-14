#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 7) in mat4 voxMod;


//out vec3 outVoxelPos;
out vec4 vertexColor;

uniform mat4 dView;
uniform mat4 dProj;

void main()
{
    // copy the voxelpositions to a vector for easy management. 
    vec3 voxPos = vec3(voxMod[3][0] , voxMod[3][1], voxMod[3][2]);

    gl_Position = dProj * dView * voxMod * vec4(aPos, 1.0); 
    //outVoxelPos = aVoxelPos; 

    vertexColor = vec4(voxPos.x, voxPos.y, voxPos.z, 1);
}