#version 410
out vec4 FragColor;
in vec4 vertexColor;


in vec2 outTexCoord;
in vec3 outNormal;

uniform sampler2D texFBOY;
void main()
{

    const vec3 light = vec3(-0.5, 0.5, 0.3); 
	float diffuse, specular, shade;

	// Diffuse
	diffuse = dot(normalize(outNormal), light);
	diffuse = max(0.0, diffuse); // No negative light

	// Specular
	vec3 r = reflect(-light, normalize(outNormal));
	vec3 v = normalize(outNormal); // View direction

	shade = 0.7 * diffuse;
	FragColor = vec4(shade* vertexColor.x, shade * vertexColor.y, shade * vertexColor.z, 1.0);
    
    //FragColor = texture(texFBOY, outTexCoord); // To display texture to see that it works here
    FragColor = vertexColor;//vertexColor;//texture(texFBOY, outTexCoord);
    // -------------------------------------------------
} 