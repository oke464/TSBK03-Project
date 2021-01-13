#version 410
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
                // This should be for final or something similar
                FragColor = vec4(outVoxelPos.x ,outVoxelPos.y ,outVoxelPos.z ,1);
            }
            else
            {
                FragColor = vec4(voxPosDepth,0);
            }
        }
        else
        {
            FragColor = vec4(voxPosDepth,0);
        }
    }
    else
    {
        FragColor = vec4(voxPosDepth,0);
    }

    FragColor = vec4(voxPosDepth.x ,voxPosDepth.y ,voxPosDepth.z,1); // This colors all and makes it so we can view voxelcube
    //FragColor = vec4(outVoxelPos.x ,outVoxelPos.y ,outVoxelPos.z,0.5);//vertexColor;//texture(texFBOY, outTexCoord);
    // -------------------------------------------------
} 