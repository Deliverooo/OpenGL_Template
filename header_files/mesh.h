#pragma once
#include "shader.h"
#include "assimp/types.h"
#include "glad/glad.h"

#ifndef MESH_H
#define MESH_H

struct Texture {
    GLuint id;
    std::string type;
    aiString path;
};

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Material {
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
    float shininess;
};

class Mesh {

    public:

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;
        Material material;

        GLuint VAO;

        Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<Texture> textures) {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            prepareMesh();
        }

        void draw(Shader &shader) {

            GLuint diffuseNr = 1;
            GLuint specularNr = 1;
            GLuint normalNr = 1;
            GLuint heightNr = 1;

            for (GLuint i = 0; i < textures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i);

                std::string num;
                std::string name = textures[i].type;
                if (name == "diffuseTex") {
                    num = std::to_string(diffuseNr++);
                } else if (name == "specularTex") {
                    num = std::to_string(specularNr++);
                } else if (name == "normalTex") {
                    num = std::to_string(normalNr++);
                } else if (name == "heightTex") {
                    num = std::to_string(heightNr++);
                } else {
                    std::cout << name << " is not a valid texture type!" << std::endl;
                }
                glUniform1i(glGetUniformLocation(shader.ID, (name + num).c_str()), i);

                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
        }

    private:
        GLuint vboID, eboID;

        void prepareMesh() {
            //generates the vertex arrays and buffers
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &vboID);
            glGenBuffers(1, &eboID);

            //binds the vertex array, so it is the one that is currently active
            glBindVertexArray(VAO);
            //binds the buffer so we can store data in it
            glBindBuffer(GL_ARRAY_BUFFER, vboID);

            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
            //let me break it down for you Mark.
            //the size is the number of elements in the attribute. e.g. a vec3 would have 3 and a vec2 would have 2
            //the stride is the total number of elements multiplied by the float size in bytes. e.g. a vec2 and vec3 would have a combined size of 5
            // the pointer is the offset for each attribute from the previous one
            // position attribute
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            //normals attribute
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
            //texture coordinates attribute
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

            // You can unbind the VAO afterward so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
            // VAOs requires a call to glBindVertexArray anyway so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
            glBindVertexArray(0);
        }

};

#endif