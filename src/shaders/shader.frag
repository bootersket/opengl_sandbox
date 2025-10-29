#version 330 core
in vec2 vTexCoord;
in vec4 vPos;
in vec3 vColor;
out vec4 FragColor;

uniform sampler2D ourTexture;


void main() {
  // FragColor = vec4(vColor, 1.0);
  FragColor = texture(ourTexture, vTexCoord);
}
