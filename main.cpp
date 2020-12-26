#include <GL\glew.h>
#include <SOIL/SOIL.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include "glUtils.h"
using namespace glm;
using namespace std;

GLuint Program;

GLuint VBO_vertex;
GLuint VBO_color;
GLuint VBO_texture;

GLuint VBO_element;
GLint Indices_count;
mat4 matrix_projection;

int texture;
int mode;

double angle = 0;

void initShader() {
	std::ifstream inVertex("vertexShader.vert");
	std::string vertexShaderText((std::istreambuf_iterator<char>(inVertex)),
		std::istreambuf_iterator<char>());
	const GLchar* vsSource = vertexShaderText.c_str();
	
	std::ifstream inFragment("fragmentShader.frag");
	std::string fragmentShaderText((std::istreambuf_iterator<char>(inFragment)),
		std::istreambuf_iterator<char>());
	const GLchar* fsSource = fragmentShaderText.c_str();

	GLuint vShader, fShader;
	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vsSource, NULL);
	glCompileShader(vShader);

	std::cout << "vertex shader \n";
	shaderLog(vShader);

	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fsSource, NULL);
	glCompileShader(fShader);
	std::cout << "fragment shader \n";
	shaderLog(fShader);

	Program = glCreateProgram();
	glAttachShader(Program, vShader);
	glAttachShader(Program, fShader);
	glLinkProgram(Program);

	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		return;
	}

	checkOpenGLerror();
}

void initVBO()
{
	GLfloat vertices[] = {
	 -1.0f , -1.0f , -1.0f ,
	 1.0f , -1.0f , -1.0f ,
	 1.0f , 1.0f , -1.0f ,
	-1.0f , 1.0f , -1.0f ,
	-1.0f , -1.0f , 1.0f ,
	 1.0f , -1.0f , 1.0f ,
	 1.0f , 1.0f , 1.0f ,
	-1.0f , 1.0f , 1.0f
	};

	GLfloat colors[] = {
		0.0f,  0.980f,  0.604f,
		0.486f, 0.988f, 0.0f,
		0.863f,  0.078f,  0.235f,
		0.545f, 0.0f, 0.0f,
		0.957f, 0.643f, 0.376f,
		0.753f, 0.753f, 0.753f,
		0.184f, 0.310f, 0.310f,
		0.502f, 0.0f, 0.502f,
	};

	GLfloat textureVertices[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
    };

	GLint indices[] = {
		0 , 4 , 5 ,
		0 , 5 , 1 ,
		1 , 5 , 6 ,
		1 , 6 , 2 ,
		2 , 6 , 7 ,
		2 , 7 , 3 ,
		3 , 7 , 4 ,
		3 , 4 , 0 ,
		4 , 7 , 6 ,
		4 , 6 , 5 ,
		3 , 0 , 1 ,
		3 , 1 , 2
	};

	Indices_count = sizeof(indices) / sizeof(indices[0]);

	//! vertex VBO
	glGenBuffers(1, &VBO_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Color VBO
	glGenBuffers(1, &VBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	//texture VBO
    glGenBuffers(1, &VBO_texture);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices), colors, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	checkOpenGLerror();
}

void freeShader()
{
	glUseProgram(0);
	glDeleteProgram(Program);
}

void freeVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO_vertex);
	glDeleteBuffers(1, &VBO_element);
	glDeleteBuffers(1, &VBO_color);
    glDeleteBuffers(1, &VBO_texture);
}
void resizeWindow(int width, int height)
{
	glViewport(0, 0, width, height);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	mat4 View = lookAt(vec3(4, 3, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 rotate_x = { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, glm::cos(angle), -glm::sin(angle), 0.0f,
		0.0f, glm::sin(angle), glm::cos(angle), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

	matrix_projection = Projection * View * rotate_x;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUseProgram(Program);

	auto Unif_matrix = glGetUniformLocation(Program, "matrix");
	glUniformMatrix4fv(Unif_matrix, 1, GL_FALSE, &matrix_projection[0][0]);
	glUniform1i(glGetUniformLocation(Program, "texture1"), 0);

    GLint mode_unif = glGetUniformLocation(Program, "mode");
    glUniform1i(mode_unif, mode);

	//setUniform(Unif_matrix, matrix_projection);
	auto vertexAttribute = getAttributeLocation(Program, "coord");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);
	glEnableVertexAttribArray(vertexAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	glVertexAttribPointer(vertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

	auto colorAttribute = getAttributeLocation(Program, "color");
	glEnableVertexAttribArray(colorAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

	auto textureAttribute = getAttributeLocation(Program, "texture");
    glEnableVertexAttribArray(textureAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture);
    glVertexAttribPointer(textureAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawElements(GL_TRIANGLES, Indices_count, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(vertexAttribute);
	glDisableVertexAttribArray(colorAttribute);
    glDisableVertexAttribArray(textureAttribute);
	glFlush();
	checkOpenGLerror();
	glutSwapBuffers();
	glUseProgram(0);
}

void specialKeys(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_PAGE_UP: angle += 5; break;
		case GLUT_KEY_PAGE_DOWN: angle -= 5; break;
	}
	glutPostRedisplay();
}

void changeMode(unsigned char key, int x, int y)
{
	switch (key)
	{
		case '1': mode = 1; break;
		case '2': mode = 2; break;
		default: mode = 0;
	}
}

int loadTexture(const char* textureName) {
	auto textureId = SOIL_load_OGL_texture(textureName, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	if (textureId == 0) {
		cout << "ERROR: Texture not loader: " << textureName;
	}
	else {
		cout << "Texture was successfully loaded: " << textureName;
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	return textureId;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Colored cube");
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status)
	{
		std::cout << "Error: " << glewGetErrorString(glew_status) << "\n";
		return 1;
	}
	if (!GLEW_VERSION_2_0) {
		std::cout << "No support for OpenGL 2.0 found\n";
		return 1;
	}
	glClearColor(1.0, 1.0, 1.0, 0);

	texture = loadTexture("textures/bricks.jpg");
	initVBO();
	glEnable(GL_TEXTURE_2D);
	initShader();
	glutSpecialFunc(specialKeys);
	glutKeyboardFunc(changeMode);
	glutReshapeFunc(resizeWindow);
	glutIdleFunc(render);
	glutDisplayFunc(render);
	glutMainLoop();
	freeShader();
	freeVBO();
}