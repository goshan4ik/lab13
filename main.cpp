#include <GL\glew.h>
#include <SOIL/SOIL.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include "glUtils.h"
#include "Model.h"

using namespace glm;
using namespace std;

int width = 0, height = 0;

const double PI = 3.1415926535;
int textureMode = 0;

GLuint program;

glm::mat4 view_projection;
glm::mat3 normal_transform;
float view_pos[]{ 0, 10, 20 };
float light_angle = 0, light_pos = 5, light_radius = 5;
float light_position[]{ 0, 0, 0 };
float m_shininess = 16;
float spec = 1;

float rotateX = 0, rotateY = 0, rotateZ = 0;

mat4 matrix_projection;

vector<Model> models;

void initShaders() {
	string read_v = readShader("vertex2.shader");
	const char* v_shader_source = read_v.c_str();
	string read_f = readShader("fragment2.shader");
	const char* f_shader_source = read_f.c_str();
	GLuint v_shader, f_shader;
	v_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v_shader, 1, &v_shader_source, NULL);
	glCompileShader(v_shader);
	f_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f_shader, 1, &f_shader_source, NULL);
	glCompileShader(f_shader);
	program = glCreateProgram();
	glAttachShader(program, v_shader);
	glAttachShader(program, f_shader);
	glLinkProgram(program);
	glDeleteShader(v_shader);
	glDeleteShader(f_shader);
}

void reshape(int w, int h) {
	width = w;
	height = h;
	glViewport(0, 0, w, h);
}

