#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "GLFWinit.hpp"
#include <camera.h>
#include <shader.h>
#include <GLObject.h>
#include <log.cpp>

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void onScroll(GLFWwindow *window, double xoffset, double yoffset)
{
    Camera *camera = static_cast<Camera *>(glfwGetWindowUserPointer(window));
    camera->ProcessMouseScroll(yoffset);
}
void onCursorPos(GLFWwindow *window, double xoffset, double yoffset)
{
    Camera *camera = static_cast<Camera *>(glfwGetWindowUserPointer(window));
    camera->ProcessMouseMovement(xoffset, yoffset);
}

void onKeyPress(GLFWwindow *window, int key, int scancode, int action,
                int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

int main()
{
    GLFWwindow *window = getWindow(800, 600);
    Camera camera;
    glfwSetWindowUserPointer(window, &camera);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetCursorPosCallback(window, onCursorPos);
    glfwSetScrollCallback(window, onScroll);
    glfwSetKeyCallback(window, onKeyPress);
    float rectangleVertices[] = {
        0.125f, 0.5f,   // top right
        0.125f, -0.5f,  // bottom right
        -0.125f, -0.5f, // bottom left
        -0.125f, -0.5f, // top right
        -0.125f, 0.5f,  // bottom right
        0.125f, 0.5f,   // bottom left
    };
    Shader scene("src/shaders/scene.vertex", "src/shaders/scene.fragment");

    GLObject2D cube(
        camera, scene, 0,
        GLObject2D::Verticies(rectangleVertices, sizeof(rectangleVertices), 2, 0)

    );

    float BallVertices[] = {
        0.1f, 0.1f,   // top right
        0.1f, -0.1f,  // bottom right
        -0.1f, -0.1f, // bottom left
        -0.1f, -0.1f, // top right
        -0.1f, 0.1f,  // bottom right
        0.1f, 0.1f,   // bottom left
    };
    GLObject2D ball(
        camera, scene, 0,
        GLObject2D::Verticies(BallVertices, sizeof(BallVertices), 2, 0)

    );
    int speed = 8, ballSpeed = 1;
    float leftCubeY = 0.0f, rightCubeY = 0.0f;
    glm::vec3 ballPositon;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        log_delta_time(deltaTime);
        // camera.ProcessKeyboard([&](int key)
        //                        { return glfwGetKey(window, key); },
        //                        deltaTime);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_UP))
        {
            rightCubeY += speed * deltaTime;
            if (rightCubeY >= 1.2f)
                rightCubeY = 1.2f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN))
        {
            rightCubeY -= speed * deltaTime;
            if (rightCubeY <= -1.2f)
                rightCubeY = -1.2f;
        }

        if (glfwGetKey(window, GLFW_KEY_W))
        {
            leftCubeY += speed * deltaTime;
            if (leftCubeY >= 1.2f)
                leftCubeY = 1.2f;
        }
        if (glfwGetKey(window, GLFW_KEY_S))
        {
            leftCubeY -= speed * deltaTime;
            if (leftCubeY <= -1.2f)
                leftCubeY = -1.2f;
        }

        cube.render(glm::vec3(2.0f, rightCubeY, 0.0f));
        cube.render(glm::vec3(-2.0f, leftCubeY, 0.0f));
        ball.render(ballPositon);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    scene.deleteProgram();

    glfwTerminate();
    return 0;
}
