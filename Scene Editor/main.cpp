#include<iostream>
#include"GetSelectFilePath.h"
#include<string>
#include<thread>
#include<glad/include/glad/glad.h>
#include<GLFW/glfw3.h>
//#include<myShader.h>
#include<myShader.h>
#include<Camera.h>
#include"Model.h"
#include"ModelsRender.h"
#include"Mesh.h"
#include<glm/glm/glm.hpp>
#include<glm/glm/gtc/matrix_transform.hpp>
#include<glm/glm/gtc/type_ptr.hpp>

//地形图
#include"Terrain.h"
#include<vector>
#define MAX_MAP 1025


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//const GLuint WIDTH = 1000, HEIGHT = 800;
glm::vec3 lightPos(4.2f, 8.0f, 2.0f);
GLuint loadTexture(GLchar* path);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods_Bit);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();


// Camera
Camera camera;

bool mouse_button_left_pressed = false, mouse_button_right_pressed = false, mouse_button_middle_pressed = false;

bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


//layout (location = 0) 
float mixValue = 0.1f;


//地形图声明
glm::vec2 bl(-50, -50);
float HighMap[MAX_MAP * MAX_MAP];//地形图用的网格信息
Terrain ourTerrain("../assets/terrain/high.txt", HighMap, MAX_MAP, 0.1, bl);//构造地形图类

GLuint loadCubemap(vector<const GLchar*> faces);//这个是搞立方体贴图的

//model
ModelUnit model1;
ModelsRender modelsRender; 

void frame_buffer_callback(GLFWwindow* win, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* win)
{
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(win, true);
	}

	if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.001f;
		if (mixValue >= 1.0f)
		{
			mixValue = 1.0f;
		}
	}
	if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.001f;
		if (mixValue <= 0.0f)
		{
			mixValue = 0.0f;
		}
	}

}

void LoadSkyboxPath(char*& right_path, char*& left_path, char*& top_path, char*& bottom_path, char*& back_path, char*& front_path) {
	//cout << "Please choose the skybox file directory: \n";
	string sky_path_str = GetSelectFolderPath_string();
	if (sky_path_str == "")
	{
		sky_path_str = "../assets/skybox/sky5";
	}
	string tmp_path;
	tmp_path = sky_path_str + "\\right.jpg";
	right_path = String2FilePath(tmp_path);
	tmp_path = sky_path_str + "\\left.jpg";
	left_path = String2FilePath(tmp_path);
	tmp_path = sky_path_str + "\\top.jpg";
	top_path = String2FilePath(tmp_path);
	tmp_path = sky_path_str + "\\bottom.jpg";
	bottom_path = String2FilePath(tmp_path);
	tmp_path = sky_path_str + "\\back.jpg";
	back_path = String2FilePath(tmp_path);
	tmp_path = sky_path_str + "\\front.jpg";
	front_path = String2FilePath(tmp_path);
}


