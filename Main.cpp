#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

//start clean code ... 

const unsigned int width = 1280;
const unsigned int height = 720;

// Vertices pour cubes
GLfloat cubeVertices[] =
{
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f
};

// Indices pour cubes
GLuint cubeIndices[] =
{
    0, 1, 2, 0, 2, 3,
    4, 5, 6, 4, 6, 7,
    0, 1, 5, 0, 5, 4,
    2, 3, 7, 2, 7, 6,
    0, 3, 7, 0, 7, 4,
    1, 2, 6, 1, 6, 5
};

int main()
{   
    int testAnimation = 0;
    // Initialisation de GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Cr�er la fen�tre GLFW
    GLFWwindow* window = glfwCreateWindow(width, height, "Rotating Cubes", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Erreur lors de la cr�ation de la fen�tre GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    // Compiler les shaders
    Shader shaderProgram("default.vert", "default.frag");

    // Pr�paration du premier cube
    VAO VAO1;
    VAO1.Bind();
    VBO VBO1(cubeVertices, sizeof(cubeVertices));
    EBO EBO1(cubeIndices, sizeof(cubeIndices));
    VAO1.LinkVBO(VBO1, 0);
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    // Pr�paration du deuxi�me cube
    VAO VAO2;
    VAO2.Bind();
    VBO VBO2(cubeVertices, sizeof(cubeVertices));
    EBO EBO2(cubeIndices, sizeof(cubeIndices));
    VAO2.LinkVBO(VBO2, 0);
    VAO2.Unbind();
    VBO2.Unbind();
    EBO2.Unbind();

    float rotationY = 0.0f; // Rotation pour le premier cube (autour de Y)
    float rotationZ = 0.0f; // Rotation pour le deuxi�me cube (autour de Z)
    float sceneRotation = 0.0f; // Rotation de la sc�ne autour de Y (axe global)
    double prevTime = glfwGetTime();

    // Boucle principale
    while (!glfwWindowShouldClose(window))
    {
        // G�rer l'appui sur la touche ESC
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Effacer l'�cran
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activer le programme shader
        shaderProgram.Activate();

        // Mise � jour de la rotation
        double crntTime = glfwGetTime();
        if (crntTime - prevTime >= 1 / 60)
        {
            rotationY += 0.5f; // Rotation autour de Y pour le premier cube
            rotationZ += 0.5f; // Rotation autour de Z pour le deuxi�me cube
            sceneRotation += 0.1f; // Rotation de la sc�ne autour de Y (axe global)
            prevTime = crntTime;
        }

        // Cr�ation de la transformation de la cam�ra
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

        // Rotation globale de la sc�ne
        glm::mat4 sceneRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(sceneRotation), glm::vec3(0.0f, 1.0f, 0.0f));

        // Dessiner le premier cube (rotation autour de Y)
        glm::mat4 model1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 1.0f, 0.0f)); // Positionner le premier cube plus haut
        model1 = glm::rotate(model1, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation locale autour de Y (gauche � droite)
        model1 = glm::scale(model1, glm::vec3(0.5f)); // Redimensionner � 0.5
        model1 = sceneRotationMatrix * model1; // Appliquer la rotation de la sc�ne
        int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
        int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
        int colorLoc = glGetUniformLocation(shaderProgram.ID, "color");
        glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f); // Bleu pour le premier cube
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Dessiner le deuxi�me cube (rotation autour de Z)
        glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, -1.0f, 0.0f)); // Positionner le deuxi�me cube plus bas
        model2 = glm::rotate(model2, glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotation locale autour de Z (gauche � droite)
        model2 = glm::scale(model2, glm::vec3(0.5f)); // Redimensionner � 0.5
        model2 = sceneRotationMatrix * model2; // Appliquer la rotation de la sc�ne
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
        glUniform3f(colorLoc, 0.0f, 0.0f, 1.0f); // Rouge pour le deuxi�me cube
        VAO2.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // �changer les buffers et traiter les �v�nements
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Lib�rer les ressources
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    VAO2.Delete();
    VBO2.Delete();
    EBO2.Delete();
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}