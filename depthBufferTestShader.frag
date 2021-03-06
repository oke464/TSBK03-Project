#version 330
out vec4 FragColor;
in vec3 outNormal;

in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

uniform float near;
uniform float far;

uniform float scaleFactor;

// Function from wikipedia for orthographic projection, in which this shader should use.
// Gives linearized value of depth [0,1] between near and far. When z_coord is in view coords. 
float transformDepthVal(float z_viewCoord)
{
    float z_prime = (2 * (z_viewCoord - near)) / (far - near) - 1; 
    return z_prime;
} 

// from z_prime depth to z_viewCoord we now can invert the function:
float transformDepthValToView(float z_prime)
{
    float z_viewCoord = ((z_prime + 1) * (far - near)) / 2 + near; 
    return z_viewCoord;
} 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    // -------Fetch depth data and write to FBO---------
    // Assuming early depth testing. Then frag values should be filtered from z-buffer.
    float z_prime;
    z_prime = transformDepthVal(gl_FragCoord.z);

    float z_viewCoord;
    z_viewCoord = transformDepthValToView(gl_FragCoord.z);

    float z = LinearizeDepth(gl_FragCoord.z);

    // Trying with plain fragcoord, looks linear to me.. 
    // Minus and plus half is to transform to 0,0 and then put a scale to separate the max and min texture a little bit. Makes z-depth only the best actually.
    FragColor = vec4(vec3((gl_FragCoord.z - 0.5) * scaleFactor + 0.5) , 1.0);
    // -------------------------------------------------
} 