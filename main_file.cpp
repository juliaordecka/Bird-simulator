#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

//zmienic te sciezki zeby nie bylo takiego odniesienia
Models::ObjModel jablko("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/jablko2.obj");
Models::ObjModel trawa("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/trawa.obj");  // ZMIENIONE NA trawa2.obj
Models::ObjModel drzewo("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/drzewo.obj");
Models::ObjModel drzewo2("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/drzewo.obj");
Models::ObjModel orzel("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/orzel.obj");
Models::ObjModel ptaszor("C:/Users/Julia/source/repos/g3d_st_04_win/g3d_st_04_win/x64/Debug/ptaszor.obj");

GLuint tex0;
GLuint texEagle;
GLuint texTrawa;  // DODANA TEKSTURA TRAWY
ShaderProgram* sp;

// Zmienne kamery
float cameraDistance = 10.0f;
float cameraHeight = 3.0f;
float cameraAngleH = 0.0f;
float cameraAngleV = 0.0f;

// Zmienne dla kamery podążającej
bool followEagle = true;
float cameraSmoothing = 2.0f;
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
float cameraOffsetY = 5.0f;
float cameraOffsetZ = -10.0f;

// Zmienne dla manualnej kontroli kamery w trybie podążającym
float manualCameraAngleH = 0.0f;  // Ręczny obrót poziomy
float manualCameraAngleV = 0.0f;  // Ręczny obrót pionowy

// Zmienne orła z realistyczną fizyką
float eagleX = 0.0f;
float eagleY = 2.0f;
float eagleZ = 0.0f;

// Orientacja orła (Euler angles)
float eagleYaw = 0.0f;      // Obrót w osi Y (lewo/prawo)
float eaglePitch = 0.0f;    // Obrót w osi X (góra/dół)
float eagleRoll = 0.0f;     // Obrót w osi Z (pochylenie w boki)

// Fizyka orła
float eagleSpeed = 8.0f;
float eagleVelocityX = 0.0f;
float eagleVelocityY = 0.0f;
float eagleVelocityZ = 0.0f;

// Zmienne dla ptaka NPC
float npcBirdX = 5.0f;
float npcBirdY = -4.0f;
float npcBirdZ = 5.0f;
float npcBirdRotation = 0.0f;
float npcBirdSpeed = 2.0f;
float npcBirdTimer = 0.0f;

const float My_Pi = 3.14159265f;

// Deklaracje funkcji
void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void updateEagle(GLFWwindow* window, float deltaTime);
void updateNPCBird(float deltaTime);
void updateCamera(float deltaTime);
GLuint readTexture(const char* filename);

// Zmienne myszy
bool firstMouse = true;
double lastX = 512, lastY = 384;
bool mousePressed = false;

void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // Przełączanie między trybami kamery
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        followEagle = !followEagle;
        printf("Tryb kamery: %s\n", followEagle ? "Podążająca" : "Wolna");
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (mousePressed) {
        float sensitivity = 0.005f;

        if (followEagle) {
            // W trybie podążającym - kontroluj kamerę wokół orła
            manualCameraAngleH += xoffset * sensitivity;
            manualCameraAngleV += yoffset * sensitivity;

            // Ogranicz kąt pionowy
            if (manualCameraAngleV > 1.4f) manualCameraAngleV = 1.4f;
            if (manualCameraAngleV < -1.4f) manualCameraAngleV = -1.4f;
        }
        else {
            // W trybie wolnym - kontroluj kamerę wolną
            cameraAngleH += xoffset * sensitivity;
            cameraAngleV += yoffset * sensitivity;

            // Ogranicz kąt pionowy
            if (cameraAngleV > 1.4f) cameraAngleV = 1.4f;
            if (cameraAngleV < -1.4f) cameraAngleV = -1.4f;
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
        }
        else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (followEagle) {
        // W trybie podążającym - zmień odległość od orła
        cameraOffsetZ += yoffset * 2.0f;
        if (cameraOffsetZ > -3.0f) cameraOffsetZ = -3.0f;
        if (cameraOffsetZ < -30.0f) cameraOffsetZ = -30.0f;
    }
    else {
        // W trybie wolnym - zmień odległość kamery
        cameraDistance -= yoffset * 2.0f;
        if (cameraDistance < 2.0f) cameraDistance = 2.0f;
        if (cameraDistance > 50.0f) cameraDistance = 50.0f;
    }
}

