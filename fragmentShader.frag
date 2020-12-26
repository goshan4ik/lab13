#version 330 core

in vec3 var_color;
in vec2 var_texture;
uniform int mode;

uniform sampler2D texture1;

void main() {
  if (mode == 1) {
   gl_FragColor = texture(texture1, var_texture);
  } else if (mode == 2) {
   gl_FragColor = mix(texture(texture1, var_texture), vec4(var_color, 1.0), 0.5);
  } else {
   gl_FragColor = vec4(var_color, 1.0);
  }
}