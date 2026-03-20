#version 330 core
in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2D uTex;

void main() {
// Basic simple logic to just draw texture
  vec3 color = texture(uTex, vTexCoords).rgb;
  FragColor = vec4(color, 1.0);
}

