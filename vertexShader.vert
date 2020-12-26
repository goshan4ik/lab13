#version 330 core

attribute vec3 coord;
attribute vec3 color;
attribute vec2 texture;
out vec3 var_color;
out vec2 var_texture;
uniform mat4 matrix;

void main() {
 gl_Position = matrix * vec4(coord, 1.0);
 var_color = color;
 var_texture = texture;
}