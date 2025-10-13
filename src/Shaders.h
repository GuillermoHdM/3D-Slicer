#pragma once





const char const* MyVertShader = R"(
	#version 330 core
	layout (location = 0) in vec2 aPos;
	
	uniform mat4 model;
	uniform mat4 projection;
	
	void main()
	{
	    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
	}
    )";

const char const* MyFragShader = R"(
	#version 330 core
	out vec4 FragColor;
	
	uniform vec4 color;
	
	void main()
	{
	    FragColor = color;
	}
    )";

const char const* GridVtxShader = R"(
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

const char const* GridFragShader = R"(
	#version 330 core
	in vec3 worldPos;
	out vec4 FragColor;
	
	uniform vec3 gridColor = vec3(0.7, 0.7, 0.7);
	
	void main() {
	    float scale = 1.0;//tile size
	    float line = 0.02;//line thickness
	
	    //XZ coords
	    float gx = abs(fract(worldPos.x / scale - 0.5) - 0.5) / fwidth(worldPos.x);
	    float gz = abs(fract(worldPos.z / scale - 0.5) - 0.5) / fwidth(worldPos.z);
	
	    float g = min(gx, gz);
	    float gridLine = 1.0 - smoothstep(0.0, line, g);
	
	    FragColor = vec4(mix(vec3(0.0), gridColor, gridLine), 1.0);
	}
	)";