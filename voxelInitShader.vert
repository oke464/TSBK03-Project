#version 410
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
    float toPixelScale = 0.00185185185; // 2/1080
    float scaleOffset = gl_InstanceID*toPixelScale;
    // Create a translation matrix, it is just in x-dir to offset a quad rendered to offscreen fbo to init colors one pixel (I hope) 
    // at a time. Color corresponding to position of voxel. 
    mat4 translate;
    translate[3][0] = scaleOffset;
    translate[3][1] = 0;
    translate[3][2] = 0;
    translate[0][0] = 1;
    translate[1][1] = 1;
    translate[2][2] = 1;
    translate[3][3] = 1;
    vec4 newpos = translate * vec4(aPos, 1.0);
    gl_Position = vec4(newpos);    
}