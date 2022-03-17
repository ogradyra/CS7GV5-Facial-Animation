#pragma warning(disable : 5208)
#define NOMINMAX

#include <limits>
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> 

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_glut.h>
#include <imgui_impl_opengl3.h>

// Assimp includes
#include <assimp/cimport.h> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h> 

// Project includes
#include "maths_funcs.h"
#include "mesh_loader.h"

// GLM includes
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/euler_angles.hpp>
#include "glm/gtx/string_cast.hpp"

#include <Eigen/Dense>
#include <Eigen/Sparse>


#pragma region MESH LOADING
/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
MeshLoader neutral("U:/animation_proj/Project1/Project1/models/neutral.obj");
//MeshLoader jaw_open("U:/animation_proj/Project1/Project1/models/Mery_jaw_open.obj");
//MeshLoader kiss("U:/animation_proj/Project1/Project1/models/Mery_kiss.obj");
//MeshLoader r_smile("U:/animation_proj/Project1/Project1/models/Mery_r_smile.obj");
//MeshLoader r_suck("U:/animation_proj/Project1/Project1/models/Mery_r_suck.obj");
//MeshLoader r_sad("U:/animation_proj/Project1/Project1/models/Mery_r_sad.obj");
//MeshLoader r_puff("U:/animation_proj/Project1/Project1/models/Mery_r_puff.obj");
//MeshLoader r_nose_wrinkle("U:/animation_proj/Project1/Project1/models/Mery_r_nose_wrinkle.obj");
//MeshLoader r_eye_upper_open("U:/animation_proj/Project1/Project1/models/Mery_r_eye_upper_open.obj");
//MeshLoader r_eye_lower_open("U:/animation_proj/Project1/Project1/models/Mery_r_lower_open.obj");
//MeshLoader r_eye_closed("U:/animation_proj/Project1/Project1/models/Mery_r_eye_closed.obj");
//MeshLoader r_brow_raise("U:/animation_proj/Project1/Project1/models/Mery_r_brow_raise.obj");
//MeshLoader r_brow_narrow("U:/animation_proj/Project1/Project1/models/Mery_r_brow_narrow.obj");
//MeshLoader r_brow_lower("U:/animation_proj/Project1/Project1/models/Mery_r_brow_lower.obj");
//MeshLoader l_smile("U:/animation_proj/Project1/Project1/models/Mery_l_smile.obj");
//MeshLoader l_suck("U:/animation_proj/Project1/Project1/models/Mery_l_suck.obj");
//MeshLoader l_sad("U:/animation_proj/Project1/Project1/models/Mery_l_sad.obj");
//MeshLoader l_puff("U:/animation_proj/Project1/Project1/models/Mery_l_puff.obj");
//MeshLoader l_nose_wrinkle("U:/animation_proj/Project1/Project1/models/Mery_l_nose_wrinkle.obj");
//MeshLoader l_eye_upper_open("U:/animation_proj/Project1/Project1/models/Mery_l_eye_upper_open.obj");
//MeshLoader l_eye_lower_open("U:/animation_proj/Project1/Project1/models/Mery_l_lower_open.obj");
//MeshLoader l_eye_closed("U:/animation_proj/Project1/Project1/models/Mery_l_eye_closed.obj");
//MeshLoader l_brow_raise("U:/animation_proj/Project1/Project1/models/Mery_l_brow_raise.obj");
//MeshLoader l_brow_narrow("U:/animation_proj/Project1/Project1/models/Mery_l_brow_narrow.obj");
//MeshLoader l_brow_lower("U:/animation_proj/Project1/Project1/models/Mery_l_brow_lower.obj");
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/