void updateCamera(float deltaTime) {
    // Płynne podążanie kamery za orłem
    if (followEagle) {
        glm::vec3 targetPos = glm::vec3(eagleX, eagleY, eagleZ);
        cameraTarget = glm::mix(cameraTarget, targetPos, deltaTime * cameraSmoothing);
    }
}

// Realistyczne sterowanie orłem z fizyką
void updateEagle(GLFWwindow* window, float deltaTime) {
    float turnSpeed = 2.0f;
    float pitchSpeed = 1.5f;

    // Obroty orła (sterowanie kierunkiem)
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        eagleYaw += turnSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        eagleYaw -= turnSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        eaglePitch += pitchSpeed * deltaTime;
        if (eaglePitch > 0.5f) eaglePitch = 0.5f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        eaglePitch -= pitchSpeed * deltaTime;
        if (eaglePitch < -0.5f) eaglePitch = -0.5f;
    }

    // Ruch do przodu i do tyłu
    float moveX = 0.0f, moveY = 0.0f, moveZ = 0.0f;

    // POPRAWIONE STEROWANIE:
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // Lot do przodu
        moveX = -sin(eagleYaw) * cos(eaglePitch) * eagleSpeed * deltaTime;  // ZMIENIONE
        moveY = sin(eaglePitch) * eagleSpeed * deltaTime;
        moveZ = -cos(eagleYaw) * cos(eaglePitch) * eagleSpeed * deltaTime;  // ZMIENIONE
    }

    // DODAJ KLAWISZ S:
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // Lot do tyłu
        moveX = sin(eagleYaw) * cos(eaglePitch) * eagleSpeed * deltaTime;
        moveY = -sin(eaglePitch) * eagleSpeed * deltaTime;
        moveZ = cos(eagleYaw) * cos(eaglePitch) * eagleSpeed * deltaTime;
    }

    // Lot boczny
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        moveX = -cos(eagleYaw) * eagleSpeed * deltaTime;  // ZMIENIONE
        moveZ = sin(eagleYaw) * eagleSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        moveX = cos(eagleYaw) * eagleSpeed * deltaTime;   // ZMIENIONE
        moveZ = -sin(eagleYaw) * eagleSpeed * deltaTime;
    }

    eagleX += moveX;
    eagleY += moveY;
    eagleZ += moveZ;

    // Bezpośrednia kontrola wysokości
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        eagleY += eagleSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        eagleY -= eagleSpeed * deltaTime;
    }

    if (eagleY < 0.5f) eagleY = 0.5f;
    if (eagleY > 15.0f) eagleY = 15.0f;
}


// AI ptaka NPC
void updateNPCBird(float deltaTime) {
    npcBirdTimer += deltaTime;

    if (npcBirdTimer > 3.0f) {
        npcBirdTimer = 0.0f;
        npcBirdRotation = ((float)rand() / RAND_MAX) * 2.0f * My_Pi;
    }

    npcBirdX += sin(npcBirdRotation) * npcBirdSpeed * deltaTime;
    npcBirdZ += cos(npcBirdRotation) * npcBirdSpeed * deltaTime;

    if (npcBirdX > 10.0f || npcBirdX < -10.0f) {
        npcBirdRotation += My_Pi;
        npcBirdTimer = 0.0f;
    }
    if (npcBirdZ > 10.0f || npcBirdZ < -10.0f) {
        npcBirdRotation += My_Pi;
        npcBirdTimer = 0.0f;
    }

    npcBirdY = -4.5f + sin(npcBirdTimer * 4.0f) * 0.1f;
}

GLuint readTexture(const char* filename) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    std::vector<unsigned char> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, filename);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
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
    glDisable(GL_CULL_FACE);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");

    // Ładowanie wszystkich tekstur
    tex0 = readTexture("metal.png");
    texEagle = readTexture("Eagle.png");
    texTrawa = readTexture("trawa.png");  // DODANA TEKSTURA TRAWY

    srand(time(NULL));
}

void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
    delete sp;
    glDeleteTextures(1, &tex0);
    glDeleteTextures(1, &texEagle);
    glDeleteTextures(1, &texTrawa);  // DODAJ TĘ LINIĘ
}

