#version 330
out vec4 FragColor;

in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

uniform float near;
uniform float far;

uniform sampler2D texFBOY;

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
    
    FragColor = vertexColor;//texture(texFBOY, outTexCoord);
    // -------------------------------------------------
} 