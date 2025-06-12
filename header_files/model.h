#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <stb_image.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "mesh.h"

struct aiMaterial;

using namespace std;

class Model {
    public:

        vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        vector<Mesh>    meshes;
        string directory;

        Model(const string &filepath) {
            loadModel(filepath);
        }
        void draw(Shader &shader) {

            for (GLuint i = 0; i < meshes.size(); i++) {
                meshes[i].draw(shader);
            }
        }

    private:

        GLuint loadTexture(const char* path, const std::string &directory) {

            string filename = string(path);
            filename = directory + '/' + filename;

            GLuint textureID;
            glGenTextures(1, &textureID);

            int width, height, nrComponents;
            unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format = 0;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
                std::cout << "Loaded texture with path: " << filename << std::endl;
            }
            else
            {
                std::cout << "Texture failed to load at path: " << path << std::endl;

                stbi_image_free(data);
            }

            return textureID;
        }
        std::vector<Texture> loadMaterialTex(aiMaterial *material, aiTextureType type, const std::string& typeName) {

            std::vector<Texture> textures;
            for (GLuint i = 0; i < material->GetTextureCount(type); i++) {
                aiString str;
                material->GetTexture(type, i, &str);
                bool skip = false;
                for (GLuint j = 0; j < textures_loaded.size(); j++) {

                    if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0) {

                        textures.push_back(textures_loaded[j]);
                        skip = true;
                        break;
                    }
                }
                if (!skip) {
                    Texture texture;
                    texture.id = loadTexture(str.C_Str(), directory);
                    texture.type = typeName;
                    texture.path = str;
                    textures.push_back(texture);
                    textures_loaded.push_back(texture);
                }
            }
            return textures;
        }
        Material loadMaterial(aiMaterial *mat) {

            Material material;
            aiColor3D colour(0.0f, 0.0f, 0.0f);
            float shininess;

            mat->Get(AI_MATKEY_COLOR_DIFFUSE, colour);
            material.diffuse = glm::vec3(colour[0], colour[1], colour[2]);

            mat->Get(AI_MATKEY_COLOR_AMBIENT, colour);
            material.ambient = glm::vec3(colour[0], colour[1], colour[2]);

            mat->Get(AI_MATKEY_COLOR_SPECULAR, colour);
            material.specular = glm::vec3(colour[0], colour[1], colour[2]);

            mat->Get(AI_MATKEY_SHININESS, shininess);
            material.shininess = shininess;

            return material;
        }

        Mesh processMesh(aiMesh *mesh, const aiScene *scene) {

            //for the input mesh, it will create a list of vertices, indies and textures for it
            std::vector<Vertex> vertices;
            std::vector<GLuint> indices;
            std::vector<Texture> textures;

            for (GLuint i = 0; i < mesh->mNumVertices; i++) {
                Vertex vertex;

                glm::vec3 vector;
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.Position = vector;

                glm::vec3 normal;
                normal.x = mesh->mNormals[i].x;
                normal.y = mesh->mNormals[i].y;
                normal.z = mesh->mNormals[i].z;
                vertex.Normal = normal;

                if (mesh->mTextureCoords[0]) {
                    glm::vec2 tex;
                    tex.x = mesh->mTextureCoords[0][i].x;
                    tex.y = mesh->mTextureCoords[0][i].y;
                    vertex.TexCoords = tex;
                } else {
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                }
                vertices.push_back(vertex);
            }

            for (GLuint i = 0; i < mesh->mNumFaces; i++) {

                aiFace face = mesh->mFaces[i];
                for (int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }

            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            // 1. diffuse maps
            vector<Texture> diffuseMaps = loadMaterialTex(material, aiTextureType_DIFFUSE, "diffuseTex");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            vector<Texture> specularMaps = loadMaterialTex(material, aiTextureType_SPECULAR, "specularTex");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<Texture> normalMaps = loadMaterialTex(material, aiTextureType_HEIGHT, "normalTex");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<Texture> heightMaps = loadMaterialTex(material, aiTextureType_AMBIENT, "heightTex");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

            return Mesh(vertices, indices, textures);
        }

        //this function takes in a node and recursively creates a mesh for each of its children, then adds it to the mesh
        void processNode(aiNode *node, const aiScene *scene) {

            for (GLuint i = 0; i < node->mNumMeshes; i++) {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(processMesh(mesh, scene));
            }
            for (GLuint i = 0; i < node->mNumChildren; i++) {
                processNode(node->mChildren[i], scene);
            }

        }

        void loadModel(const std::string &filepath) {
            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
                return;
            }
            //calls the process node for the root node, meaning all subsequent meshes will be added to the
            //mesh vertex list
            directory = filepath.substr(0, filepath.find_last_of('/'));

            processNode(scene->mRootNode, scene);

        }
};
#endif //MODEL_H