int main()
{	
	
	cout << "Please choose the skybox file directory: \n";
	char* skybox_path[6];
	LoadSkyboxPath(skybox_path[0], skybox_path[1], skybox_path[2], skybox_path[3], skybox_path[4], skybox_path[5]);

	//part 1
	//Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	//Initialize Window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Scene Editor V0.1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Fail to create" << std::endl;
		glfwTerminate();
		return -1;
	}//create window
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))//
	{
		std::cout << "fail to initialize glad" << std::endl;
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, frame_buffer_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//part 1 end

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glViewport(0, 0, WIDTH, HEIGHT);
	//stbi_set_flip_vertically_on_load(true);这个先不能用，因为包围盒在外部
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Shader ourshader("../assets/shader/1.model_loading.vs", "../assets/shader/1.model_loading.fs");
	Shader ourshader("../assets/shader/3.3.shader.vs", "../assets/shader/3.3.shader.fs");
	Shader skyboxshader("../assets/shader/3.3.skyboxshader.vs", "../assets/shader/3.3.skyboxshader.fs");
	//Shader highshader("../assets/shader/3.3.highshade.vs", "../assets/shader/3.3.highshade.fs");
	Shader highshader("../assets/shader/4.highshade.vs", "../assets/shader/4.highshade.fs");

	//地形图处理
	ourTerrain.putinVN(HighMap);
	ourTerrain.putinPointNormal(HighMap);
	float* highVertices = ourTerrain.mergeVN();

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	//Setup 地形图
	GLuint highVAO, highVBO;
	glGenVertexArrays(1, &highVAO);
	glGenBuffers(1, &highVBO);
	glBindVertexArray(highVAO);
	glBindBuffer(GL_ARRAY_BUFFER, highVBO);
	glBufferData(GL_ARRAY_BUFFER, 5 * ((ourTerrain.vertics).size()) * sizeof(float), highVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	// Setup skybox VAO
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Cubemap (Skybox)
	vector<const GLchar*> faces;
	/**/
	for (int i = 0; i < 6; i++) {
		faces.push_back(skybox_path[i]);
	}
	GLuint cubemapTexture = loadCubemap(faces);

	string groundTexturePath1 = "C:/Users/15474/Desktop/计图代码/Simple_Scene_Editor-master/assets/texture/1.jpg";
	string groundTexturePath2 = "C:/Users/15474/Desktop/计图代码/Simple_Scene_Editor-master/assets/texture/3.jpg";
	GLuint groundTexture1 = loadTexture((GLchar*)groundTexturePath1.c_str());
	GLuint groundTexture2 = loadTexture((GLchar*)groundTexturePath2.c_str());


	//model
	modelsRender.addShader(&ourshader);
	
	int i = 0, j = 0, k = 0;
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		Do_Movement();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		// Draw skybox 
		glDepthMask(GL_FALSE);// Remember to turn depth writing off
		skyboxshader.use();
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(skyboxshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(skyboxshader.ID, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);

		//draw highmap
		/**/
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		highshader.use();
		view = camera.GetViewMatrix();  //glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 model;
		//projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		//phong光照明
		glUniformMatrix4fv(glGetUniformLocation(highshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(highshader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(highshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// highmap
		glBindVertexArray(highVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, groundTexture1);
		glUniform1i(glGetUniformLocation(highshader.ID, "u_Texture0"), 0);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, groundTexture2);
		glUniform1i(glGetUniformLocation(highshader.ID, "u_Texture1"), 1);

		glDrawArrays(GL_TRIANGLES, 0, (ourTerrain.vertics).size());
		glBindVertexArray(0);


		
		modelsRender.render(camera);
		
	
		//modelsRender.getModelFromUser();
		// Swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//end
	glfwTerminate();
	return 0;

}

GLuint loadTexture(GLchar* path)//这个是搞普通贴图的
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height, nrChannels;
	unsigned char* image = stbi_load(path, &width, &height, &nrChannels, 0);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);
	return textureID;
}

// Loads a cubemap texture from 6 individual texture faces
// Order should be:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
GLuint loadCubemap(vector<const GLchar*> faces)//这个是搞立方体贴图的
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrChannels;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = stbi_load(faces[i], &width, &height, &nrChannels, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods_Bit) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		
		mouse_button_left_pressed = 1;
		//鼠标左键按下，开始检测交点并放置模型
		//string path;
		glm::vec3 be = camera.Position;
		glm::vec3 dir = camera.Front;
		Ray ray1(be, dir);
		glm::vec3 hit;
		bool ishit = ourTerrain.Intersects(HighMap, ray1, hit);
		if (ishit)
		{
			cout << "[" << hit.x << "," << hit.y << "," << hit.z << "]" << endl;
			cout << "Please choose the model's obj file: \n";
			char* model_path = GetSelectFilePath();
			if ((string)model_path == "")
			{
				return;
			}
			
			Model ourModel1((GLchar*)model_path);
			model1.model = ourModel1;
			model1.translate = hit;//+glm::vec3(0,0.5,0)
			cout << "Please choose the scale of model ( x_scale , y_scale , z_scale ): \n";
			float x_scale, y_scale, z_scale;
			cin>> x_scale >> y_scale >> z_scale;
			glm::vec3 scale_input(x_scale, y_scale, z_scale);
			model1.scale = scale_input;
			modelsRender.addModel(model1);

		}
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		mouse_button_left_pressed = 0;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		mouse_button_right_pressed = 1;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		mouse_button_right_pressed = 0;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		mouse_button_middle_pressed = 1;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
		mouse_button_middle_pressed = 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	if (mouse_button_left_pressed == 1) {
		//camera.ProcessMouseDragLeft(xoffset, yoffset);
	}
	if (mouse_button_right_pressed == 1) {
		camera.ProcessMouseDragRight(xoffset, yoffset);
	}
	if (mouse_button_middle_pressed == 1) {
		camera.ProcessMouseDragMiddle(xoffset, yoffset);
	}
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


