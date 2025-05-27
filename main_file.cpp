#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <vector>

float speed = 0;
//zmienic te sciezki
Models::ObjModel jablko("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/jablko2.obj");
Models::ObjModel trawa("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/trawa.obj");
Models::ObjModel drzewo("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/drzewo.obj");
Models::ObjModel drzewo2("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/drzewo.obj");
Models::ObjModel ptaszor("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/ptaszor.obj");
//Models::Torus torus;


void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed=-PI; 
		if (key == GLFW_KEY_RIGHT) speed = PI; 
	}

	if (action == GLFW_RELEASE) {
		speed = 0; 
	}

}

void initOpenGLProgram(GLFWwindow* window) {
    initShaders();	
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, key_callback);
}

void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
}

void drawScene(GLFWwindow* window, float angle) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 0.1f, 50.0f);

	glm::mat4 V = glm::lookAt(
		glm::vec3(0.0f, 5.0f, -10.0f),  
		glm::vec3(0.0f, 0.0f, 0.0f),    
		glm::vec3(0.0f, 1.0f, 0.0f)     
	);
    spLambert->use();
    glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));

    glBindVertexArray(0);

	glm::mat4 groundM = glm::mat4(1.0f);
	groundM = glm::translate(groundM, glm::vec3(0.0f, -5.5f, 0.0f));
	groundM = glm::scale(groundM, glm::vec3(10.0f, 0.5f, 10.0f));
	groundM = glm::rotate(groundM, glm::radians(-90.0f), glm::vec3(1, 0, 0));

	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(groundM));
	glUniform4f(spLambert->u("color"), 0.1f, 0.8f, 0.1f, 1.0f); 

	trawa.drawSolid(true);

    glm::mat4 appleM = glm::mat4(1.0f);
    appleM = glm::translate(appleM, glm::vec3(0.0f, 0.5f, 0.0f));
    appleM = glm::rotate(appleM, angle, glm::vec3(0, 1, 0));
    appleM = glm::scale(appleM, glm::vec3(0.5f, 0.5f, 0.5f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(appleM));
    glUniform4f(spLambert->u("color"), 0.8f, 0.1f, 0.1f, 1.0f);  
    jablko.drawSolid(true);


	glm::mat4 treeM = glm::mat4(1.0f);
	treeM = glm::translate(treeM, glm::vec3(2.0f, -2.0f, -0.8f));
	treeM = glm::rotate(treeM, angle, glm::vec3(0, 1, 0));
	treeM = glm::scale(treeM, glm::vec3(0.5f, 0.5f, 0.5f));

	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(treeM));
	glUniform4f(spLambert->u("color"), 0.0f, 0.3f, 0.0f, 1.0f);
	drzewo.drawSolid(true);

	glm::mat4 tree2M = glm::mat4(1.0f);
	tree2M = glm::translate(tree2M, glm::vec3(-2.0f, -4.0f, -0.8f));
	tree2M = glm::rotate(tree2M, angle, glm::vec3(0, 1, 0));
	tree2M = glm::scale(tree2M, glm::vec3(0.5f, 0.5f, 0.5f));

	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(tree2M));
	glUniform4f(spLambert->u("color"), 0.0f, 0.3f, 0.0f, 1.0f);
	drzewo2.drawSolid(true);

	glm::mat4 ptakM = glm::mat4(1.0f);
	ptakM = glm::translate(ptakM, glm::vec3(-5.0f, -4.0f, -0.8f));
	ptakM = glm::rotate(ptakM, angle, glm::vec3(0, 1, 0));
	ptakM = glm::scale(ptakM, glm::vec3(0.5f, 0.5f, 0.5f));

	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(ptakM));
	glUniform4f(spLambert->u("color"), 0.0f, 0.0f, 0.5f, 1.0f);
	ptaszor.drawSolid(true);



    glfwSwapBuffers(window);
}




int main(void)
{
	GLFWwindow* window; 

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) { 
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1024, 1024, "OpenGL", NULL, NULL); 

	if (!window)
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}


	initOpenGLProgram(window);


	float angle = 0; 
	glfwSetTime(0); 
	while (!glfwWindowShouldClose(window)) 
	{
		angle += speed * glfwGetTime(); 
		glfwSetTime(0); 

		drawScene(window,angle); 
		glfwPollEvents(); 
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); 
	glfwTerminate(); 
	exit(EXIT_SUCCESS);
}
