#version 330 core

// get the vertex attributes from the VBO
// in this case, these are (u,v) texture coordinates
layout (location = 0) in vec2 texCoord;

// imgui variables
uniform float elevation;
uniform float azimuth;
uniform float scale;
uniform float offset;
uniform float terrainHeight;
uniform float zoom;

// projection matrix
uniform mat4 proj;

// input perlin texture
uniform sampler2D PerlinTexture;

// TexCoord is passed to the fragment shader
out vec2 TexCoord;

// executed once per vertex
void main()
{
	// sample the texture
	float perlinNoise = texture(PerlinTexture, texCoord * scale + vec2(offset, offset)).x;

	// calculate the 3D position of the current vertex
	float x = texCoord.x - 0.5f;
	float y = (perlinNoise - 0.5f) * terrainHeight;
	float z = texCoord.y - 0.5f;

	// rotate around the X axis
	y = y * cos(elevation) - z * sin(elevation);
	z = y * sin(elevation) + z * cos(elevation);
	
	// rotate around the Y axis
	float temp_x = x * cos(azimuth) + z * sin(azimuth);
	z = -x * sin(azimuth) + z * cos(azimuth);
	x = temp_x;

	// move backwards
	z = z - 2.0f / zoom;

	// project the 3D position of the vertex on the screen,
	// these are called NDC (normalized device coordinates)
    vec4 normalized_device_coordinates = proj * vec4(x, y, z, 1.0);

	// in an OpenGL vertex shader, we need to write the NDC to "gl_Position"
	gl_Position = normalized_device_coordinates;

    TexCoord = texCoord;
}