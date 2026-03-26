#version 330 core
in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2D uTex;

uniform mat3 uColorMatrix;
uniform sampler2D preLut;
uniform sampler2D postLut;
uniform float gammaExponent;
uniform bool useLut;
uniform float blackPointValue;

/*==========================================*/
/*============= HELPER FUNCTIONS ===========*/
/*==========================================*/
float srgbToLinear(float c) {
  // if (c <= 0.04045) {
  //   return c / 12.92;
  // }
  // else {
  //   return pow((c+0.055)/1.055, 2.4);
  // }
  return pow(c, 2.4);
}

float linearToSrgb(float c) {
  // if (c <= 0.0031308) {
  //   return c * 12.92;
  // }
  // else {
  //   return 1.055*pow(c, 1/2.4) - 0.055;
  // }
  return pow(c, 1/2.4);
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

vec3 srgbToLinear(vec3 color, float value) {
  float rL = pow(color.r, value);
  float gL = pow(color.g, value);
  float bL = pow(color.b, value);

  return vec3(rL, gL, bL);
}

vec3 linearToSrgb(vec3 color, float value) {
  float rSrgb = pow(color.r, 1/value);
  float gSrgb = pow(color.g, 1/value);
  float bSrgb = pow(color.b, 1/value);

  return vec3(rSrgb, gSrgb, bSrgb);
}


vec3 blackPointCorrect(vec3 color) {
  return (color - blackPointValue) / (1 - blackPointValue);
}




void main() {
  vec3 color = texture(uTex, vTexCoords).rgb;

  if (useLut) {
    color.r = texture(preLut, vec2(color.r, 0.5)).r;
    color.g = texture(preLut, vec2(color.g, 0.5)).g;
    color.b = texture(preLut, vec2(color.b, 0.5)).b;
  }
  else {
    color = srgbToLinear(color);
  }


  color = blackPointCorrect(color);
  color = uColorMatrix * color;
  color = clamp(color, 0.0, 1.0);

  if (useLut) {
    color.r = texture(postLut, vec2(color.r, 0.5)).r;
    color.g = texture(postLut, vec2(color.g, 0.5)).g;
    color.b = texture(postLut, vec2(color.b, 0.5)).b;
  }
  else {
    color = linearToSrgb(color);
  }

  FragColor = vec4(color, 1.0);


}

/*
// Basic simple logic to just draw texture
  vec3 color = texture(uTex, vTexCoords).rgb;
  FragColor = vec4(color, 1.0);
*/
