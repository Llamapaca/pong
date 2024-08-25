
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

#include <shader.h>

int WIDTH = 800;
int HEIGHT = 600;

struct Vertices
{
  std::vector<float> data;
  GLint size = 2;

  GLuint totalSize() const
  {
    return sizeof(float) * this->data.size();
  }

  Vertices(std::vector<float> data) : data(data) {}

  ~Vertices()
  {
    std::cout << "Verticies Destroyed" << std::endl;
  }

  int indicesCount() const
  {
    return (this->totalSize() / this->size) / sizeof(float);
  }
};

class GLObject2D
{
public:
  std::unique_ptr<Shader> shader;
  std::unique_ptr<Vertices> verticies;
  GLuint VAO, VBO;

  GLObject2D(std::unique_ptr<Shader> &shader,
             std::unique_ptr<Vertices> &verticies)
      : shader(std::move(shader)), verticies(std::move(verticies))
  {
    this->createArrays();
  }

  ~GLObject2D()
  {
    std::cout << "GLObject Destroyed" << std::endl;
  }

  void render(const glm::vec2 scale, const glm::vec2 transform) const
  {
    this->shader->use();
    this->shader->setvec2("stretch", scale);
    this->shader->setvec2("translate", transform);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, this->verticies->indicesCount());
    glBindVertexArray(0);
  }

  void createArrays()
  {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->verticies->totalSize(), this->verticies->data.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, this->verticies->size, GL_FLOAT, GL_FALSE,
                          this->verticies->size * sizeof(float), (void *)0);
    glBindVertexArray(0);
  }
};
