#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>

#include "stb_image.h"
#include "camera.h"
#include "shader.h"
#include "model.h"

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

unsigned int currentModel = 0;
unsigned int currentShader = 0;
bool canSwitchModel = true;
bool canSwitchShader = true;
bool toggleWireframe = true;

int main() {

	// Setup for window creation and OpenGL API

	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW!" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "ModelViewer", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// End of setup

	Shader shader1("./vertex_shader.glsl", "./fragment_shader.glsl");
	Shader normals("./vertex_shader.glsl", "./normals.glsl");
	Shader lightSource("./light_vertex.glsl", "./lightSource.glsl");

	Model subject;
	subject.loadOBJ("./monkey.obj");

	Model light;
	light.loadOBJ("./monkey.obj");

	
	/* Textures are unused right now.
	unsigned int texture1;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	*/
	

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Uncomment for Wireframe Mode!
	bool modelCanChange = true;
	float lastSwap = 0.0f;

	bool loadSuccess = true;

	Shader* shader = &shader1;

	shader->use();
	shader->setVec3("material.ambient", glm::vec3(0.329412f, 0.223529f, 0.027451f));
	shader->setVec3("material.diffuse", glm::vec3(0.780392f, 0.568627f, 0.113725f));
	shader->setVec3("material.specular", glm::vec3(0.992157f, 0.941176f, 0.807843f));
	shader->setFloat("material.shininess", 27.897f);

	glm::vec3 modelScale(1, 1, 1);

	// Model Viewer Main Loop
	// Move with						 [ W A S D]
	// Look with						 [ MOUSE ]
	// Cycle through preset models with  [SPACE]
	// Cycle through preset shaders with [L SHIFT]
	// Toggle Wireframe Mode with		 [L ALT]
	while (!glfwWindowShouldClose(window)) {

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glm::vec3 background(0.1f, 0.1f, 0.1f);

		// Rendering
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Model Swapping
		if (!canSwitchModel) {
			switch (currentModel % 9) {
			case 0:
				loadSuccess = subject.loadOBJ("./monkey.obj");
				shader->use();
				shader->setVec3("material.ambient", glm::vec3(0.329412f, 0.223529f, 0.027451f));
				shader->setVec3("material.diffuse", glm::vec3(0.780392f, 0.568627f, 0.113725f));
				shader->setVec3("material.specular", glm::vec3(0.992157f, 0.941176f, 0.807843f));
				shader->setFloat("material.shininess", 27.897f);
				canSwitchModel = true;
				modelScale = glm::vec3(1.0f);
				break;
			case 1:
				// Normal averaging process seems to have made the "patching" effect less noticable on the sphere.
				loadSuccess = subject.loadOBJ("./sphere.obj");
				shader->use();
				shader->setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
				shader->setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
				shader->setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
				shader->setFloat("material.shininess", 32.0f);
				canSwitchModel = true;
				modelScale = glm::vec3(1.0f);
				break;
			case 2:
				// Normal Averaging seems to have fixed the polar lighting on the cube. Still not too happy with the interpolation of normals for these low-poly models.
				loadSuccess = subject.loadOBJ("./cube.obj");
				shader->use();
				shader->setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
				shader->setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
				shader->setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
				shader->setFloat("material.shininess", 32.0f);
				canSwitchModel = true;
				modelScale = glm::vec3(1.0f);
				break;
			case 3:
				loadSuccess = subject.loadOBJ("./multiple.obj");
				shader->use();
				shader->setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
				shader->setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
				shader->setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
				shader->setFloat("material.shininess", 32.0f);
				canSwitchModel = true;
				modelScale = glm::vec3(1.0f);
				break;
			case 4: // The bunny, cow and dragon don't come with prepackaged normals, so are fairly boring to look at. May have to start calculating my own normals.
				// Bunny is tiny
				loadSuccess = subject.loadOBJ("./stanford-bunny.obj");
				shader->use();
				shader->setVec3("material.ambient", glm::vec3(0.25f, 0.20725f, 0.20725f));
				shader->setVec3("material.diffuse", glm::vec3(1.0f, 0.829f, 0.829f));
				shader->setVec3("material.specular", glm::vec3(0.296648f, 0.296648f, 0.296648f));
				shader->setFloat("material.shininess", 11.264f);
				modelScale = glm::vec3(10.0f);
				canSwitchModel = true;
				break;
			case 5:
				loadSuccess = subject.loadOBJ("./cow.obj");
				shader1.use();
				shader1.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
				shader1.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
				shader1.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
				shader1.setFloat("material.shininess", 32.0f);
				modelScale = glm::vec3(0.3f);
				canSwitchModel = true;
				break;
			case 6:
				// Dragon and beetle seem to be most affected by the strange rippling due to the normal averaging.
				loadSuccess = subject.loadOBJ("./beetle.obj");
				shader1.use();
				shader1.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
				shader1.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
				shader1.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
				shader1.setFloat("material.shininess", 32.0f);
				canSwitchModel = true;
				modelScale = glm::vec3(2.0f);
				break;
			case 7:
				loadSuccess = subject.loadOBJ("./xyzrgb_dragon.obj");
				shader1.use();
				shader1.setVec3("material.ambient", glm::vec3(0.135f, 0.2225f, 0.1575f));
				shader1.setVec3("material.diffuse", glm::vec3(0.54f, 0.89f, 0.63f));
				shader1.setVec3("material.ambient", glm::vec3(0.316228f, 0.316228f, 0.316228f));
				shader1.setFloat("material.shininess", 12.8f);
				modelScale = glm::vec3(0.01f);
				canSwitchModel = true;
				break;
			default:
				// Shoutout to Valve :)
				loadSuccess = subject.loadOBJ("./error.obj");
				shader1.use();
				shader1.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
				shader1.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
				shader1.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
				shader1.setFloat("material.shininess", 32.0f);
				canSwitchModel = true;
				modelScale = glm::vec3(1.0f);
				break;
			}
		}

		// Shader swapping
		if (!canSwitchShader) {
			switch (currentShader % 3) {
			case 0:
				shader = &shader1;
				canSwitchShader = true;
				break;
			case 1:
				shader = &normals;
				canSwitchShader = true;
				break;
			default:
				shader = &lightSource;
				canSwitchShader = true;
				break;
			}
		}

		// Load error model if load failed
		if (!loadSuccess) {
			loadSuccess = subject.loadOBJ("./error.obj");
		}

		// glBindTexture(GL_TEXTURE_2D, texture1);

		glm::vec3 lightPosition = glm::vec3(5.0f * glm::sin(currentFrame), 2.0f* glm::cos(currentFrame), 3.0f);

		shader->use();

		shader->setVec3("light.position", lightPosition);
		shader->setVec3("light.diffuse", glm::vec3(0.7f));
		shader->setVec3("light.ambient", 0.5f * background);
		shader->setVec3("light.specular", glm::vec3(1.0f));
		shader->setVec3("viewPos", camera.Position);

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		shader->setMat4("projection", projection);
		

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		shader->setMat4("view", view);

		glm::mat4 model = glm::scale(glm::mat4(1.0f), modelScale);
		model = glm::rotate(model, currentFrame, glm::vec3(0.f, 1.f, 0.f));
		shader->setMat4("model", model);
		subject.render(*shader);

		lightSource.use();
		lightSource.setMat4("projection", projection);
		lightSource.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.2f));
		lightSource.setMat4("model", model);

		lightSource.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		light.render(lightSource);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (canSwitchModel) {
			// std::cout << "Switching Model!" << std::endl;
			currentModel++;
			canSwitchModel = false;
		}
	}

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) 
	{
		if (canSwitchShader) {
			// std::cout << "Switching Shader!" << std::endl;
			currentShader++;
			canSwitchShader = false;
		}
	}

	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
		toggleWireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		toggleWireframe = !toggleWireframe;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}