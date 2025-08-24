#version 330 core
in vec2 vTexCoords;
in vec4 vPos;
in vec3 vColor;
out vec4 FragColor;

uniform sampler2D crateTexture;


void main() {
  FragColor = texture(crateTexture, vTexCoords);
  // FragColor = vec4(vColor, 1.0);
  vColor;
}
