
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>

int WIDTH = 800;
int HEIGHT = 600;

class GLObject2D {
 public:
  struct Verticies {
    int dataSize, posSize, texSize, stride;
    float *rawData;

    Verticies(float *rawData, int dataSize, int posSize, int texSize)
        : rawData(rawData),
          dataSize(dataSize),
          posSize(posSize),
          texSize(texSize),
          stride(posSize + texSize) {}

    int indicesCount() const {
      return (this->dataSize / this->stride) / sizeof(float);
    }
  };

  Shader &shader;
  Camera &camera;
  Verticies verticies;
  unsigned int VAO, VBO, texture;

  GLObject2D(Camera &camera, Shader &shader, unsigned int texture,
             Verticies verticies)
      : camera(camera), shader(shader), texture(texture), verticies(verticies) {
    this->createArrays(verticies);
  }

  void render(glm::vec3 transform = glm::vec3(), bool mirror = false) {
    auto view = this->camera.GetViewMatrix(mirror);
    auto projection =
        glm::perspective(glm::radians(this->camera.Zoom),
                         (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    this->shader.use();
    this->shader.setMat4("view", view);
    this->shader.setMat4("projection", projection);
    this->shader.setMat4("model", glm::translate(glm::mat4(1.0f), transform));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glDrawArrays(GL_TRIANGLES, 0, this->verticies.indicesCount());
    glBindVertexArray(0);
  }

  void createArrays(Verticies verticies) {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, verticies.dataSize, verticies.rawData,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, verticies.posSize, GL_FLOAT, GL_FALSE,
                          verticies.stride * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, verticies.texSize, GL_FLOAT, GL_FALSE,
                          verticies.stride * sizeof(float),
                          (void *)(verticies.posSize * sizeof(float)));
    glBindVertexArray(0);
  }
};

class GLSkybox : public GLObject2D {
 public:
  using GLObject2D::GLObject2D;

  void render(glm::vec3 transform = glm::vec3(), bool mirror = false) {
    auto view = glm::mat4(glm::mat3(this->camera.GetViewMatrix()));
    auto projection =
        glm::perspective(glm::radians(this->camera.Zoom),
                         (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    this->shader.use();
    this->shader.setMat4("view", view);
    this->shader.setMat4("projection", projection);
    this->shader.setMat4("model", glm::translate(glm::mat4(1.0f), transform));

    glDepthMask(GL_FALSE);
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);
    glDrawArrays(GL_TRIANGLES, 0, this->verticies.indicesCount());
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
  }
};

class GLMirror : public GLObject2D {
 public:
  unsigned int frameBuffer;

  GLMirror(Camera &camera, Shader &shader, Verticies verticies)
      : GLObject2D(camera, shader, 0, verticies) {
    this->createFrameBuffer();
  }

  void createFrameBuffer() {
    glGenFramebuffers(1, &this->frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->frameBuffer);

    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           this->texture, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                << std::endl;
  }
};
