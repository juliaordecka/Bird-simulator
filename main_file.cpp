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
#include <vector>

float speed = 0;
//zmienic te sciezki zeby nie bylo takiego odniesienia
Models::ObjModel jablko("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/jablko2.obj");
Models::ObjModel trawa("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/trawa.obj");
Models::ObjModel drzewo("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/drzewo.obj");
Models::ObjModel drzewo2("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/drzewo.obj");
Models::ObjModel ptaszor("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/ptaszor.obj");
GLuint tex0;
ShaderProgram* sp;

// Zmienne kamery
float cameraDistance = 10.0f;
float cameraHeight = 3.0f;
float cameraAngleH = 0.0f;  // Kąt poziomy (yaw)
float cameraAngleV = 0.0f;  // Kąt pionowy (pitch)

// Pozycja ptaka
float birdX = 0.0f;
float birdY = 2.0f;
float birdZ = 0.0f;
float birdSpeed = 5.0f;

// Zmienne dla myszy
bool firstMouse = true;
bool mousePressed = false;  // Czy przycisk myszy jest wciśnięty
float lastX = 512.0f;
float lastY = 512.0f;
float mouseSensitivity = 0.005f;

void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

// Callback dla przycisków myszy
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
            firstMouse = true; // Reset pozycji przy nowym kliknięciu
        }
        else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }
    }
}

// Callback dla ruchu myszy
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!mousePressed) return; // Obracaj tylko gdy przycisk wciśnięty

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    cameraAngleH += xoffset;
    cameraAngleV += yoffset;

    // Ograniczenia kąta pionowego
    if (cameraAngleV > 1.5f) cameraAngleV = 1.5f;
    if (cameraAngleV < -1.5f) cameraAngleV = -1.5f;
}

// Callback dla scrolla myszy (zoom)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cameraDistance -= yoffset * 2.0f;
    if (cameraDistance < 2.0f) cameraDistance = 2.0f;
    if (cameraDistance > 30.0f) cameraDistance = 30.0f;
}

// Key callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) speed = -PI;
        if (key == GLFW_KEY_RIGHT) speed = PI;
    }

    if (action == GLFW_RELEASE) {
        speed = 0;
    }
}

// Funkcja do sterowania ptakiem
void updateBird(GLFWwindow* window, float deltaTime) {
    // Sterowanie ptakiem WASD
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        birdZ += birdSpeed * deltaTime; // Do przodu
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        birdZ -= birdSpeed * deltaTime; // Do tyłu
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        birdX -= birdSpeed * deltaTime; // W lewo
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        birdX += birdSpeed * deltaTime; // W prawo
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        birdY += birdSpeed * deltaTime; // W górę
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        birdY -= birdSpeed * deltaTime; // W dół
    }

    // Ograniczenia dla ptaka
    if (birdY < 0.5f) birdY = 0.5f;
    if (birdY > 15.0f) birdY = 15.0f;
}

GLuint readTexture(const char* filename) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    //Wczytanie do pamięci komputera
    std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
    unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
    //Wczytaj obrazek
    unsigned error = lodepng::decode(image, width, height, filename);

    //Import do pamięci karty graficznej
    glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
    glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}

void initOpenGLProgram(GLFWwindow* window) {
    initShaders();
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); // Wyłącz culling globalnie - obiekty widoczne z obu stron

    // Callback'i
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
    tex0 = readTexture("metal.png");
}


void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
    delete sp;
    glDeleteTextures(1, &tex0);
}

