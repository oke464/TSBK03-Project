#version 410
out vec4 FragColor;

in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  
in vec3 outVoxelPos;


uniform float near;
uniform float far;

uniform sampler2D depthTex;

in vec2 outTexCoord;
//uniform int INIT;

void main()
{

    // Extract depthbuffer data from texture
    vec4 depthTexture = texture(depthTex, outTexCoord);

    // Transform voxPos to depthbuffer coordinates.
    vec3 voxPosDepth = outVoxelPos / (far - near);
    
    // Subtract voxel position from depth data. 
    // Sign convention: 
    //      If for the min limit, for instance x_min:
    //          x_min - voxPos < 0
    //      For the max limit, for instance x_max:
    //          x_max - voxPos > 0 
    // Then the voxel is inside the limits and thus is active. 

    // Also the alpha channel for the background a = 0 and for the depth data a = 1. 
    // So create a color for the voxelPosition with alpha = 0, then we can check 
    // that we don't have background color with alpha. 

    // Output subracted data to the offscreen FBO.
    FragColor = depthTexture - vec4(voxPosDepth, 0);
    //FragColor = texture(depthTex, outTexCoord);
} 