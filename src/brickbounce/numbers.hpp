#pragma once
float zeroVertices[] = {
  -3.0f, 5.0f,
  -3.0f, -5.0f,
  3.0f, -5.0f,
  -3.0f, 5.0f,
  -1.0f, 2.0f,
  -1.0f, -2.0f,
  1.0f, -2.0f,
  1.0f, 2.0f
};

unsigned int zeroIndices[] = {
  0, 4, 3,
  0, 5, 4,
  0, 1, 5,
  5, 1, 6,
  6, 1, 2,
  7, 6, 2,
  3, 7, 2,
  3, 4, 7
};


float oneVertices[] = {
  -1.0f, 5.0f,
  -1.0f, -5.0f,
  1.0f, -5.0f,
  1.0f, 5.0f
};

unsigned int oneIndices[] = {
  0, 2, 3,
  0, 1, 2
};

float twoVertices[] = {
  -3.0f, 5.0f,
  -3.0f, 3.0f,
  1.0f, 3.0f,
  1.0f, 1.0f,
  -3.0f, 1.0f,
  -3.0f, -5.0f,
  3.0f, -5.0f,
  3.0f, -3.0f,
  -1.0f, -3.0f,
  -1.0f, -1.0f,
  3.0f, -1.0f,
  3.0f, 5.0f
};

unsigned int twoIndices[] = {
  0, 1, 2,
  0, 2, 11,
  11, 2, 10,
  2, 3, 10,
  3, 9, 10,
  4, 9, 3,
  4, 5, 9,
  9, 5, 8,
  8, 5, 7,
  7, 5, 6
};

float sevenVertices[] = {
  -3.0f, 5.0f,
  3.0f, 5.0f,
  3.0f, 3.0f,
  3.0f, -5.0f,
  1.0f, -5.0f,
  1.0f, 3.0f,
  -3.0f, 3.0f
};


unsigned int sevenIndices[] = {
  0, 6, 1,
  1, 6, 2,
  2, 5, 3,
  5, 4, 3
};