void drawScene(GLFWwindow* window, float angle) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 100.0f);

    glm::vec3 cameraPos = glm::vec3(
        sin(cameraAngleH) * cos(cameraAngleV) * cameraDistance,
        sin(cameraAngleV) * cameraDistance + cameraHeight,
        cos(cameraAngleH) * cos(cameraAngleV) * cameraDistance
    );

    glm::mat4 V = glm::lookAt(
        cameraPos,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // ====== TRAWA - POKRYCIE CAŁEJ PRZESTRZENI ======
    spConstant->use();
    glUniformMatrix4fv(spConstant->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spConstant->u("V"), 1, false, glm::value_ptr(V));

    // Wyłącz culling dla trawy żeby była widoczna z obu stron
    glDisable(GL_CULL_FACE);

    // Rysuj trawę w kilku kopiach żeby pokryć całą przestrzeń
    for (int x = -2; x <= 2; x++) {
        for (int z = -2; z <= 2; z++) {
            glm::mat4 groundM = glm::mat4(1.0f);
            groundM = glm::translate(groundM, glm::vec3(x * 20.0f, -5.2f, z * 20.0f));
            groundM = glm::scale(groundM, glm::vec3(15.0f, 1.0f, 15.0f));

            glUniformMatrix4fv(spConstant->u("M"), 1, false, glm::value_ptr(groundM));
            glUniform4f(spConstant->u("color"), 0.3f, 0.7f, 0.2f, 1.0f);
            trawa.drawSolid(true);
        }
    }

    // Włącz z powrotem culling dla pozostałych obiektów
    glEnable(GL_CULL_FACE);

    // ====== JABŁKO ======
    sp->use();
    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

    glm::mat4 appleM = glm::mat4(1.0f);
    appleM = glm::translate(appleM, glm::vec3(0.0f, -4.0f, 0.0f));
    appleM = glm::rotate(appleM, angle * 0.3f, glm::vec3(0, 1, 0));
    appleM = glm::scale(appleM, glm::vec3(0.8f, 0.8f, 0.8f));

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(appleM));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(sp->u("textureMap0"), 0);
    glUniform1i(sp->u("textureMap1"), 0);

    jablko.drawSolid(true);

    // ====== DRZEWA ======
    spLambert->use();
    glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));

    glBindTexture(GL_TEXTURE_2D, 0);

    // Drzewo 1
    glm::mat4 treeM = glm::mat4(1.0f);
    treeM = glm::translate(treeM, glm::vec3(-3.0f, -3.0f, -1.0f));
    treeM = glm::rotate(treeM, angle * 0.1f, glm::vec3(0, 1, 0));
    treeM = glm::scale(treeM, glm::vec3(0.8f, 0.8f, 0.8f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(treeM));
    glUniform4f(spLambert->u("color"), 0.15f, 0.5f, 0.15f, 1.0f);
    drzewo.drawSolid(true);

    // Drzewo 2
    glm::mat4 tree2M = glm::mat4(1.0f);
    tree2M = glm::translate(tree2M, glm::vec3(3.5f, -3.5f, -2.0f));
    tree2M = glm::rotate(tree2M, -angle * 0.1f, glm::vec3(0, 1, 0));
    tree2M = glm::scale(tree2M, glm::vec3(0.9f, 0.9f, 0.9f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(tree2M));
    glUniform4f(spLambert->u("color"), 0.25f, 0.6f, 0.2f, 1.0f);
    drzewo2.drawSolid(true);

    // ====== PTAK ======
    glm::mat4 ptakM = glm::mat4(1.0f);
    ptakM = glm::translate(ptakM, glm::vec3(birdX, birdY, birdZ));
    ptakM = glm::scale(ptakM, glm::vec3(0.4f, 0.4f, 0.4f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(ptakM));
    glUniform4f(spLambert->u("color"), 0.3f, 0.4f, 0.9f, 1.0f);
    ptaszor.drawSolid(true);

    glfwSwapBuffers(window);
}


int main(void) {
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Nie można zainicjować GLFW.\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(1024, 1024, "OpenGL", NULL, NULL);

    if (!window) {
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
    float lastTime = 0;

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        angle += speed * deltaTime;

        updateBird(window, deltaTime); // Aktualizuj pozycję ptaka
        drawScene(window, angle);
        glfwPollEvents();
    }

    freeOpenGLProgram(window);
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}