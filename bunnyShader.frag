#version 330
out vec4 FragColor;
in vec3 outNormal;

in vec4 vertexColor;  

void main()
{
    const vec3 light = vec3(0.58, 0.58, 0.58); 
	float diffuse, specular, shade;

	// Diffuse
	diffuse = dot(normalize(outNormal), light);
	diffuse = max(0.0, diffuse); // No negative light

	// Specular
	vec3 r = reflect(-light, normalize(outNormal));
	vec3 v = normalize(outNormal); // View direction
	//specular = dot(r, v);
	//if (specular > 0.0)
	//	specular = 1.0 * pow(specular, 50.0);
	//specular = max(specular, 0.0);
	//shade = 0.7 * diffuse + 10.0 * specular; Disable specular for now.
	shade = 0.7 * diffuse;
	FragColor = vec4(shade* vertexColor.x, shade * vertexColor.y, shade * vertexColor.z, 1.0);
} 