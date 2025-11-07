#include "Shaders.h"


const char* MyVertShader = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aNormal;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	out vec3 Normal;
	out vec3 FragPos;

	void main()
	{
		FragPos = vec3(model * vec4(aPos, 1.0));
		Normal = mat3(transpose(inverse(model))) * aNormal; // normal correctamente transformada
		gl_Position = projection * view * model * vec4(aPos, 1.0);
	}
    )";

const char* MyFragShader = R"(
	#version 330 core
	out vec4 FragColor;

	in vec3 Normal;
	in vec3 FragPos;

	const vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
	uniform vec4 color;
	
	void main()
	{
		vec3 norm = normalize(Normal);
		vec3 c = color.rgb;
		float diff = max(dot(norm, lightDir), 0.0);

		vec3 ambient = 0.2 * c;
		vec3 diffuse = diff * c;
		vec3 result = ambient + diffuse;

	    FragColor = vec4(result, 1.0);
	}
    )";

const char* GridVtxShader = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;
	uniform mat4 view;
	uniform mat4 projection;
	
	out vec3 worldPos;
	
	void main() {
	    worldPos = aPos;
	    gl_Position = projection * view * vec4(aPos, 1.0);
	}
	)";

const char* GridFragShader = R"(
	#version 330 core
	in vec3 worldPos;
	out vec4 FragColor;
	
	uniform vec3 gridColor = vec3(0.92, 0.847, 0.031);
	
	void main() {
	    float scale = 3.0;//tile size
	    float line = 0.2;//line thickness
	
	    //XZ coords
	    float gx = abs(fract(worldPos.x / scale - 0.5) - 0.5) / fwidth(worldPos.x);
	    float gz = abs(fract(worldPos.z / scale - 0.5) - 0.5) / fwidth(worldPos.z);
	
	    float g = min(gx, gz);
	    float gridLine = 1.0 - smoothstep(0.0, line, g);
	
	    FragColor = vec4(mix(vec3(0.0), gridColor, gridLine), 1.0);
	}
	)";


const char* SliceVtxShader = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;
	uniform mat4 u_MVP;
	void main() {
	    gl_Position = u_MVP * vec4(aPos, 1.0);
	}
	)";

const char* SliceFragShader = R"(
	#version 330 core
	out vec4 FragColor;
	uniform vec4 u_Color;
	void main() {
	    FragColor = u_Color;
	}
	)";