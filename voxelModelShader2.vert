#version 410
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 7) in mat4 voxMod;

out vec4 vertexColor; // specify a color output to the fragment shader

uniform mat4 dView;
uniform mat4 dProj;

uniform sampler2D FBOXminActive;
uniform sampler2D FBOYminActive;
uniform sampler2D FBOZminActive;
uniform sampler2D FBOXmaxActive;
uniform sampler2D FBOYmaxActive;
uniform sampler2D FBOZmaxActive;

out vec2 outTexCoord;


// Use gl_InstanceID for getting the instnace number

void main()
{
    outTexCoord = aTexCoords;

    // Get the color on position of the instance number only shifts in x-dir
    // so we should be able to sample at constant y=0, i.e first value.
    vec4 xMinCheck = texelFetch(FBOXminActive, ivec2(gl_InstanceID, 0), 0);
    vec4 yMinCheck = texelFetch(FBOYminActive, ivec2(gl_InstanceID, 0), 0);
    vec4 zMinCheck = texelFetch(FBOZminActive, ivec2(gl_InstanceID, 0), 0);

    vec4 xMaxCheck = texelFetch(FBOXmaxActive, ivec2(gl_InstanceID, 0), 0);
    vec4 yMaxCheck = texelFetch(FBOYmaxActive, ivec2(gl_InstanceID, 0), 0);
    vec4 zMaxCheck = texelFetch(FBOZmaxActive, ivec2(gl_InstanceID, 0), 0);

    vec3 voxPos = vec3(voxMod[3][0] , voxMod[3][1], voxMod[3][2]);

    // Check if voxel is active. First check aplha channel, if == 0 then it is a background color.
    if (xMinCheck.a == 0 || yMinCheck.a == 0 || zMinCheck.a == 0 || xMaxCheck.a == 0 || yMaxCheck.a == 0 || zMaxCheck.a == 0)
    {
        
    }
    else
    {
        
    }

    if (xMinCheck.x < 0 &&
        xMinCheck.y < 0 &&
        xMinCheck.z < 0)
    {
        gl_Position = dProj * dView * voxMod * vec4(aPos, 1.0); 
        
        vertexColor = vec4(xMinCheck.xy, -xMinCheck.z, 1.0);
    }
    gl_Position = dProj * dView * voxMod * vec4(aPos, 1.0); 
        
    vertexColor = vec4(yMinCheck.xy, -yMinCheck.z, 1.0);

    // Only draw boxes if position is active
    // if(texPos.a == 1)
    // {
    //     gl_Position = dProj * dView * voxMod * vec4(aPos, 1.0); 
    //     //vertexColor = vec4(voxPos.x, voxPos.y, -voxPos.z, 1.0) / 10;   
    //     vertexColor = vec4(texPos.xy, -texPos.z, 1.0);
           
    // }
    
    // gl_Position = dProj * dView * voxMod * vec4(aPos, 1.0); 
    // //vertexColor = vec4(texPos.xy, -texPos.z, 1.0);
    // vertexColor = vec4(voxPos.xy, -voxPos.z, 1.0) / 10;
}