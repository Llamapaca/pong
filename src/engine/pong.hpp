#ifndef PONG_HPP
#define PONG_HPP
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <memory>
#include <vector>

#include "GLFWinit.hpp"
#include <shader.h>
#include <GLObject.h>

namespace Pong
{
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

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

  void printVec2(const glm::vec2 &vec)
  {
    std::cout << "x = " << vec.x << " y = " << vec.y << std::endl;
  }

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist2(0, 1);

  class Playground
  {
  public:
    std::shared_ptr<GLObject2D> globj2d;
    glm::vec2 wall;

    Playground(std::shared_ptr<GLObject2D> globj2d, glm::vec2 wall)
        : globj2d(globj2d), wall(wall)
    {
    }

    void draw() const
    {
      this->drawCenterLine();
      this->drawWalls();
    }

    void drawCenterLine() const
    {
      for (float i = -0.9; i < 1; i += 0.1)
      {
        this->globj2d->render(glm::vec2(0.01, 0.01), glm::vec2(0.0, i));
      }
    }

    void drawWalls() const
    {

      this->globj2d->render(glm::vec2(1, 0.01), this->wall);
      this->globj2d->render(glm::vec2(1, 0.01), -this->wall);
    }
  };

  class Rectangle
  {
  public:
    float speed;
    glm::vec2 position, wall;
    GLFWwindow *window;
    glm::vec2 scale = glm::vec2(0.05, 0.3);
    GLint up, down;

    std::shared_ptr<GLObject2D> globj2d;

    Rectangle(std::shared_ptr<GLObject2D> globj2d,
              glm::vec2 position, glm::vec2 wall,
              GLint up, GLint down, GLFWwindow *window)
        : globj2d(globj2d),
          speed(3),
          position(position),
          wall(wall),
          up(up),
          down(down),
          window(window)
    {
    }

    void update()
    {
      if (glfwGetKey(this->window, this->up))
      {
        this->position.y += this->speed * deltaTime;

        if (this->position.y + this->scale.y >= this->wall.y)
        {
          this->position.y = this->wall.y - this->scale.y;
        }
      }

      if (glfwGetKey(this->window, this->down))
      {
        this->position.y -= this->speed * deltaTime;
        if (this->position.y - this->scale.y <= -this->wall.y)
        {
          this->position.y = -this->wall.y + this->scale.y;
        }
      }

      this->globj2d->render(this->scale, this->position);
    }
  };

  class Ball
  {
  public:
    glm::vec2 speed;
    glm::vec2 wall;
    glm::vec2 position;
    glm::vec2 scale = glm::vec2(0.05, 0.05);
    unsigned int counter = 0;

    std::shared_ptr<GLObject2D> globj2d;

    Ball(std::shared_ptr<GLObject2D> globj2d, glm::vec2 wall)
        : globj2d(globj2d),
          wall(wall),
          speed(glm::vec2(Pong::dist2(rng) % 2 ? -0.5 : 0.5, Pong::dist2(rng) % 2 ? -0.5 : 0.5)),
          position(glm::vec2(0))
    {
    }