const char* expressions[25] = {
	"jaw_open",
	"kiss",
	"l_brow_lower",
	"l_brow_narrow",
	"l_brow_raise",
	"l_eye_closed",
	"l_eye_lower_open",
	"l_eye_upper_open",
	"l_nose_wrinkle",
	"l_puff",
	"l_sad",
	"l_smile",
	"l_suck",
	"r_brow_lower",
	"r_brow_narrow",
	"r_brow_raise",
	"r_eye_closed",
	"r_eye_lower_open",
	"r_eye_upper_open",
	"r_nose_wrinkle",
	"r_puff",
	"r_sad",
	"r_smile",
	"r_suck"
};

#pragma endregion MESH LOADING

using namespace std;
GLuint shaderProgramID;

int width = 1400;
int height = 800;

GLuint loc1, loc2, loc3;

// camera stuff
glm::vec3 cameraPos = glm::vec3(0.0f, 16.0f, 38.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

int projType = 0;
float fov = 45.0f;

unsigned int neutral_vbo1, neutral_vbo2, neutral_vao;

int k = 24;
Eigen::VectorXf f0 = Eigen::VectorXf::Zero(neutral.numVertices * 3, 1); // F0
Eigen::MatrixXf B = Eigen::MatrixXf::Zero(neutral.numVertices * 3, k); // B
Eigen::VectorXf w = Eigen::VectorXf::Zero(k, 1); // w
Eigen::VectorXf f = Eigen::VectorXf::Zero(neutral.numVertices * 3, 1); // F

std::vector<MeshLoader> dataArray;

Eigen::VectorXf m, m0;
std::vector<int> constraints;
float alpha = 2;
float mu = 0.0001;

bool animate = false;

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS
char* readShaderSource(const char* shaderFile) {
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		std::cerr << "Error creating shader..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024] = { '\0' };
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		std::cerr << "Error compiling "
			<< (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader program: " << InfoLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		std::cerr << "Error creating shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, "U:/animation_proj/Project1/Project1/simpleVertexShader.txt", GL_VERTEX_SHADER);
	AddShader(shaderProgramID, "U:/animation_proj/Project1/Project1/simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { '\0' };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
void generateObjectBufferMesh(Eigen::VectorXf face) {

	for (int i = 0; i < neutral.meshVertices.size(); i++) {
		neutral.meshVertices[i] = face(i);
	}

	// Model
	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");

	glGenBuffers(1, &neutral_vbo1);
	glBindBuffer(GL_ARRAY_BUFFER, neutral_vbo1);
	glBufferData(GL_ARRAY_BUFFER, 3 * neutral.numVertices * sizeof(float), &neutral.meshVertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &neutral_vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, neutral_vbo2);
	glBufferData(GL_ARRAY_BUFFER, 3 * neutral.numVertices * sizeof(float), &neutral.meshNormals[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &neutral_vao);
	glBindVertexArray(neutral_vao);

	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, neutral_vbo1);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, neutral_vbo2);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

}
#pragma endregion VBO_FUNCTIONS

#pragma region BLENDSHAPE_FUNCTIONS
void blendshape_array() {

	/*dataArray.push_back(jaw_open);
	dataArray.push_back(kiss);
	dataArray.push_back(l_brow_lower);
	dataArray.push_back(l_brow_narrow);
	dataArray.push_back(l_brow_raise);
	dataArray.push_back(l_eye_closed);
	dataArray.push_back(l_eye_lower_open);
	dataArray.push_back(l_eye_upper_open);
	dataArray.push_back(l_nose_wrinkle);
	dataArray.push_back(l_puff);
	dataArray.push_back(l_sad);
	dataArray.push_back(l_smile);
	dataArray.push_back(l_suck);
	dataArray.push_back(r_brow_lower);
	dataArray.push_back(r_brow_narrow);
	dataArray.push_back(r_brow_raise);
	dataArray.push_back(r_eye_closed);
	dataArray.push_back(r_eye_lower_open);
	dataArray.push_back(r_eye_upper_open);
	dataArray.push_back(r_nose_wrinkle);
	dataArray.push_back(r_puff);
	dataArray.push_back(r_sad);
	dataArray.push_back(r_smile);
	dataArray.push_back(r_suck);*/
}

void createF0Matrix() {
	for (int i = 0; i < neutral.meshVertices.size(); i++) {
		f0(i) = neutral.meshVertices[i];
	}
}

void createBMatrix() {

	for (int n = 0; n < dataArray.size(); n++) {

		for (int i = 0; i < dataArray[n].meshVertices.size(); i++) {
			B(i, n) = dataArray[n].meshVertices[i] - f0[i];
		}

	}
}
#pragma endregion BLENDSHAPE_FUNCTIONS

#pragma region VERTEX_MANIPULATORS

GLuint v_index = 0; /* index of closest vertex */

glm::vec3 vertex_picker(int x, int y, glm::mat4 VM, glm::mat4 P) {

	glm::vec3 window;
	window.x = x;
	window.y = height - y - 1;
	glReadPixels(x, height - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &window.z);

	/* get object coordinates */
	glm::vec3 object = glm::unProject(window, VM, P, glm::vec4(0.0f, 0.0f, width, height));
	std::cout << "object coordinates are " << glm::to_string(object) << endl;

	/* find nearest vertex */
	GLfloat dist = 10;
	GLfloat temp = 0.0f;

	for (int i = 0; i < neutral.meshVertices.size(); i += 3) {

		glm::vec3 pos(neutral.meshVertices[i], neutral.meshVertices[i + 1], neutral.meshVertices[i + 2]);
		temp = glm::distance(object, pos);

			if (temp <= dist) {
				dist = temp;
				v_index = i;
			}
	}

	glm::vec3 vertex(neutral.meshVertices[v_index], neutral.meshVertices[v_index + 1], neutral.meshVertices[v_index + 2]);

	constraints.push_back(v_index); //add index of constrained vertex to list of constraints
	m0.conservativeResize(constraints.size() * 3);
	m0(3 * constraints.size() - 3) = vertex.x;
	m0(3 * constraints.size() - 2) = vertex.y;
	m0(3 * constraints.size() - 1) = vertex.z;
	return vertex;	
}

void get_mouse_loc(int x, int y, glm::mat4 VM, glm::mat4 P) {
	glm::vec3 window;
	window.x = x;
	window.y = height - y - 1;
	glReadPixels(x, height - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &window.z);

	glm::vec3 targetVerticePos = glm::unProject(window, VM, P, glm::vec4(0.0f, 0.0f, width, height));

	//constraints.push_back(v_index); //add index of constrained vertex to list of constraints
	m.conservativeResize(constraints.size() * 3);
	m(3 * constraints.size() - 3) = targetVerticePos.x;
	m(3 * constraints.size() - 2) = targetVerticePos.y;
	m(3 * constraints.size() - 1) = targetVerticePos.z;

	cout << "m0" << m0 << endl;
	cout << "m" << m << endl;

}

Eigen::VectorXf my_direct_manip_method() {

	// equation to solve for w: (transpose(B2)*B2 + (alpha + mu)I)w = transpose(B2)(m - m0) + alpha(w-1)
	// in the form Aw = b

	// setup left-hand side
	Eigen::MatrixXf B2(constraints.size() * 3, k);

	for (int i = 0; i < constraints.size(); i++) {
		for (int j = 0; j < dataArray.size(); j++) {
			B2(3 * i, j - 1) = B(3 * constraints[i], j - 1);
			B2(3 * i + 1, j - 1) = B(3 * constraints[i] + 1, j - 1);
			B2(3 * i + 2, j - 1) = B(3 * constraints[i] + 2, j - 1);
		}
	}

	Eigen::MatrixXf A = B2.transpose() * B2 + (alpha + mu) * Eigen::MatrixXf::Identity(k, k);

	//setup RHS
	Eigen::VectorXf b(k);
	b = B2.transpose() * (m - m0) + (alpha * w);

	// solve least squares problem Aw = b
	Eigen::LDLT<Eigen::MatrixXf> solver(A);
	Eigen::VectorXf w_new = solver.solve(b);
	return w_new;

}


#pragma endregion VETREX_MANIPULATORS

glm::mat4 persp_proj;
glm::mat4 view;

void display() {

	// --------------------------------- UI --------------------------------------

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGLUT_NewFrame();

	ImGui::Begin("Facial Weights Controller");                   
	
	for (int i = 0; i < dataArray.size(); i++) {

		ImGui::SliderFloat(expressions[i], &w[i], 0.0f, 1.0f);

	}

	if (ImGui::Button("Play Animation"))
		animate = true;

	ImGui::End();

	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);

	// tell GL to only draw onto a pixel if the shape is closer to the viewe
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glUseProgram(shaderProgramID);

	// --------------------------------- CAMERA --------------------------------------

	//setting up projection matrix
	persp_proj = glm::perspective(glm::radians(fov), (float)width / (float)height, 1.0f, 100.0f);
	if (projType == 0) {
		persp_proj = glm::perspective(45.0f, (float)width / (float)height, 1.0f, 100.0f);
	}

	else if (projType == 1) {
		persp_proj = glm::ortho(-16.0f, 16.0f, -12.0f, 12.0f, 1.0f, 100.0f);
	}

	//setting up camera
	//lookAt(position, target, up vector);
	view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	// --------------------------------- FACE --------------------------------------

	glBindVertexArray(neutral_vao);
	glm::mat4 model = glm::mat4(1.0f);

	// update uniforms & draw
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "proj"), 1, GL_FALSE, glm::value_ptr(persp_proj));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, neutral.numVertices);

	glutSwapBuffers();
}

