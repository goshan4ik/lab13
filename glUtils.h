#pragma once
#include <GL\glew.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

GLint getAttributeLocation(GLuint Program, const char* attr_name) {
	auto location = glGetAttribLocation(Program, attr_name);
	if (location == -1)
	{
		std::cout << "could not bind attrib " << attr_name << std::endl;
	}
	return location;
}

void shaderLog(unsigned int shader)
{
	int infologLen = 0;
	int charsWritten = 0;
	char* infoLog;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		infoLog = new char[infologLen];
		if (infoLog == NULL)
		{
			std::cout << "ERROR: Could not allocate InfoLog buffer\n";
			exit(1);
		}
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
		std::cout << "InfoLog: " << infoLog << "\n\n\n";
		delete[] infoLog;
	}
}

void checkOpenGLerror()
{
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode);
}

//void loadOBJ(const std::string& path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals, const double& scale) {
//    std::vector<unsigned int> vertex_indices, uv_indices, normal_indices;
//    std::vector<glm::vec3> temp_vertices;
//    std::vector<glm::vec2> temp_uvs;
//    std::vector<glm::vec3> temp_normals;
//
//    std::ifstream infile(path);
//    std::string line;
//    while (getline(infile, line))
//    {
//        std::stringstream ss(line);
//        std::string lineHeader;
//        getline(ss, lineHeader, ' ');
//        if (lineHeader == "v")
//        {
//            glm::vec3 vertex;
//            ss >> vertex.x >> vertex.y >> vertex.z;
//            vertex.x *= scale;
//            vertex.y *= scale;
//            vertex.z *= scale;
//            temp_vertices.push_back(vertex);
//        }
//        else if (lineHeader == "vt")
//        {
//            glm::vec2 uv;
//            ss >> uv.x >> uv.y;
//            temp_uvs.push_back(uv);
//        }
//        else if (lineHeader == "vn")
//        {
//            glm::vec3 normal;
//            ss >> normal.x >> normal.y >> normal.z;
//            temp_normals.push_back(normal);
//        }
//        else if (lineHeader == "f")
//        {
//            unsigned int vertex_index, uv_index, normal_index;
//            char slash;
//            while (ss >> vertex_index >> slash >> uv_index >> slash >> normal_index)
//            {
//                vertex_indices.push_back(vertex_index);
//                uv_indices.push_back(uv_index);
//                normal_indices.push_back(normal_index);
//            }
//        }
//    }
//
//    for (unsigned int i = 0; i < vertex_indices.size(); i++)
//    {
//        unsigned int vertexIndex = vertex_indices[i];
//        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
//        out_vertices.push_back(vertex);
//
//        unsigned int uvIndex = uv_indices[i];
//        glm::vec2 uv = temp_uvs[uvIndex - 1];
//        out_uvs.push_back(uv);
//
//        unsigned int normalIndex = normal_indices[i];
//        glm::vec3 normal = temp_normals[normalIndex - 1];
//        out_normals.push_back(normal);
//    }
//}