void drawScene(GLFWwindow* window, float angle) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 100.0f);

    glm::vec3 cameraPos;
    glm::vec3 lookTarget;

    if (followEagle) {
        // ===== KAMERA PODĄŻAJĄCA Z MANUALNĄ KONTROLĄ =====

        // Podstawowa pozycja kamery za orłem
        glm::vec3 eagleForward = glm::vec3(sin(eagleYaw), 0.0f, cos(eagleYaw));
        glm::vec3 baseCameraPos = cameraTarget +
            eagleForward * cameraOffsetZ +
            glm::vec3(0, cameraOffsetY, 0);

        // Dodaj manualny obrót wokół orła
        float totalAngleH = manualCameraAngleH;
        float totalAngleV = manualCameraAngleV;

        // Oblicz pozycję kamery z manualnym obrotem
        glm::vec3 offset = glm::vec3(
            sin(totalAngleH) * cos(totalAngleV) * abs(cameraOffsetZ),
            sin(totalAngleV) * abs(cameraOffsetZ) + cameraOffsetY,
            cos(totalAngleH) * cos(totalAngleV) * abs(cameraOffsetZ)
        );

        cameraPos = cameraTarget + offset;
        lookTarget = cameraTarget; // Zawsze patrzy na orła

    }
    else {
        // ===== KAMERA WOLNA =====

        cameraPos = glm::vec3(
            sin(cameraAngleH) * cos(cameraAngleV) * cameraDistance,
            sin(cameraAngleV) * cameraDistance + cameraHeight,
            cos(cameraAngleH) * cos(cameraAngleV) * cameraDistance
        );

        lookTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    glm::mat4 V = glm::lookAt(cameraPos, lookTarget, glm::vec3(0.0f, 1.0f, 0.0f));

    // ====== TRAWA Z SHADEREM SP ======
    sp->use();
    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

    glDisable(GL_CULL_FACE);

    // Aktywuj teksturę PRZED pętlą
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texTrawa);
    glUniform1i(sp->u("textureMap0"), 0);
    glUniform1i(sp->u("textureMap1"), 0);

    for (int x = -8; x <= 8; x++) {
        for (int z = -8; z <= 8; z++) {
            glm::mat4 groundM = glm::mat4(1.0f);
            groundM = glm::translate(groundM, glm::vec3(x * 12.0f, -5.2f, z * 12.0f));
            groundM = glm::scale(groundM, glm::vec3(18.0f, 1.0f, 18.0f));

            glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(groundM));

            trawa.drawSolid(true);
        }
    }

    glEnable(GL_CULL_FACE);


    // ====== JABŁKO ======
    sp->use();
    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

    glm::mat4 appleM = glm::mat4(1.0f);
    appleM = glm::translate(appleM, glm::vec3(0.0f, -4.0f, 0.0f));
    appleM = glm::scale(appleM, glm::vec3(0.8f, 0.8f, 0.8f));

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(appleM));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(sp->u("textureMap0"), 0);
    glUniform1i(sp->u("textureMap1"), 0);

    jablko.drawSolid(true);

    // ====== DRZEWA (POPRAWIONA WYSOKOŚĆ) ======
    spLambert->use();
    glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));

    glBindTexture(GL_TEXTURE_2D, 0);

    // Drzewo 1 - oryginalne (POPRAWIONE)
    glm::mat4 treeM = glm::mat4(1.0f);
    treeM = glm::translate(treeM, glm::vec3(-3.0f, -5.0f, -1.0f));
    treeM = glm::scale(treeM, glm::vec3(0.8f, 0.8f, 0.8f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(treeM));
    glUniform4f(spLambert->u("color"), 0.15f, 0.5f, 0.15f, 1.0f);
    drzewo.drawSolid(true);

    // Drzewo 2 - oryginalne (POPRAWIONE)
    glm::mat4 tree2M = glm::mat4(1.0f);
    tree2M = glm::translate(tree2M, glm::vec3(3.5f, -5.0f, -2.0f));
    tree2M = glm::scale(tree2M, glm::vec3(0.9f, 0.9f, 0.9f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(tree2M));
    glUniform4f(spLambert->u("color"), 0.25f, 0.6f, 0.2f, 1.0f);
    drzewo2.drawSolid(true);

    // ====== NOWE DRZEWA - LAS (POPRAWIONA WYSOKOŚĆ) ======

    // Drzewo 3 - z lewej strony
    glm::mat4 tree3M = glm::mat4(1.0f);
    tree3M = glm::translate(tree3M, glm::vec3(-8.0f, -5.0f, 5.0f));
    tree3M = glm::rotate(tree3M, glm::radians(45.0f), glm::vec3(0, 1, 0));
    tree3M = glm::scale(tree3M, glm::vec3(1.1f, 1.1f, 1.1f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(tree3M));
    glUniform4f(spLambert->u("color"), 0.12f, 0.45f, 0.12f, 1.0f);
    drzewo.drawSolid(true);

    // Drzewo 4 - z prawej strony
    glm::mat4 tree4M = glm::mat4(1.0f);
    tree4M = glm::translate(tree4M, glm::vec3(7.0f, -5.0f, 3.0f));
    tree4M = glm::rotate(tree4M, glm::radians(-30.0f), glm::vec3(0, 1, 0));
    tree4M = glm::scale(tree4M, glm::vec3(0.7f, 0.7f, 0.7f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(tree4M));
    glUniform4f(spLambert->u("color"), 0.3f, 0.65f, 0.25f, 1.0f);
    drzewo2.drawSolid(true);

    // Drzewo 5 - w tle
    glm::mat4 tree5M = glm::mat4(1.0f);
    tree5M = glm::translate(tree5M, glm::vec3(-1.0f, -5.0f, 8.0f));
    tree5M = glm::rotate(tree5M, glm::radians(120.0f), glm::vec3(0, 1, 0));
    tree5M = glm::scale(tree5M, glm::vec3(1.2f, 1.2f, 1.2f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(tree5M));
    glUniform4f(spLambert->u("color"), 0.18f, 0.55f, 0.18f, 1.0f);
    drzewo.drawSolid(true);

    // Drzewo 6 - w tle z prawej
    glm::mat4 tree6M = glm::mat4(1.0f);
    tree6M = glm::translate(tree6M, glm::vec3(5.0f, -5.0f, 9.0f));
    tree6M = glm::rotate(tree6M, glm::radians(-60.0f), glm::vec3(0, 1, 0));
    tree6M = glm::scale(tree6M, glm::vec3(0.9f, 0.9f, 0.9f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(tree6M));
    glUniform4f(spLambert->u("color"), 0.22f, 0.58f, 0.22f, 1.0f);
    drzewo2.drawSolid(true);

    // Drzewo 7 - z lewej strony daleko
    glm::mat4 tree7M = glm::mat4(1.0f);
    tree7M = glm::translate(tree7M, glm::vec3(-6.0f, -5.0f, -5.0f));
    tree7M = glm::rotate(tree7M, glm::radians(90.0f), glm::vec3(0, 1, 0));
    tree7M = glm::scale(tree7M, glm::vec3(0.8f, 0.8f, 0.8f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(tree7M));
    glUniform4f(spLambert->u("color"), 0.14f, 0.48f, 0.14f, 1.0f);
    drzewo.drawSolid(true);

    // Drzewo 8 - z prawej strony daleko
    glm::mat4 tree8M = glm::mat4(1.0f);
    tree8M = glm::translate(tree8M, glm::vec3(8.5f, -5.0f, -4.0f));
    tree8M = glm::rotate(tree8M, glm::radians(15.0f), glm::vec3(0, 1, 0));
    tree8M = glm::scale(tree8M, glm::vec3(1.0f, 1.0f, 1.0f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(tree8M));
    glUniform4f(spLambert->u("color"), 0.28f, 0.62f, 0.25f, 1.0f);
    drzewo2.drawSolid(true);

    // ====== DODATKOWE MAŁE DRZEWA (POPRAWIONA WYSOKOŚĆ) ======

    // Małe drzewo 1
    glm::mat4 smallTree1M = glm::mat4(1.0f);
    smallTree1M = glm::translate(smallTree1M, glm::vec3(-4.5f, -5.1f, 2.0f));
    smallTree1M = glm::rotate(smallTree1M, glm::radians(75.0f), glm::vec3(0, 1, 0));
    smallTree1M = glm::scale(smallTree1M, glm::vec3(0.5f, 0.5f, 0.5f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(smallTree1M));
    glUniform4f(spLambert->u("color"), 0.20f, 0.52f, 0.18f, 1.0f);
    drzewo.drawSolid(true);

    // Małe drzewo 2
    glm::mat4 smallTree2M = glm::mat4(1.0f);
    smallTree2M = glm::translate(smallTree2M, glm::vec3(4.2f, -5.1f, 1.5f));
    smallTree2M = glm::rotate(smallTree2M, glm::radians(-45.0f), glm::vec3(0, 1, 0));
    smallTree2M = glm::scale(smallTree2M, glm::vec3(0.6f, 0.6f, 0.6f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(smallTree2M));
    glUniform4f(spLambert->u("color"), 0.26f, 0.60f, 0.23f, 1.0f);
    drzewo2.drawSolid(true);

    // Małe drzewo 3
    glm::mat4 smallTree3M = glm::mat4(1.0f);
    smallTree3M = glm::translate(smallTree3M, glm::vec3(-2.0f, -5.1f, 6.0f));
    smallTree3M = glm::rotate(smallTree3M, glm::radians(150.0f), glm::vec3(0, 1, 0));
    smallTree3M = glm::scale(smallTree3M, glm::vec3(0.4f, 0.4f, 0.4f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(smallTree3M));
    glUniform4f(spLambert->u("color"), 0.16f, 0.50f, 0.16f, 1.0f);
    drzewo.drawSolid(true);

    // ====== DRZEWA NA LEKKICH WZNIESIENIACH ======

    // Drzewo na wzniesieniu 1 (nie za wysoko)
    glm::mat4 hillTree1M = glm::mat4(1.0f);
    hillTree1M = glm::translate(hillTree1M, glm::vec3(-10.0f, -4.5f, -8.0f));
    hillTree1M = glm::rotate(hillTree1M, glm::radians(30.0f), glm::vec3(0, 1, 0));
    hillTree1M = glm::scale(hillTree1M, glm::vec3(1.3f, 1.3f, 1.3f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(hillTree1M));
    glUniform4f(spLambert->u("color"), 0.10f, 0.42f, 0.10f, 1.0f);
    drzewo.drawSolid(true);

    // Drzewo na wzniesieniu 2 (nie za wysoko)
    glm::mat4 hillTree2M = glm::mat4(1.0f);
    hillTree2M = glm::translate(hillTree2M, glm::vec3(12.0f, -4.3f, -6.0f));
    hillTree2M = glm::rotate(hillTree2M, glm::radians(-120.0f), glm::vec3(0, 1, 0));
    hillTree2M = glm::scale(hillTree2M, glm::vec3(1.4f, 1.4f, 1.4f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(hillTree2M));
    glUniform4f(spLambert->u("color"), 0.08f, 0.40f, 0.08f, 1.0f);
    drzewo2.drawSolid(true);

    // ====== ORZEŁ z jasną teksturą ======
    spTextured->use();
    glUniformMatrix4fv(spTextured->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spTextured->u("V"), 1, false, glm::value_ptr(V));

    glm::mat4 eagleM = glm::mat4(1.0f);
    eagleM = glm::translate(eagleM, glm::vec3(eagleX, eagleY, eagleZ));

    // DODAJ KOREKTĘ ORIENTACJI - testuj różne wartości:
    eagleM = glm::rotate(eagleM, glm::radians(180.0f), glm::vec3(0, 1, 0)); // Obrót o 180° w osi Y
    // LUB spróbuj:
    // eagleM = glm::rotate(eagleM, glm::radians(90.0f), glm::vec3(0, 1, 0));   // 90° w prawo
    // eagleM = glm::rotate(eagleM, glm::radians(-90.0f), glm::vec3(0, 1, 0));  // 90° w lewo

    // Następnie normalne obroty sterowania:
    eagleM = glm::rotate(eagleM, eagleYaw, glm::vec3(0, 1, 0));
    eagleM = glm::rotate(eagleM, eaglePitch, glm::vec3(1, 0, 0));
    eagleM = glm::rotate(eagleM, eagleRoll, glm::vec3(0, 0, 1));
    eagleM = glm::scale(eagleM, glm::vec3(0.6f, 0.6f, 0.6f));

    glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(eagleM));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texEagle);
    glUniform1i(spTextured->u("tex"), 0);

    orzel.drawSolid(true);

    // ====== PTAK NPC ======
    spLambert->use();
    glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));

    glBindTexture(GL_TEXTURE_2D, 0);

    glm::mat4 npcBirdM = glm::mat4(1.0f);
    npcBirdM = glm::translate(npcBirdM, glm::vec3(npcBirdX, npcBirdY, npcBirdZ));
    npcBirdM = glm::rotate(npcBirdM, npcBirdRotation, glm::vec3(0, 1, 0));
    npcBirdM = glm::scale(npcBirdM, glm::vec3(0.3f, 0.3f, 0.3f));

    glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(npcBirdM));
    glUniform4f(spLambert->u("color"), 0.8f, 0.3f, 0.1f, 1.0f);

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

    float lastTime = 0;

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        updateEagle(window, deltaTime);   // Realistyczne sterowanie orłem
        updateNPCBird(deltaTime);         // AI ptaka NPC
        updateCamera(deltaTime);          // Aktualizacja kamery
        drawScene(window, 0.0f);
        glfwPollEvents();
    }

    freeOpenGLProgram(window);
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}