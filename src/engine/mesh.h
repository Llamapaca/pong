#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glad/gl.h>

#include "shader.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
  glm::vec3 Tangent;
  glm::vec3 Bitangent;
  int m_BoneIDs[MAX_BONE_INFLUENCE];
  float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
  unsigned int id;
  std::string type;
};

class Mesh {
 public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  unsigned int VAO;

  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures)
      : vertices(vertices), indices(indices), textures(textures) {
    this->setupMesh();
  };

  void Draw(Shader &shader) {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < this->textures.size(); i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      std::string number;
      std::string name = this->textures[i].type;
      if (name == "texture_diffuse")
        number = std::to_string(diffuseNr++);
      else if (name == "texture_specular")
        number = std::to_string(specularNr++);
      else if (name == "texture_normal")
        number = std::to_string(normalNr++);
      else if (name == "texture_height")
        number = std::to_string(heightNr++);

      shader.setInt(name + number, i);
      glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
    }

    // draw
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES,
                   static_cast<unsigned int>(this->indices.size()),
                   GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
  };

 private:
  unsigned int VBO, EBO;

  void setupMesh() {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
                 &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 this->indices.size() * sizeof(unsigned int), &this->indices[0],
                 GL_STATIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Normal));

    // Vertex texture coors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, TexCoords));

    // Vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Tangent));

    // Vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Bitangent));

    // Vertex bone ids
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, MAX_BONE_INFLUENCE, GL_INT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));

    // Vertex weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));

    glBindVertexArray(0);
  };
};

#endif
