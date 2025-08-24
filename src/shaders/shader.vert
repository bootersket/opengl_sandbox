#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoords;

out vec4 vPos;
out vec3 vColor;
out vec2 vTexCoords;

uniform float uOffsetX;
uniform float uOffsetY;

uniform mat4 transform;

void main() {
  gl_Position = transform * vec4(aPos.x + uOffsetX, aPos.y + uOffsetY, aPos.z, 1.0);
  vPos = gl_Position;
  vTexCoords = aTexCoords;
  vColor = aColor;
}
