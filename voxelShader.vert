#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 7) in mat4 voxMod;

out vec4 vertexColor; // specify a color output to the fragment shader
//out vec3 outNormal;

out vec2 outTexCoord;

uniform mat4 dView;
uniform mat4 dProj;

uniform sampler2D texFBOX;
uniform sampler2D texFBOY;
uniform sampler2D texFBOZ;
uniform sampler2D texFBOXGreater;
uniform sampler2D texFBOYGreater;
uniform sampler2D texFBOZGreater;

uniform float near;
uniform float far;

void main()
{
    outTexCoord = aTexCoords;

    // Extract depthbuffer data from texture
    vec4 x_min = textureLod(texFBOX, aTexCoords, 0.0);
    vec4 x_max = textureLod(texFBOXGreater, aTexCoords, 0.0);
    vec4 y_min = textureLod(texFBOY, aTexCoords, 0.0);
    vec4 y_max = textureLod(texFBOYGreater, aTexCoords, 0.0);
    vec4 z_min = textureLod(texFBOZ, aTexCoords, 0.0);
    vec4 z_max = textureLod(texFBOZGreater, aTexCoords, 0.0);

    // Copy the voxelpositions to a vector for easy management. 
    vec3 voxPos = vec3(voxMod[3][0] , voxMod[3][1], voxMod[3][2]);

    // Transform voxPos to depthbuffer coordinates.
    vec3 voxPosDepth = voxPos / (far - near);
    
    // Check if voxPos is inside limits from depth buffer data.
    
    if (voxPosDepth.x > x_min.x && voxPosDepth.x < x_max.x)
    {
        if(voxPosDepth.y > y_min.x && voxPosDepth.y < y_max.x)
        {
            if(voxPosDepth.z > z_min.x && voxPosDepth.z < z_max.x)
            {
                
            }
        }
    }
    gl_Position = dProj * dView * voxMod * vec4(aPos, 1.0); 
    //outVoxelPos = aVoxelPos; 
    //vertexColor = vec4(voxPos.x, voxPos.y, voxPos.z, 1);
    vertexColor = x_min;


    
}