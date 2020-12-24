#include <GL\glew.h>
#include <SOIL/SOIL.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>
#include <vector>
using namespace glm;

using namespace std;


GLuint Program;
GLint Attrib_vertex;
GLint Unif_matrix;
GLint Attrib_color;

GLuint VBO_vertex;
GLuint VBO_color;

GLuint VBO_element;
GLint Indices_count;
mat4 matrix_projection;

double angle = 0;

struct vertex
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

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

void initShader()
{
	const GLchar* vsSource =
		"#version 330 core\n"
		"attribute vec3 coord;\n"
		"attribute vec3 color;\n"
		"varying vec3 var_color;\n"
		"uniform mat4 matrix;\n"
		"void main() {\n"
		" gl_Position = matrix * vec4(coord, 1.0);\n"
		"var_color = color;\n"
		"}\n";
	const GLchar* fsSource =
		"#version 330 core\n"
		"varying vec3 var_color;\n"
		"void main() {\n"
		" gl_FragColor = vec4(var_color, 1.0);\n"
		"}\n";

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

	const char* attr_name = "coord";
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1)
	{
		std::cout << "could not bind attrib " << attr_name << std::endl;
		return;
	}

	attr_name = "color";
	Attrib_color = glGetAttribLocation(Program, attr_name);
	//! ���������� ID �����
	if (Attrib_color == -1)
	{
		std::cout << "could not bind attrib " << attr_name << std::endl;
		return;
	}

	attr_name = "matrix";
	Unif_matrix = glGetUniformLocation(Program, attr_name);

	checkOpenGLerror();
}

//! ������������� VBO
void initVBO()
{
	vertex vertices[] = {
	 -1.0f , -1.0f , -1.0f ,
	 1.0f , -1.0f , -1.0f ,
	 1.0f , 1.0f , -1.0f ,
	-1.0f , 1.0f , -1.0f ,
	-1.0f , -1.0f , 1.0f ,
	 1.0f , -1.0f , 1.0f ,
	 1.0f , 1.0f , 1.0f ,
	-1.0f , 1.0f , 1.0f
	};

	/*float vertices[] = {
		// positions          // colors               // texture coords
		 -1.0f, -1.0f, -1.0f,   0.0f,    0.980f, 0.604f,    0.0f , 0.0f,
		 1.0f, -1.0f, -1.0f,   0.486f,  0.988f, 0.0f,      0.0f , 1.0f,
		 1.0f,  1.0f, -1.0f,   0.863f,  0.078f, 0.235f,    1.0 , 1.0,
		-1.0f,  1.0f, -1.0f,   0.545f,  0.0f,   0.0f,      1.0 , 1.0,
		-1.0f, -1.0f,  1.0f,   0.957f,  0.643f, 0.376f,    0.0 , 0.0,
		 1.0f, -1.0f,  1.0f,   0.753f,  0.753f, 0.753f,    0.0 , 1.0,
		 1.0f,  1.0f,  1.0f,   0.184f,  0.310f, 0.310f,    1.0 , 1.0,
		-1.0f,  1.0f,  1.0f,   0.502f,  0.0f,   0.502f,    1.0 , 1.0
	}; */

	//����� ������
	vertex colors[] = {
		0.0f,  0.980f,  0.604f,
		0.486f, 0.988f, 0.0f,
		0.863f,  0.078f,  0.235f,
		0.545f, 0.0f, 0.0f,
		0.957f, 0.643f, 0.376f,
		0.753f, 0.753f, 0.753f,
		0.184f, 0.310f, 0.310f,
		0.502f, 0.0f, 0.502f,
	};



	//�������
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

	//! �������� ������� � �����
	glGenBuffers(1, &VBO_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//! �������� ����� � �����
	glGenBuffers(1, &VBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	//! �������� ������� � �����
	glGenBuffers(1, &VBO_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	checkOpenGLerror();
}
//! ������������ ��������
void freeShader()
{
	//! ��������� ����, �� ��������� �������� ���������
	glUseProgram(0);
	//! ������� ��������� ���������
	glDeleteProgram(Program);
}
//! ������������ ������
void freeVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO_vertex);
	glDeleteBuffers(1, &VBO_element);
	glDeleteBuffers(1, &VBO_color);
}
void resizeWindow(int width, int height)
{
	glViewport(0, 0, width, height);
}
//! ���������
void render()
{
	//angle += 0.00005;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	mat4 View = lookAt(vec3(4, 3, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 rotate_x = { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, glm::cos(angle), -glm::sin(angle), 0.0f,
		0.0f, glm::sin(angle), glm::cos(angle), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

	matrix_projection = Projection * View * rotate_x;

	//! ������������� ��������� ��������� �������
	glUseProgram(Program);
	//! �������� ������� � ������
	glUniformMatrix4fv(Unif_matrix, 1, GL_FALSE, &matrix_projection[0][0]);

	//setUniform(Unif_matrix, matrix_projection);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);
	//! �������� ������ ���������
	glEnableVertexAttribArray(Attrib_vertex);
	//! ���������� VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	//! �������� pointer 0 ��� ������������ ������, �� ��������� ��� ������ � VBO
	glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(Attrib_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glVertexAttribPointer(Attrib_color, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//! ��������� VBO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawElements(GL_TRIANGLES, Indices_count, GL_UNSIGNED_INT, 0);
	//! ��������� ������ ���������
	glDisableVertexAttribArray(Attrib_vertex);
	glDisableVertexAttribArray(Attrib_color);
	//! ��������� ��������� ���������
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

int loadTexture(const char* textureName) {
	auto textureId = SOIL_load_OGL_texture("data/floor.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	if (textureId == 0) {
		cout << "ERROR: Texture not loader: " << textureName;
	}
	else {
		cout << "Texture was successfully loaded: " << textureName;
	}
	return textureId;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Colored cube");
	//z-�����
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//! ����������� ����� �������������� ��������
	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status)
	{
		//! GLEW �� ���������������������
		std::cout << "Error: " << glewGetErrorString(glew_status) << "\n";
		return 1;
	}
	//! ��������� ����������� OpenGL 2.0
	if (!GLEW_VERSION_2_0)
	{
		//! OpenGl 2.0 ��������� �� ��������
		std::cout << "No support for OpenGL 2.0 found\n";
		return 1;
	}
	glClearColor(1.0, 1.0, 1.0, 0);

	int texture = loadTexture("bricks.jpg");
	glBindTexture(GL_TEXTURE_2D, texture);
	initVBO();
	glEnable(GL_TEXTURE_2D);
	initShader();
	glutSpecialFunc(specialKeys);
	glutReshapeFunc(resizeWindow);
	glutIdleFunc(render);
	glutDisplayFunc(render);
	glutMainLoop();
	//! ������������ ��������
	freeShader();
	freeVBO();
}