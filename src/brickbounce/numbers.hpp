
#pragma once
float vertices0[] = {
  -3.0f, 5.0f,
  -3.0f, -5.0f,
  3.0f, -5.0f,
  3.0f, 5.0f,
  -1.0f, 2.0f,
  -1.0f, -2.0f,
  1.0f, -2.0f,
  1.0f, 2.0f
};
unsigned int indices0[] = {
  0, 4, 3,
  0, 5, 4,
  0, 1, 5,
  5, 1, 6,
  6, 1, 2,
  7, 6, 2,
  3, 7, 2,
  3, 4, 7
};


float vertices1[] = {
  -1.0f, 5.0f,
  -1.0f, -5.0f,
  1.0f, -5.0f,
  1.0f, 5.0f
};

unsigned int indices1[] = {
  0, 2, 3,
  0, 1, 2
};

float vertices2[] = {
  -3.0f, 5.0f,
  -3.0f, 3.0f,
  1.0f, 3.0f,
  1.0f, 1.0f,
  -3.0f, 1.0f,
  -3.0f, -5.0f,
  3.0f, -5.0f,
  3.0f, -3.0f,
  -1.0f, -3.0f, -1.0f, -1.0f,
  3.0f, -1.0f,
  3.0f, 5.0f
};

unsigned int indices2[] = {
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

float vertices3[] = {
  -3.0f, 5.0f,
  -3.0f, 3.0f,
  1.0f, 3.0f,
  1.0f, 1.0f,
  -3.0f, 1.0f,
  -3.0f, -1.0f,
  1.0f, -1.0f,
  1.0f, -3.0f,
  -3.0f, -3.0f,
  -3.0f, -5.0f,
  3.0f, -5.0f,
  3.0f, 5.0f
};

unsigned int indices3[] = {
  0, 1, 2,
  0, 2, 11,
  11, 2, 10,
  2, 7, 10,
  3, 5, 6,
  4, 5, 3,
  8, 9, 7,
  7, 9, 10
};

float vertices4[] = {
  -1.0f, 5.0f,
  -3.0f, 5.0f,
  -3.0f, -1.0f,
  1.0f, -1.0f,
  1.0f, -5.0f,
  3.0f, -5.0f,
  3.0f, 5.0f,
  1.0f, 5.0f,
  1.0f, 1.0f,
  -1.0f, 1.0f,
};
unsigned int indices4[] = {
  1, 2, 0,
  0, 2, 9,
  9, 2, 3,
  9, 3, 8,
  3, 4, 5,
  6, 3, 5,
  6, 8, 3,
  7, 8, 6
};

float vertices5[] = {
  -3.0f, 5.0f,
  -3.0f, -1.0f,
  1.0f, -1.0f,
  1.0f, -3.0f,
  -3.0f, -3.0f,
  -3.0f, -5.0f,
  3.0f, -5.0f,
  3.0f, 1.0f,
  -1.0f, 1.0f,
  -1.0f, 3.0f,
  3.0f, 3.0f,
  3.0f, 5.0f
};
unsigned int indices5[] = {
  11, 9, 10,
  0, 9, 11,
  0, 1, 9,
  9, 1, 8,
  8, 1, 2,
  8, 2, 7,
  7, 2, 6,
  2, 3, 6,
    3, 5, 6,
  4, 5, 3
};

float vertices6[] = {
  3.0f, 5.0f,
  -3.0f, 5.0f,
  -3.0f, -5.0f,
  3.0f, -5.0f,
  3.0f, 1.0f,
  -1.0f, 1.0f,
  -1.0f, 3.0f,
  3.0f, 3.0f,
  -1.0f, -1.0f,
  -1.0f, -3.0f,
  1.0f, -3.0f,
  1.0f, -1.0f,
};
unsigned int indices6[] = {
  0, 6, 7,
  1, 6, 0,
  1, 2, 6,
  6, 2, 5,
  5, 2, 9,
  9, 2, 10,
  10, 2, 3,
  11, 10, 3,
  4, 11, 3,
  4, 8, 11,
  5, 8, 4
};

float vertices7[] = {
  -3.0f, 5.0f,
  3.0f, 5.0f,
  3.0f, 3.0f,
  3.0f, -5.0f,
  1.0f, -5.0f,
  1.0f, 3.0f,
  -3.0f, 3.0f
};
unsigned int indices7[] = {
  0, 6, 1,
  1, 6, 2,
  2, 5, 3,
  5, 4, 3
};

float vertices8[] = {
  3.0f, 5.0f,
  -3.0f, 5.0f,
  -3.0f, -5.0f,
  3.0f, -5.0f,
  -1.0f, 3.0f,
  -1.0f, 1.0f,
  1.0f, 1.0f,
  1.0f, 3.0f,
  -1.0f, -1.0f,
  -1.0f, -3.0f,
  1.0f, -3.0f,
  1.0f, -1.0f
};
unsigned int indices8[] = {
  1, 4, 0,
  1, 5, 4,
  1, 2, 5,
  5, 2, 9,
  9, 2, 3,
  9, 3, 10,
  6, 10, 3,
  0, 6, 3,
  0, 7, 6,
  0, 4, 7,
  5, 8, 11,
  5, 11, 6
};

float vertices9[] = {
  -3.0f, 5.0f,
  -3.0f, -1.0f,
  1.0f, -1.0f,
  1.0f, -5.0f,
  3.0f, -5.0f,
  3.0f, 5.0f,
  -1.0f, 3.0f,
  -1.0f, 1.0f,
  1.0f, 1.0f,
  1.0f, 3.0f
};
unsigned int indices9[] = {
  0, 6, 5,
  0, 1, 6,
  6, 1, 7,
  7, 1, 8,
  8, 1, 2,
  9, 3, 4,
  5, 9, 4,
  5, 6, 9
};

float* allVertices[] = {
  vertices0,
  vertices1,
  vertices2,
  vertices3,
  vertices4,
  vertices5,
  vertices6,
  vertices7,
  vertices8,
  vertices9
};

std::size_t allVerticesSizes[] = {
  sizeof(vertices0),
  sizeof(vertices1),
  sizeof(vertices2),
  sizeof(vertices3),
  sizeof(vertices4),
  sizeof(vertices5),
  sizeof(vertices6),
  sizeof(vertices7),
  sizeof(vertices8),
  sizeof(vertices9)
};

unsigned int* allIndices[] = {
  indices0,
  indices1,
  indices2,
  indices3,
  indices4,
  indices5,
  indices6,
  indices7,
  indices8,
  indices9
};
std::size_t allIndicesSizes[] = {
  sizeof(indices0),
  sizeof(indices1),
  sizeof(indices2),
  sizeof(indices3),
  sizeof(indices4),
  sizeof(indices5),
  sizeof(indices6),
  sizeof(indices7),
  sizeof(indices8),
  sizeof(indices9)
};



class NumberModel {
  public:
    NumberModel(int num) {
      float* vertices = allVertices[num];
      size_t verticesSize = allVerticesSizes[num];
      unsigned int* indices = allIndices[num];
      size_t indicesSize = allIndicesSizes[num];
      *this = NumberModel(vertices, verticesSize, indices, indicesSize);
    }
    NumberModel(float vertices[], int verticesSize, unsigned int indices[], int indicesSize) {
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

      glGenBuffers(1, &ebo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);

      numElements = indicesSize / sizeof(indices[0]);
      std::cout << "constructor" << std::endl;
    }
    void draw() {
      glBindVertexArray(vao);
      glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
    }
    GLuint getVAO() {
      return vao;
    }
  private:
    GLuint vao, vbo, ebo;
    unsigned int numElements;
};


