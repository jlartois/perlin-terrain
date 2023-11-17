#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glHelper.h"
#include <iostream>

// From CMAKE preprocessor
std::string cmakelists_dir = CMAKELISTS_SOURCE_DIR;


int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
		return -1;
	}

	GlHelper glHelper;

	int resolution = 1000;
	GLFWwindow* window = glHelper.createWindow(resolution);
	if (!window) {
		return -1;
	}

	Shader shaderProgram;
	std::string srcPath = cmakelists_dir + "/src/"; // path to folder containing shader files
	if (!glHelper.createShaderProgram(shaderProgram, srcPath)) {
		glfwTerminate();
		return -1;
	}

	int nrIndices;
	int nrSubdivisions = 150; // Number of terrain mesh subdivisions
	glHelper.createEBOandVBO(nrIndices, nrSubdivisions);

	if (!glHelper.createTexture(argv[1])) {
		glfwTerminate();
		return -1;
	}

	glHelper.setupImGui();
	// some state to keep track off
	float elevation = 0.7f; // rotation around horizontal axis
	float azimuth = 0;   // rotation around vertical axis
	float scale = 0.5f;     // zoom perlin noise in/out
	float offset = 0;   // offset perlin noise
	float terrainHeight = 0.2f; 
	float zoom = 1.4f;      // zoom in and out of the screen
	bool wireframe = false;
	shaderProgram.setFloat("elevation", elevation);
	shaderProgram.setFloat("azimuth", azimuth);
	shaderProgram.setFloat("scale", scale);
	shaderProgram.setFloat("offset", offset);
	shaderProgram.setFloat("terrainHeight", terrainHeight);
	shaderProgram.setFloat("zoom", zoom);
	// Update the projection matrix used in the vertex shader
	glHelper.setPerspectiveMatrix();

	// Enable z-depth testing
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		// Poll for and process events, e.g. keyboard handling
		glfwPollEvents();

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Define the ImGui widgets
		ImGui::Begin("Perlin noise terrain generator");
		if (ImGui::SliderAngle("elevation", &elevation, -90, 90)) {
			shaderProgram.setFloat("elevation", elevation);
		}
		if (ImGui::SliderAngle("azimuth", &azimuth)) {
			shaderProgram.setFloat("azimuth", azimuth);
		}
		if (ImGui::SliderFloat("terrainHeight", &terrainHeight, -2, 2)) {
			shaderProgram.setFloat("terrainHeight", terrainHeight);
		}
		if (ImGui::SliderFloat("scale", &scale, -2, 2)) {
			shaderProgram.setFloat("scale", scale);
		}
		if (ImGui::SliderFloat("offset", &offset, -2, 2)) {
			shaderProgram.setFloat("offset", offset);
		}
		if (ImGui::SliderFloat("zoom", &zoom, 0.1f, 2)) {
			shaderProgram.setFloat("zoom", zoom);
		}
		if (ImGui::Checkbox("Wireframe mode", &wireframe)) {
			glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE : GL_FILL);
		}
		if (ImGui::SliderInt("Nr mesh subdivisions", &nrSubdivisions, 2, 200)) {
			glHelper.createEBOandVBO(nrIndices, nrSubdivisions);
		}
		ImGui::End();

		// Clear the color and depth buffer (to which the fragment shader writes)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw the vertices whose indices are listed in the EBO
		glDrawElements(GL_TRIANGLES, nrIndices, GL_UNSIGNED_INT, 0);

		// render gui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap the front and back buffers
		glfwSwapBuffers(window);
	}

	// Clean up
	glHelper.cleanup();
	glfwTerminate();

	return 0;
}
