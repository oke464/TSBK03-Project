#version 410
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 7) in mat4 voxMod;

out vec4 vertexColor; // specify a color output to the fragment shader

uniform mat4 dView;
uniform mat4 dProj;

uniform sampler2D FBOXmin;
uniform sampler2D FBOYmin;
uniform sampler2D FBOZmin;
uniform sampler2D FBOXmax;
uniform sampler2D FBOYmax;
uniform sampler2D FBOZmax;

uniform float near;
uniform float far;

uniform float texWidth;
uniform float texHeight;

uniform float modelScaleFactor;
uniform mat4 modelRotation;
uniform mat4 modelTranslation;
uniform mat4 originTranslation;

out vec2 outTexCoord;
out vec3 outNormal;

void main()
{
    outTexCoord = aTexCoords;
    outNormal = aNormal;

    // Transform voxel position coords to "look-up" coordinates for a 2D texture. 
    // For instance in x-direction, our "look-up" space is the yz-plane (yz component of voxPos), 
    // and the x-component of voxPos is the comparison value. NOTE all transformed. 
    vec3 voxPos = vec3(voxMod[3][0] , voxMod[3][1], voxMod[3][2]);

    // Transform voxPos [0,10]->z-bufferSpace[0,1]->textureIndexSpace[0,texWidth] or [0,texHeight]
    // In x-dir y-dir is height and z-dir is width
    float yHeightLookUp = (voxPos.y / (far - near)) * texHeight;
    float zWidthLookUp = (voxPos.z / (far - near)) * texWidth;
    // In y-dir x-dir is height and z-dir is width
    float xHeightLookUp = (voxPos.x / (far - near)) * texHeight;
    // In z-dir y-dir is height and x-dir is width
    float xWidthLookUp = (voxPos.x / (far - near)) * texWidth;

    // All voxPos coordinates in z-buffer range 0,1 for comparison
    float voxPosDepthX = voxPos.x / (far - near); 
    float voxPosDepthY = voxPos.y / (far - near); 
    float voxPosDepthZ = - voxPos.z / (far - near); 
    
    // Fetch texel on transformed coordinate from voxel position. This is the comparison value for each component. 
    // offset a little bit to make more difference between min and max
    float offset = 0.5;
    // In x-dir we have the yz plane as look-up coords
    // Note that we have to offset coordinates in  width direction, to start from the correct corner--positive left to right, 
    // will otherwise be looking from right to left. zWidthLookUp is negative from voxel orientation. 
    vec4 xMinDepth = texelFetch(FBOXmin, ivec2(texWidth + zWidthLookUp, yHeightLookUp), 0);
    vec4 xMaxDepth = texelFetch(FBOXmax, ivec2(texWidth + zWidthLookUp, yHeightLookUp), 0);
    // In y-dir we have the xz plane as look-up coords
    vec4 yMinDepth = texelFetch(FBOYmin, ivec2(-zWidthLookUp, xHeightLookUp), 0);
    vec4 yMaxDepth = texelFetch(FBOYmax, ivec2(-zWidthLookUp, xHeightLookUp), 0);
    // In z-dir we have the xy plane as look-up coords
    vec4 zMinDepth = texelFetch(FBOZmin, ivec2(xWidthLookUp, yHeightLookUp), 0);
    vec4 zMaxDepth = texelFetch(FBOZmax, ivec2(xWidthLookUp, yHeightLookUp), 0);

    // Create scale matrix from scalefactor passed from host program. 
    mat4 scaleMatrix = mat4(0);
    scaleMatrix[0][0] = modelScaleFactor;
    scaleMatrix[1][1] = modelScaleFactor;
    scaleMatrix[2][2] = modelScaleFactor;
    scaleMatrix[3][3] = 1;
    
    // If alpha channel == 0 then we have background color so don't skip if so.
    // It does not seem to be needed because the other tests filters this by default.
    //if (xMinDepth.a != 0 && xMaxDepth.a != 0 && yMinDepth.a != 0 && yMaxDepth.a != 0 && zMinDepth.a != 0 && zMaxDepth.a != 0)
    //{

        // Check if voxel is active, within constraints of depthdata. 
        // Can use either .x .y or .z component since depth data is grey (all components have same value)
        if (voxPosDepthX >= (xMinDepth.x) && voxPosDepthX <= (xMaxDepth.x))
        {
            if (voxPosDepthY >= (yMinDepth.x) && voxPosDepthY <= (yMaxDepth.x))
            {
                if (voxPosDepthZ >= (zMinDepth.x) && voxPosDepthZ <= (zMaxDepth.x))
                {
                    gl_Position = dProj * dView * scaleMatrix * modelTranslation * modelRotation * originTranslation * voxMod * vec4(aPos, 1.0); 
                    vertexColor = vec4(voxPosDepthX, voxPosDepthY, voxPosDepthZ, 1.0);
                }
            }
        }

    //}
}