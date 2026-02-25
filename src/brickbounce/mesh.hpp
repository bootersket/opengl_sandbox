#include <glad/glad.h>
#include <GLFW/glfw3.h>

const float playerScale = 1.0f;
const float playerBaseWidth = 5.0f;
const float playerBaseHeight = 1.0f;

const float brickScale = 1.0f;
const float brickBaseWidth = 5.0f;
const float brickBaseHeight = 2.0f;

class Mesh {
  public:
    // todo: in all honesty I don't think the vbo and ebo are even needed after initial vertex setup.
    // Which begs the question: is this Mesh class even needed? I think at this stage, no. But eventually
    // it would be nice to have funcs like mesh.draw(), etc.
    // Either way, leaving it for now as it's still an improvement over the previous version of the code.
    GLuint vbo;
    GLuint vao;
    GLuint ebo;
};

Mesh meshSetup_player(GLint posAttrLoc);
Mesh meshSetup_brick(GLint posAttrLoc);
Mesh meshSetup_cross(GLint posAttrLoc);
