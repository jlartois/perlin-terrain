#version 330 core

// from the vertex shader
in vec2 TexCoord;

// we need to calculate the color of the current pixel
out vec4 color;

// imgui variables
uniform float scale;
uniform float offset;
uniform sampler2D PerlinTexture;

// executed once per fragment (pixel)
void main()
{
	// sample texture
	float perlinNoise = texture(PerlinTexture, TexCoord * scale + vec2(offset, offset)).x;

	// rgba
    color = vec4(0, perlinNoise, 0, 1);
}