#ifndef GLHELPERS_H
#define GLHELPERS_H

#include "shader.h"


class GlHelper {
public:
	GLFWwindow* window;
	GLuint VAO = NULL;
	GLuint VBO, EBO;
	unsigned int* vertexIndicesPerQuad = NULL;
	float* texCoordsPerVertex = NULL;
	GLuint texture;
	Shader shaderProgram;

	GlHelper() {}

	// Use GLFW and GLAD to create a window
	GLFWwindow* createWindow(int resolution) {
		// Initialize GLFW
		if (!glfwInit()) {
			std::cerr << "Failed to initialize GLFW" << std::endl;
			return false;
		}

		// Set GLFW to not create an OpenGL context (version 330 core)
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


		// Create a GLFW window
		GLFWwindow* window = glfwCreateWindow(resolution, resolution, "Perlin terrain", nullptr, nullptr);
		if (!window) {
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return NULL;
		}

		// Make the OpenGL context current
		glfwMakeContextCurrent(window);

		// Initialize GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cerr << "Failed to initialized GLAD" << std::endl;
			glfwTerminate();
			return NULL;
		}
		this->window = window;
		return window;
	}

	// Create the shader program by compiling the vertex and fragment shaders
	bool createShaderProgram(Shader& shaderProgram, std::string srcPath) {
		if (!shaderProgram.init((srcPath + "vertex_shader.fs").c_str(), (srcPath + "fragment_shader.fs").c_str())) {
			std::cout << "failed to compile " << srcPath + "vertex.fs" << " or " << srcPath + "fragment.fs" << std::endl;
			return false;
		}
		// bind the shader program
		shaderProgram.use();
		this->shaderProgram = shaderProgram;
		return true;
	}

	// Create a EBO to define what triangles are formed by what vertices
	// Create a VBO to define the texture coordinates of each vertex
	// Create a VAO to simply hold the EBO and VBO
	void createEBOandVBO(int& nrIndices, int N) {
		// Create a 
		//  1. vertex array object (VAO) (will contain the VBO and EBO)
		//  2. vertex buffer object (VBO) (will contain the positions and texture coordinates of each vertex)
		//  3. element buffer object (EBO) (will contain the indices of vertices to build triangles, kinda like in an .obj file)
		// and bind them
		if (!VAO) {
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
		}
		glBindVertexArray(VAO); // bind the VAO, after which the VBO and EBO need to be binded

		// fill the EBO
		// 3 vertices per triangle, 2 triangles per quad => 6 vertex indices per quad
		// N x N quads
		nrIndices = 6 * N * N;
		if (vertexIndicesPerQuad) {
			delete[] vertexIndicesPerQuad;
		}
		vertexIndicesPerQuad = new unsigned int[nrIndices];
		unsigned int t = 0;
		for (int row = 0; row < (N + 1) * N; row += N + 1) {
			for (int col = 0; col < N; col++) {
				vertexIndicesPerQuad[t] = row + col;
				vertexIndicesPerQuad[t + 1] = row + N + col + 1;
				vertexIndicesPerQuad[t + 2] = row + col + 1;
				vertexIndicesPerQuad[t + 3] = row + col + 1;
				vertexIndicesPerQuad[t + 4] = row + N + col + 1;
				vertexIndicesPerQuad[t + 5] = row + N + col + 2;
				t += 6;
			}
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nrIndices, vertexIndicesPerQuad, GL_STATIC_DRAW);

		// fill the VBO with 2D texture coordinates (u,v)
		int nrVertices = (N + 1) * (N + 1);
		if (texCoordsPerVertex) {
			delete[] texCoordsPerVertex;
		}
		texCoordsPerVertex = new float[nrVertices * 2];
		t = 0;
		float N_f = float(N);
		for (int row = 0; row < N + 1; row++) {
			for (int col = 0; col < N + 1; col++) {
				texCoordsPerVertex[t] = col / N_f;
				texCoordsPerVertex[t + 1] = row / N_f;
				t += 2;
			}
		}

		// Our vertex shader needs, per vertex:
		//   1. the 2D texture coordinate (u,v)
		// In OpenGL, info like this are called vertex attributes
		// We now need to pass a pointer to the vertex attributes to OpenGL
		// (so that the vertex shader can access them)
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nrVertices * 2, texCoordsPerVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	// create an OpenGL texture and fill it with the perlin noise texture
	bool createTexture(const char* texturePath) {
		// Load image using stb_image
		int width, height, channels;
		unsigned char* image = stbi_load(texturePath, &width, &height, &channels, STBI_grey);
		if (!image) {
			std::cout << "Error: failed to load texture " << texturePath << std::endl;
			glfwTerminate();
			return false;
		}
		// Generate and bind an OpenGL texture
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		// Set texture parameters (how out of bounds indices are handled and interpolation between pixels-
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Load image data into the texture (GL_RED because 1 channel)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image);
		// Free stb_image data
		stbi_image_free(image);

		// we have 1 texture, which we give index 0, so "GL_TEXTURE0"
		glActiveTexture(GL_TEXTURE0);
		// bind our texture to GL_TEXTURE0
		glBindTexture(GL_TEXTURE_2D, texture); 
		// tell the shader program that it can find its "PerlinTexture" at index 0 (i.e. GL_TEXTURE0)
		shaderProgram.setInt("PerlinTexture", 0);

		return true;
	}

	// Setup Dear ImGui
	void setupImGui() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	// create a projection matrix
	void setPerspectiveMatrix() {

		float fov = 45.0f;          // Field of view in degrees
		float nearPlane = 0.1f;     // Near clipping plane
		float farPlane = 100.0f;    // Far clipping plane

		float aspectRatio = 1;

		float tanHalfFOV = tan(0.5f * fov * (M_PI / 180.0f));
		float range = nearPlane - farPlane;

		float projectionMatrix[16] = { // column major
			1.0f / (tanHalfFOV * aspectRatio), 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / tanHalfFOV, 0.0f, 0.0f,
			0.0f, 0.0f, (nearPlane + farPlane) / range, -1.0f,
			0.0f, 0.0f, (2.0f * farPlane * nearPlane) / range, 0.0f
		};

		glUniformMatrix4fv(shaderProgram.getUniformLocation("proj"), 1, GL_FALSE, projectionMatrix);
	}

	void cleanup() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		delete[] vertexIndicesPerQuad;
		delete[] texCoordsPerVertex;
		glDeleteTextures(1, &texture);

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
};



#endif