Eigen::VectorXf weight = Eigen::VectorXf::Zero(neutral.numVertices * 3, 1);

void readTextFile() {

	fstream newfile;
	newfile.open("U:/animation_proj/Project1/Project1/animation.txt", ios::in); //open a file to perform read operation using file object

	if (newfile.is_open()) { //checking whether the file is open
		string val;
		int pbs = 0;

		while (getline(newfile, val, ' ')) { //read data from file object and put it into string.

			weight(pbs) = std::stof(val);
			pbs++;

		}

		newfile.close(); //close the file object.

	}

}

int frame_num = 0;
int j = 0;

void updateScene() {

	if (animate) {

		for (int i = 0; i < dataArray.size(); i++) {

			w[i] = weight(j);
			j++;
		}

		frame_num++;

		if (frame_num == 250) {

			animate = false;
			frame_num = 0;
			j = 0;
		}

	}



	f = f0 + (B * w);
	generateObjectBufferMesh(f);

	glutPostRedisplay();
}


void init()
{

	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// load mesh into a vertex buffer array
	blendshape_array();
	createF0Matrix();
	createBMatrix();
	generateObjectBufferMesh(f0);
	readTextFile();

}

void mouse_click(int button, int state, int x, int y) {

	if (button == 0 && state == GLUT_DOWN) {


		std::cout << "MOUSE DOWN X: " << x << std::endl;
		std::cout << "MOUSE DOWN Y: " << y << std::endl;

		//glm::vec3 mouse_pos = vertex_picker(x, y, view, persp_proj);
	}

	if (button == 0 && state == GLUT_UP) {

		std::cout << "MOUSE UP X: " << x << std::endl;
		std::cout << "MOUSE UP Y: " << y << std::endl;

		//get_mouse_loc(x, y, view, persp_proj);
		//w = my_direct_manip_method();
		
	}

}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowSize(width, height);
	glutCreateWindow("Hello Triangle");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	//glutSpecialFunc(specialKeys);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	//Setup IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplGLUT_Init();
	ImGui_ImplGLUT_InstallFuncs(); // use the imgui glut funcs
	glutMouseFunc(mouse_click);
	ImGui_ImplOpenGL3_Init();

	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();

	//Shutdown
	ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();

	return 0;
}
