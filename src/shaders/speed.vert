#version 330 core
layout(location = 0) in vec3 aPos;

uniform float uX;
uniform float uY;

void main() {
  gl_Position = vec4(aPos.x + uX, aPos.y + uY, aPos.z, 1.0);
}
