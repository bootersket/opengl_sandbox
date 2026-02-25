#include "mesh.hpp"

// todo i don't think I like having to pass posAttrLoc into this function... change that.
Mesh meshSetup_player(GLint posAttrLoc) {
  /* Set up player stuff */
  float playerVertices[] = {
    0.0f, playerBaseHeight,
    0.0f, 0.0f,
    playerBaseWidth, 0.0f,
    playerBaseWidth, playerBaseHeight
  };

  unsigned int playerIndices[] = {
    0, 1, 2,
    0, 2, 3
  };

  Mesh mesh;
  glGenVertexArrays(1, &mesh.vao);
  glBindVertexArray(mesh.vao);

  glGenBuffers(1, &mesh.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_STATIC_DRAW);

  glGenBuffers(1, &mesh.ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(playerIndices), playerIndices, GL_STATIC_DRAW);

  glVertexAttribPointer(posAttrLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
  glEnableVertexAttribArray(posAttrLoc);

  return mesh;

}


Mesh meshSetup_brick(GLint posAttrLoc) {
  /* Set up brick stuff */
  float brickVertices[] = {
    0.0f, brickBaseHeight,
    0.0f, 0.0f,
    brickBaseWidth, 0.0f,
    brickBaseWidth, brickBaseHeight
  };
  unsigned int brickIndices[] = {
    0, 1, 2,
    0, 2, 3
  };
  Mesh mesh;
  glGenVertexArrays(1, &mesh.vao);
  glBindVertexArray(mesh.vao);

  glGenBuffers(1, &mesh.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(brickVertices), brickVertices, GL_STATIC_DRAW);

  glGenBuffers(1, &mesh.ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(brickIndices), brickIndices, GL_STATIC_DRAW);
  glVertexAttribPointer(posAttrLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
  glEnableVertexAttribArray(posAttrLoc);

  return mesh;
}

Mesh meshSetup_cross(GLint posAttrLoc) {
  // Draw cross bars for tinkering
  float crossThickness = 0.100f;
  float width = 10.0f;
  float height = 10.0f;
  float crossVertices[] = {
    // todo: worldHeight / worldWidth are variables from brickbounce.cpp so wasn't sure how to reconcile that.
    // For now, just using really big values. Not like the cross is a core part of the game anyways; it's really just
    // for development purposes.

    // // Vertical bar
    // crossThickness, worldHeight,
    // crossThickness, -worldHeight,
    // -crossThickness, -worldHeight,
    // -crossThickness, -worldHeight,
    // -crossThickness, worldHeight,
    // crossThickness, worldHeight,
    //
    // // Horizontal bar
    // -worldWidth, crossThickness,
    // -worldWidth, -crossThickness,
    // worldWidth, -crossThickness,
    //
    // -worldWidth, crossThickness,
    // worldWidth, crossThickness,
    // worldWidth, -crossThickness,

    // Vertical bar
    crossThickness, 5000,
    crossThickness, -5000,
    -crossThickness, -5000,
    -crossThickness, -5000,
    -crossThickness, 5000,
    crossThickness, 5000,

    // Horizontal bar
    -5000, crossThickness,
    -5000, -crossThickness,
    5000, -crossThickness,

    -5000, crossThickness,
    5000, crossThickness,
    5000, -crossThickness,
  };

  // GLuint crossVAO, crossVBO;
  Mesh mesh;
  glGenVertexArrays(1, &mesh.vao);
  glBindVertexArray(mesh.vao);
  glGenBuffers(1, &mesh.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(crossVertices), crossVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(posAttrLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
  glEnableVertexAttribArray(posAttrLoc);

  return mesh;

}











