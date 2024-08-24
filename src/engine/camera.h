#ifndef CAMERA_H
#define CAMERA_H

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <functional>

const float YAW = -90.0f;
const float PITCH = -0.0f;
const float SPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;

  float Yaw;
  float Pitch;

  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;

  //  Mouse movement
  float lastMouseX = 400, lastMouseY = 300;
  bool mouseInit = true;

  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 4.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
         float pitch = PITCH)
      : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        Zoom(ZOOM),
        Position(position),
        WorldUp(up),
        Yaw(yaw),
        Pitch(pitch)
  {
    this->updateCameraVectors();
  }

  glm::mat4 GetViewMatrix(bool mirrored = false)
  {
    if (mirrored)
    {
      this->Yaw += 180.0f;
      this->updateCameraVectors();
      auto lookat =
          glm::lookAt(this->Position, this->Position + this->Front, this->Up);
      this->Yaw -= 180.0f;
      this->updateCameraVectors();
      return lookat;
    }
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
  }

  void ProcessKeyboard(std::function<int(int)> get_key, float deltaTime)
  {
    float velocity = this->MovementSpeed * deltaTime;
    if (get_key(GLFW_KEY_W))
    {
      this->Position += this->Front * velocity;
    }
    if (get_key(GLFW_KEY_S))
    {
      this->Position -= this->Front * velocity;
    }
    if (get_key(GLFW_KEY_A))
    {
      this->Position -= this->Right * velocity;
    }
    if (get_key(GLFW_KEY_D))
    {
      this->Position += this->Right * velocity;
    }
    if (get_key(GLFW_KEY_E))
    {
      this->Position += this->Up * velocity;
    }
    if (get_key(GLFW_KEY_Q))
    {
      this->Position -= this->Up * velocity;
    }
  }

  void ProcessMouseMovement(double xpos, double ypos,
                            GLboolean constrainPitch = true)
  {
    if (this->mouseInit)
    {
      this->lastMouseX = xpos;
      this->lastMouseY = ypos;
      this->mouseInit = false;
    }
    float xoffset = (xpos - this->lastMouseX) * this->MouseSensitivity;
    float yoffset = (ypos - this->lastMouseY) * this->MouseSensitivity;
    this->lastMouseX = xpos;
    this->lastMouseY = ypos;

    this->Yaw += xoffset;
    this->Pitch -= yoffset;

    if (constrainPitch)
    {
      if (this->Pitch > 89.0f)
      {
        this->Pitch = 89.0f;
      }
      if (this->Pitch < -89.0f)
      {
        this->Pitch = -89.0f;
      }
    }

    this->updateCameraVectors();
  }

  void ProcessMouseScroll(float yoffset)
  {
    this->Zoom -= (float)yoffset;
    if (this->Zoom < 1.0f)
      this->Zoom = 1.0f;
    if (this->Zoom > 45.0f)
      this->Zoom = 45.0f;
  }

private:
  void updateCameraVectors()
  {
    glm::vec3 front;
    front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    front.y = sin(glm::radians(this->Pitch));
    front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    this->Front = glm::normalize(front);

    this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
    this->Up = glm::normalize(glm::cross(this->Right, this->Front));
  }
};

#endif
