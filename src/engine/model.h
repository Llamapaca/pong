#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <map>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>

#include "shader.h"
#include "mesh.h"

unsigned int TextureFromFile(const std::string& path,
                             const std::string& directory, bool gamma = false) {
  std::string filename = directory + '/' + path;

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char* data =
      stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (!data) {
    std::cout << "Texture failed to load at path: " << filename << std::endl;
    stbi_image_free(data);
    throw std::runtime_error("Failed to read Texture");
  }

  GLenum format;
  if (nrComponents == 1)
    format = GL_RED;
  else if (nrComponents == 3)
    format = GL_RGB;
  else if (nrComponents == 4)
    format = GL_RGBA;

  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);

  std::cout << "Loaded Texture: " << filename << std::endl;

  return textureID;
};

class Model {
 public:
  std::vector<Texture> textures_loaded;
  std::map<const char*, Texture> textures;
  std::vector<Mesh> meshes;
  std::string directory;
  bool gammaCorrection;

  Model(std::string path) { loadModel(path); };
  void Draw(Shader& shader) {
    for (auto mesh : this->meshes) {
      mesh.Draw(shader);
    }
  };

 private:
  void loadModel(std::string path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                  aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
      std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
      throw std::runtime_error("Failed to load assets");
    }
    this->directory = path.substr(0, path.find_last_of("/"));
    this->processNode(scene->mRootNode, scene);
  };

  void processNode(aiNode* node, const aiScene* scene) {
    std::cout << "Processing Node" << std::endl;

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      this->meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
      this->processNode(node->mChildren[i], scene);
    }
  };

  Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Vertex vertex;
      glm::vec3 _pos(mesh->mVertices[i].x, mesh->mVertices[i].y,
                     mesh->mVertices[i].z);
      vertex.Position = _pos;
      if (mesh->HasNormals()) {
        glm::vec3 _norm(mesh->mNormals[i].x, mesh->mNormals[i].y,
                        mesh->mNormals[i].z);
        vertex.Normal = _norm;
      }

      if (mesh->mTextureCoords[0]) {
        glm::vec2 _texCoords(mesh->mTextureCoords[0][i].x,
                             mesh->mTextureCoords[0][i].y);
        vertex.TexCoords = _texCoords;
        glm::vec3 _tan(mesh->mTangents[i].x, mesh->mTangents[i].y,
                       mesh->mTangents[i].z);
        vertex.Tangent = _tan;

        glm::vec3 _btan(mesh->mBitangents[i].x, mesh->mBitangents[i].y,
                        mesh->mBitangents[i].z);
        vertex.Bitangent = _btan;
      } else {
        vertex.TexCoords = glm::vec2(0.0f);
      }

      vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++) {
        indices.push_back(face.mIndices[j]);
      }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    {
      std::vector<Texture> diffuseMaps = loadMaterialTextures(
          material, aiTextureType_DIFFUSE, "texture_diffuse");
      textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    }
    {
      std::vector<Texture> specularMaps = loadMaterialTextures(
          material, aiTextureType_SPECULAR, "texture_specular");
      textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    {
      std::vector<Texture> normalMaps = loadMaterialTextures(
          material, aiTextureType_NORMALS, "texture_normal");
      textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }
    {
      std::vector<Texture> heightMaps = loadMaterialTextures(
          material, aiTextureType_HEIGHT, "texture_height");
      textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    Mesh ourMesh(vertices, indices, textures);
    return ourMesh;
  };

  std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                            std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
      aiString path;
      mat->GetTexture(type, i, &path);

      std::cout << "Path: " << path.C_Str() << std::endl;

      if (this->textures.find(path.C_Str()) != this->textures.end()) {
        textures.push_back(this->textures[path.C_Str()]);
        continue;
      }

      Texture texture;
      texture.id = TextureFromFile(path.C_Str(), this->directory);
      texture.type = typeName;
      textures.push_back(texture);
      this->textures[path.C_Str()] = texture;
    }

    return textures;
  };
};

#endif
