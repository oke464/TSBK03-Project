#version 330
out vec4 FragColor;
in vec3 outNormal;

in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

in vec2 outTexCoord;

uniform sampler2D texFBOX;
uniform sampler2D texFBOY;
uniform sampler2D texFBOZ;
uniform sampler2D texFBOXGreater;
uniform sampler2D texFBOYGreater;
uniform sampler2D texFBOZGreater;

uniform float voxelRadius;

uniform float near;
uniform float far;

uniform int INIT;

void main()
{
    // -------Fetch depth data from textures and write to FBO---------
    if (INIT == 1)
    {
        // The thought is to fill the frustum box i.e. voxel space.
        // To do this fill a texture with initial positions which is all voxels with certain radius that fit in box:
        //      a far*far*far box with coordinates, spaced with 2*radius. Then a voxel box/sphere can be placed on each position later.
        // Set upload INIT = 1 when we want to init. 
        // Maybe have a separate texture only for initialized texture, then copy all to another texture which is voxelpos texture.
        // 
        float x = near;
        float y = near;
        float z = near;
        float active = 0; // Let 0 be inactive
        
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
    
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    // -------------------------------------------------
} 