    void update(const Rectangle &leftPad, const Rectangle &rightPad)
    {
      this->counter++;
      this->position.x += this->speed.x * deltaTime;
      this->position.y += this->speed.y * deltaTime;

      if (this->position.x + this->scale.x >= rightPad.position.x - rightPad.scale.x)
      {
        if (this->position.x - this->scale.x <= rightPad.position.x + rightPad.scale.x)
        {
          if (this->position.y + this->scale.y >= rightPad.position.y - rightPad.scale.y)
          {
            if (this->position.y - this->scale.y <= rightPad.position.y + rightPad.scale.y)
            {
              std::cout << "Right Pad hit" << std::endl;
              this->speed.x = -this->speed.x;
            }
          }
        }
      }

      if (this->position.x - this->scale.x <= leftPad.position.x + leftPad.scale.x)
      {
        if (this->position.x + this->scale.x >= leftPad.position.x - leftPad.scale.x)
        {
          if (this->position.y + this->scale.y >= leftPad.position.y - leftPad.scale.y)
          {
            if (this->position.y - this->scale.y <= leftPad.position.y + leftPad.scale.y)
            {
              std::cout << "Left Pad hit" << std::endl;
              this->speed.x = -this->speed.x;
            }
          }
        }
      }

      if (this->position.y + this->scale.y >= this->wall.y)
      {
        std::cout << "Top Wall hit" << std::endl;
        this->speed.y = -this->speed.y;
      }
      if (this->position.y - this->scale.y <= -this->wall.y)
      {
        std::cout << "Bottom Wall hit" << std::endl;
        this->speed.y = -this->speed.y;
      }

      if (this->counter > 1000)
      {
        std::cout << "Speeding up" << std::endl;
        if (this->speed.y >= 0)
        {
          this->speed.y += Pong::dist2(Pong::rng) % 2 ? 0.2 : 0;
        }
        else
        {
          this->speed.y -= Pong::dist2(Pong::rng) % 2 ? 0.2 : 0;
        }
        if (this->speed.x >= 0)
        {
          this->speed.x += Pong::dist2(Pong::rng) % 2 ? 0.2 : 0;
        }
        else
        {
          this->speed.x -= Pong::dist2(Pong::rng) % 2 ? 0.2 : 0;
        }
        this->counter = 0;
      }

      this->globj2d->render(this->scale, this->position);
    }
  };

  class Game
  {
    GLFWwindow *window;
    std::unique_ptr<Ball> ball;
    std::unique_ptr<Rectangle> leftPad, rightPad;
    std::unique_ptr<Playground> playground;

  public:
    Game(GLFWwindow *window,
         std::shared_ptr<GLObject2D> globj2d)
        : window(window)
    {
      glm::vec2 wall(0, 0.99);

      this->leftPad = std::make_unique<Rectangle>(
          globj2d, glm::vec2(-0.91, 0.0), wall, GLFW_KEY_W, GLFW_KEY_S, this->window);
      this->rightPad = std::make_unique<Rectangle>(
          globj2d, glm::vec2(0.91, 0.0), wall, GLFW_KEY_UP, GLFW_KEY_DOWN, this->window);
      this->ball = std::make_unique<Ball>(globj2d, wall);
      this->playground = std::make_unique<Playground>(globj2d, wall);
    }

    ~Game()
    {
      std::cout << "Game deleted" << std::endl;
      glfwTerminate();
    }

    bool alive() const
    {
      return !glfwWindowShouldClose(this->window);
    }

    void run()
    {
      float currentFrame = static_cast<float>(glfwGetTime());
      Pong::deltaTime = currentFrame - lastFrame;
      Pong::lastFrame = currentFrame;

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      this->playground->draw();
      this->leftPad->update();
      this->rightPad->update();
      this->ball->update(*this->leftPad, *this->rightPad);

      glfwSwapBuffers(this->window);
      glfwPollEvents();
    }
  };

  Pong::Game getGame()
  {
    GLFWwindow *window = getWindow(800, 800);
    glfwSetKeyCallback(window, Pong::onKeyPress);

    std::unique_ptr<Shader> scene = std::make_unique<Shader>(
        "src/shaders/scene.vertex",
        "src/shaders/scene.fragment");

    const std::vector<float> ballPoints = {
        1.0f, 1.0f,   // top right
        1.0f, -1.0f,  // bottom right
        -1.0f, -1.0f, // bottom left
        -1.0f, -1.0f, // top right
        -1.0f, 1.0f,  // bottom right
        1.0f, 1.0f,   // bottom left
    };

    auto vertices = std::make_unique<Vertices>(ballPoints);

    auto globj2d = std::make_shared<GLObject2D>(scene, vertices);

    return Pong::Game(window, globj2d);
  }
}

#endif
