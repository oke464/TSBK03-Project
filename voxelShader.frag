#version 330
out vec4 FragColor;

in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  
in vec3 outVoxelPos;


uniform float near;
uniform float far;

uniform sampler2D texFBOX;
uniform sampler2D texFBOY;
uniform sampler2D texFBOZ;
uniform sampler2D texFBOXGreater;
uniform sampler2D texFBOYGreater;
uniform sampler2D texFBOZGreater;

in vec2 outTexCoord;
//uniform int INIT;

void main()
{
    int INIT = 0;
    // -------Fetch depth data from textures and write to FBO---------
    if (INIT == 1)
    {
        // The thought is to fill the frustum box i.e. voxel space.
        // To do this fill a texture with initial positions which is all voxels with certain radius that fit in box:
        //      a far*far*far box with coordinates, spaced with 2*radius. Then a voxel box/sphere can be placed on each position later.
        // Set upload INIT = 1 when we want to init. 
        // Maybe have a separate texture only for initialized texture, then copy all to another texture which is voxelpos texture.
        // Upload Texture coordinates manually. The texture coord steps will be 1/texWidth or 1/texHeight if texcoord 0->1 in both dirs.

        float x = near;
        float y = near;
        float z = near;
        float activeVoxel = 0; // Let alpha=0 be inactive
        
        // Fill a far*far*far box with coordinates, spaced with 2*radius
        while (z < far)
        {
            y = near; // reset 

            while(y < far)
            {
                x = near; // reset 

                while (x < far)
                {

                    x += 1;
                }

                y += 1;
            }

            z += 1;
        }

    }

    // Extract depthbuffer data from texture
    vec4 x_min = texture(texFBOX, outTexCoord);
    vec4 x_max = texture(texFBOXGreater, outTexCoord);
    vec4 y_min = texture(texFBOY, outTexCoord);
    vec4 y_max = texture(texFBOYGreater, outTexCoord);
    vec4 z_min = texture(texFBOZ, outTexCoord);
    vec4 z_max = texture(texFBOZGreater, outTexCoord);

    // Transform voxPos to depthbuffer coordinates.
    vec3 voxPosDepth = outVoxelPos / (far - near);
    
    // Check if voxPos is inside limits from depth buffer data.
    
    if (voxPosDepth.x > x_min.x && voxPosDepth.x < x_max.x)
    {
        if(voxPosDepth.y > y_min.x && voxPosDepth.y < y_max.x)
        {
            if(voxPosDepth.z > z_min.x && voxPosDepth.z < z_max.x)
            {
                FragColor = vec4(outVoxelPos.x ,outVoxelPos.y ,outVoxelPos.z ,1);
            }
        }
    }
    
    FragColor = vec4(outVoxelPos.x ,outVoxelPos.y ,outVoxelPos.z,0.5);//vertexColor;//texture(texFBOY, outTexCoord);
    // -------------------------------------------------
} 