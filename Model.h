#ifndef LAB11_MODEL_H
#define LAB11_MODEL_H

#include <glm/glm.hpp>

struct Model {
    GLuint VAO;
    GLuint vertex_buffer, uv_buffer, normal_buffer, element_buffer;
    long int size;
    GLuint texture1, texture2;

    glm::vec3 translation;
};

#endif //LAB11_MODEL_H
