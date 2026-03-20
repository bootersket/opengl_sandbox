#version 330 core
in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2D uTex;

uniform mat3 uColorMatrix;
uniform sampler2D lut;

/*==========================================*/
/*============= HELPER FUNCTIONS ===========*/
/*==========================================*/
float srgbToLinear(float c) {
  if (c <= 0.04045) {
    return c / 12.92;
  }
  else {
    return pow((c+0.055)/1.055, 2.4);
  }
}

float linearToSrgb(float c) {
  if (c <= 0.0031308) {
    return c * 12.92;
  }
  else {
    return 1.055*pow(c, 1/2.4) - 0.055;
  }
}

vec3 srgbToLinear(vec3 color) {
  float rL = srgbToLinear(color.r);
  float gL = srgbToLinear(color.g);
  float bL = srgbToLinear(color.b);

  return vec3(rL, gL, bL);
}

vec3 linearToSrgb(vec3 color) {
  float rSrgb = linearToSrgb(color.r);
  float gSrgb = linearToSrgb(color.g);
  float bSrgb = linearToSrgb(color.b);

  return vec3(rSrgb, gSrgb, bSrgb);
}









void main() {
  vec3 color = texture(uTex, vTexCoords).rgb;
  color.r = texture(lut, vec2(color.r, 0.5)).r / 0xff;
  color.g = texture(lut, vec2(color.g, 0.5)).g / 0xff;
  color.b = texture(lut, vec2(color.b, 0.5)).b / 0xff;

  vec3 linear = srgbToLinear(color);

  vec3 corrected = uColorMatrix * linear;
  corrected = clamp(corrected, 0.0, 1.0);

  FragColor = vec4(linearToSrgb(corrected), 1.0);
}

/*
// Basic simple logic to just draw texture
  vec3 color = texture(uTex, vTexCoords).rgb;
  FragColor = vec4(color, 1.0);
*/
