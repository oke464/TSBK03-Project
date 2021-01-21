#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 7) in mat4 voxelModelMatrix;


//out vec3 outVoxelPos;
out vec4 vertexColor;

uniform mat4 View;
uniform mat4 Proj;

uniform float modelScaleFactor;
uniform mat4 modelRotation;
uniform mat4 modelTranslation;
uniform mat4 originTranslation;

void main()
{
    // copy the voxelpositions to a vector for easy management. 
    vec3 voxPos = vec3(voxelModelMatrix[3][0] , voxelModelMatrix[3][1], voxelModelMatrix[3][2]);

    // Create scale matrix from scalefactor passed from host program. 
    mat4 scaleMatrix = mat4(0);
    scaleMatrix[0][0] = modelScaleFactor;
    scaleMatrix[1][1] = modelScaleFactor;
    scaleMatrix[2][2] = modelScaleFactor;
    scaleMatrix[3][3] = 1;

    gl_Position = Proj * View * modelTranslation * modelRotation * scaleMatrix * originTranslation * voxelModelMatrix * vec4(aPos, 1.0);  
    //outVoxelPos = aVoxelPos; 

    vertexColor = vec4(voxPos.x, voxPos.y, -voxPos.z, 1) / 10;
}