void loadOBJ(const string& path, vector<glm::vec3>& out_vertices, vector<glm::vec2>& out_uvs, vector<glm::vec3>& out_normals, const double& scale) {
	vector<unsigned int> vertex_indices, uv_indices, normal_indices;
	vector<glm::vec3> temp_vertices;
	vector<glm::vec2> temp_uvs;
	vector<glm::vec3> temp_normals;

	ifstream infile(path);
	string line;
	while (getline(infile, line)){
		stringstream ss(line);
		string lineHeader;
		getline(ss, lineHeader, ' ');
		if (lineHeader == "v")
		{
			glm::vec3 vertex;
			ss >> vertex.x >> vertex.y >> vertex.z;
			vertex.x *= scale;
			vertex.y *= scale;
			vertex.z *= scale;
			temp_vertices.push_back(vertex);
		}
		else if (lineHeader == "vt")
		{
			glm::vec2 uv;
			ss >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (lineHeader == "vn")
		{
			glm::vec3 normal;
			ss >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (lineHeader == "f")
		{
			unsigned int vertex_index, uv_index, normal_index;
			char slash;
			while (ss >> vertex_index >> slash >> uv_index >> slash >> normal_index)
			{
				vertex_indices.push_back(vertex_index);
				uv_indices.push_back(uv_index);
				normal_indices.push_back(normal_index);
			}
		}
	}

	for (unsigned int i = 0; i < vertex_indices.size(); i++)
	{
		unsigned int vertexIndex = vertex_indices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);

		unsigned int uvIndex = uv_indices[i];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		out_uvs.push_back(uv);

		unsigned int normalIndex = normal_indices[i];
		if (temp_normals.size() != 0) {
			glm::vec3 normal = temp_normals[normalIndex - 1];
			out_normals.push_back(normal);
		}
	}
}

struct PackedVertex
{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	bool operator<(const PackedVertex that) const
	{
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
	};
};

void indexVBO(vector<glm::vec3>& in_vertices, vector<glm::vec2>& in_uvs, vector<glm::vec3>& in_normals,
	vector<unsigned short>& out_indices, vector<glm::vec3>& out_vertices, vector<glm::vec2>& out_uvs, vector<glm::vec3>& out_normals)
{
	std::map<PackedVertex, unsigned short> VertexToOutIndex;

	for (unsigned int i = 0; i < in_vertices.size(); i++)
	{
		PackedVertex packed = { in_vertices[i], in_uvs[i], in_normals[i] };

		unsigned short index;
		auto it = VertexToOutIndex.find(packed);
		if (it != VertexToOutIndex.end())
			out_indices.push_back(it->second);
		else
		{
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			unsigned short newindex = (unsigned short)out_vertices.size() - 1;
			out_indices.push_back(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}
}

Model loadModel(const char* modelName, const char* texture1FileName, const char* texture2FileName, const double& scale) {
	auto texture1 = SOIL_load_OGL_texture(texture1FileName, SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	auto texture2 = SOIL_load_OGL_texture(texture2FileName, SOIL_LOAD_AUTO,
	SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	vector<glm::vec3> vertices, normals, indexed_vertices, indexed_normals;
	vector<glm::vec2> uvs, indexed_uvs;
	loadOBJ(modelName, vertices, uvs, normals, scale);

	GLuint VAO;

	GLuint vertex_buffer, uv_buffer, normal_buffer, element_buffer;
	vector<unsigned short> object_indices;
	indexVBO(vertices, uvs, normals, object_indices, indexed_vertices, indexed_uvs, indexed_normals);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//�� �������� ������� � �����
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	//�� �������� ���������� ���������� � �����
	glGenBuffers(1, &uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	//�� �������� ������� � �����
	glGenBuffers(1, &normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	//�� �������� �������� � �����
	glGenBuffers(1, &element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object_indices.size() * sizeof(unsigned short), &object_indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	Model model;
	model.texture1 = texture1;
	model.texture2 = texture2;
	model.VAO = VAO;
	model.vertex_buffer = vertex_buffer;
	model.uv_buffer = uv_buffer; 
	model.normal_buffer = normal_buffer;
	model.element_buffer = element_buffer;
	model.size = object_indices.size();
	return model;
}

void setTransform(glm::mat4 model_pos) {
	//���������� ���� �������� �� ��������
	GLint s_model = glGetUniformLocation(program, "transform.model");
	GLint s_proj = glGetUniformLocation(program, "transform.viewProjection");
	GLint s_normal = glGetUniformLocation(program, "transform.normal");
	GLint s_view = glGetUniformLocation(program, "transform.viewPosition");

	//�������� � �������
	glUniformMatrix4fv(s_model, 1, GL_FALSE, &model_pos[0][0]);
	glUniformMatrix4fv(s_proj, 1, GL_FALSE, &view_projection[0][0]);
	glUniformMatrix3fv(s_normal, 1, GL_FALSE, &normal_transform[0][0]);
	glUniform3fv(s_view, 1, view_pos);
}

void setPointLight() {
	//���������� ���� ��������� ����� �� ��������
	GLint s_position = glGetUniformLocation(program, "light.position");
	GLint s_ambient = glGetUniformLocation(program, "light.ambient");
	GLint s_diffuse = glGetUniformLocation(program, "light.diffuse");
	GLint s_specular = glGetUniformLocation(program, "light.specular");
	GLint s_attenuation = glGetUniformLocation(program, "light.attenuation");

	float ambient[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	float diffuse[]{ 1.0f, 1.0f, 1.0f, 1.0f };
	float specular[]{ 1.0f, 1.0f, 1.0f, 1.0f };
	float attenuation[]{ 1.0f, 0.0f, 0.0f };

	glUniform4fv(s_position, 1, light_position);
	glUniform4fv(s_ambient, 1, ambient);
	glUniform4fv(s_diffuse, 1, diffuse);
	glUniform4fv(s_specular, 1, specular);
	glUniform3fv(s_attenuation, 1, attenuation);
}

void setMaterial(float* m_ambient, float* m_diffuse, float* m_specular, float* m_emission, float m_shiness) {
	GLint s_ambient = glGetUniformLocation(program, "material.ambient");
	GLint s_diffuse = glGetUniformLocation(program, "material.diffuse");
	GLint s_specular = glGetUniformLocation(program, "material.specular");
	GLint s_emission = glGetUniformLocation(program, "material.emission");
	GLint s_shiness = glGetUniformLocation(program, "material.shiness");

	glUniform4fv(s_ambient, 1, m_ambient);
	glUniform4fv(s_diffuse, 1, m_diffuse);
	glUniform4fv(s_specular, 1, m_specular);
	glUniform4fv(s_emission, 1, m_emission);
	glUniform1f(s_shiness, m_shiness);
}

void drawObject(int cur_ind, float* m_ambient, float* m_diffuse, float* m_specular, float* m_emission, float m_shiness) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glUseProgram(program);
	for (auto model: models) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, model.texture1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model.texture2);
		glEnable(GL_TEXTURE_2D);


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.element_buffer);

        glBindBuffer(GL_ARRAY_BUFFER, model.vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, model.normal_buffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, model.uv_buffer);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);


        GLint use_texture_unif = glGetUniformLocation(program, "texture_mode");
        glUniform1i(use_texture_unif, textureMode);
        float col[] = {1.0f, 0.0f, 0.0f};
        GLint color_unif = glGetUniformLocation(program, "col");
        glUniform3fv(color_unif, 1, col);
        glUniform1i(glGetUniformLocation(program, "texture1"), 0);
        glUniform1i(glGetUniformLocation(program, "texture2"), 1);
        checkOpenGLerror();

		glm::mat4 model_pos = glm::mat4(1.0f);
		model_pos = glm::rotate(model_pos, glm::radians(rotateX), glm::vec3(1, 0, 0));
		model_pos = glm::rotate(model_pos, glm::radians(rotateY), glm::vec3(0, 1, 0));
		model_pos = glm::rotate(model_pos, glm::radians(rotateZ), glm::vec3(0, 0, 1));
        model_pos = glm::translate(model_pos, model.translation);
        setTransform(model_pos);
        setPointLight();
        setMaterial(m_ambient, m_diffuse, m_specular, m_emission, m_shiness);

		glBindVertexArray(model.VAO);
        glDrawElements(GL_TRIANGLES, model.size, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
	glUseProgram(0);
}

void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	view_projection = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
	view_projection *= lookAt(glm::vec3(view_pos[0], view_pos[1], view_pos[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	float m_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float m_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float m_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float m_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	int cur_ind = 0;
	models[0].translation = glm::vec3(light_position[0], light_position[1], light_position[2]);
	drawObject(cur_ind, m_ambient, m_diffuse, m_specular, m_emission, m_shininess);

	if (glIsEnabled(GL_TEXTURE_2D))
		glDisable(GL_TEXTURE_2D);
	glFlush();
	glutSwapBuffers();
}

void updateLight()
{
	light_position[0] = light_radius * cos(light_angle / 180 * PI);
	light_position[1] = light_pos;
	light_position[2] = light_radius * sin(light_angle / 180 * PI);
}

void light_change(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_UP:
			light_pos += 0.5;
			break;
		case GLUT_KEY_DOWN:
			light_pos -= 0.5;
			break;
		case GLUT_KEY_RIGHT:
			light_angle -= 3;
			break;
		case GLUT_KEY_LEFT:
			light_angle += 3;
			break;
		case GLUT_KEY_PAGE_UP:
			light_radius -= 0.5;
			break;
		case GLUT_KEY_PAGE_DOWN:
			light_radius += 0.5;
			break;
		default:
			break;
	}
	updateLight();
	glutPostRedisplay();
}

void keyboard_rotate(unsigned char key, int x, int y)
{
	switch (key) {
	case '0':
		textureMode = 0;
		break;
	case '1':
		textureMode = 1;
		break;
	case '2':
		textureMode = 2;
		break;
	case 'w':
		rotateX -= 2;
		break;
	case 's':
		rotateX += 2;
		break;
	case 'q':
		rotateY -= 2;
		break;
	case 'e':
		rotateY += 2;
		break;
	case 'a':
		rotateZ -= 2;
		break;
	case 'd':
		rotateZ += 2;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void loadModels() {
	auto sphere = loadModel("models/sphere.obj", "textures/sun.jpg", "textures/bricks.jpg", 0.1);
	sphere.translation = glm::vec3(light_position[0], light_position[1], light_position[2]);
	models.push_back(sphere);
	auto floor = loadModel("models/floor.obj", "textures/marble.png", "textures/bricks.jpg", 1.5);
	floor.translation = glm::vec3(0, -3.7, 0);
	models.push_back(floor);
    auto teapot = loadModel("models/teapot.obj", "textures/sun.jpg", "textures/square.jpg", 3);
    teapot.translation = glm::vec3(0, 3.4, 0);
    models.push_back(teapot);
    auto cube = loadModel("models/lamp.obj", "textures/square.jpg", "textures/bricks.jpg", 0.2);
    cube.translation = glm::vec3(-5.5, 3.65f, 0);
	models.push_back(cube);
	auto stol = loadModel("models/stol.obj", "textures/table.png", "textures/square.jpg", 0.3);
	stol.translation = glm::vec3(0, 0, 0);
	models.push_back(stol);
	auto head = loadModel("models/box.obj", "textures/box.png", "textures/bricks.jpg", 0.09);
	head.translation = glm::vec3(12, -3.7, 0);
	models.push_back(head);
	auto vase = loadModel("models/vase.obj", "textures/vase.bmp", "textures/bricks.jpg", 0.15);
	vase.translation = glm::vec3(5, 3.65f, 3);
	models.push_back(vase);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);

	glutInitWindowPosition(100, 100);
	glutCreateWindow("Shaders");
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

	glewInit();
	initShaders();
	loadModels();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	updateLight();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(reshape);
	glutSpecialFunc(light_change);
	glutKeyboardFunc(keyboard_rotate);
	glutMainLoop();
}