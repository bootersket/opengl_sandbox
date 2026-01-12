#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define MOVE_SPEED_X 5.0f
float xPos = 0.0f;
float yPos = 0.0f;
float deltaTime = 0.0f;

bool keyStates[500]; // GLFW keys are 32 - 348

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_Q) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void updateKeyState(GLFWwindow* window, int key) {
  if (glfwGetKey(window, key) == GLFW_PRESS) {
    keyStates[key] = true;
  }
  else if (glfwGetKey(window, key) == GLFW_RELEASE) {
    keyStates[key] = false;
  }
}

void updatePos() {
  if (keyStates[GLFW_KEY_D]) xPos += MOVE_SPEED_X * deltaTime;
  if (keyStates[GLFW_KEY_A]) xPos -= MOVE_SPEED_X * deltaTime;
  if (keyStates[GLFW_KEY_W]) yPos += MOVE_SPEED_X * deltaTime;
  if (keyStates[GLFW_KEY_S]) yPos -= MOVE_SPEED_X * deltaTime;
  xPos = std::min(2.1f, xPos);
  xPos = std::max(-2.1f, xPos);
  yPos = std::min(1.2f, yPos);
  yPos = std::max(-1.2f, yPos);
}

bool dDown = false;
void processKeyInput(GLFWwindow* window) {
  updateKeyState(window, GLFW_KEY_D);
  updateKeyState(window, GLFW_KEY_A);
  updateKeyState(window, GLFW_KEY_W);
  updateKeyState(window, GLFW_KEY_S);

  updatePos();


}

void cleanup(GLFWwindow* window) {
  glfwDestroyWindow(window);
  glfwTerminate();
}

GLFWwindow* createWindow(int width, int height) {
  if (!glfwInit()) {
    std::cerr << "Failed to init GLFW\n";
    return NULL;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


  GLFWwindow* window = glfwCreateWindow(width, height, "Brick Bounce", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create window\n";
    glfwTerminate();

    return NULL;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return NULL;
  }
  glViewport(0, 0, width, height);

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetKeyCallback(window, keyCallback);

  return window;
}

enum {
  VERTEX,
  FRAGMENT,
  PROGRAM
};
void checkForShaderCompileErrors(GLuint shader, int shaderType) {
  char infoLog[512];
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::string shaderTypeStr;
    if (shaderType == VERTEX) shaderTypeStr = "VERTEX";
    else if (shaderType == FRAGMENT) shaderTypeStr = "FRAGMENT";
    else if (shaderType == PROGRAM) shaderTypeStr = "PROGRAM";
    std::cout << "ERROR::SHADER::" << shaderTypeStr << "::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
}

GLuint createShaderProgram() {
  std::string vertexShaderSourceCode;
  std::string fragShaderSourceCode;

  std::ifstream vertexShaderFile;
  std::ifstream fragShaderFile;

  std::stringstream vertexShaderStream;
  std::stringstream fragShaderStream;

  vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  
  try {
    vertexShaderFile.open("src/brickbounce/shader.vert");
    fragShaderFile.open("src/brickbounce/shader.frag");

    // Read file's buffer contents into stream
    vertexShaderStream << vertexShaderFile.rdbuf();
    fragShaderStream << fragShaderFile.rdbuf();

    // Close file handlers
    vertexShaderFile.close();
    fragShaderFile.close();

    // Convert stream into string
    vertexShaderSourceCode = vertexShaderStream.str();
    fragShaderSourceCode = fragShaderStream.str();
  }
  catch(std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }

  // Grab c_str since that's what glShaderSource() expects
  const char* vertexShaderSourceCodeCStr = vertexShaderSourceCode.c_str();
  const char* fragShaderSourceCodeCStr = fragShaderSourceCode.c_str();

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSourceCodeCStr, NULL);
  glCompileShader(vertexShader);
  checkForShaderCompileErrors(vertexShader, VERTEX);

  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, &fragShaderSourceCodeCStr, NULL);
  glCompileShader(fragShader);
  checkForShaderCompileErrors(fragShader, FRAGMENT);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragShader);
  glLinkProgram(program);
  checkForShaderCompileErrors(program, PROGRAM);

  glDeleteShader(vertexShader);
  glDeleteShader(fragShader);

  return program;
}

int main() {
  float ASPECT_RATIO = 16.0/9.0;
  const int WINDOW_WIDTH = 800;
  const int WINDOW_HEIGHT = WINDOW_WIDTH / ASPECT_RATIO;

  GLFWwindow* window = createWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
  if (window == NULL) return -1;

  GLuint shaderProgram = createShaderProgram();
  GLint posAttrLoc = glGetAttribLocation(shaderProgram, "aPos");
  GLint colorAttrLoc = glGetAttribLocation(shaderProgram, "aColor");

  float playerVertices[] = {
    -0.5f, 0.1f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.1f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.1f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.1f, 0.0f, 1.0f, 0.0f
  };

  unsigned int playerIndices[] = {
    0, 1, 2,
    0, 2, 3
  };

  GLuint playerVAO, playerVBO, playerEBO;
  glGenVertexArrays(1, &playerVAO);
  glBindVertexArray(playerVAO);

  glGenBuffers(1, &playerVBO);
  glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_STATIC_DRAW);

  glGenBuffers(1, &playerEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(playerIndices), playerIndices, GL_STATIC_DRAW);

  glVertexAttribPointer(posAttrLoc, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
  glEnableVertexAttribArray(posAttrLoc);

  glVertexAttribPointer(colorAttrLoc, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
  glEnableVertexAttribArray(colorAttrLoc);

  float sevenVertices[] = {
    -.3f, .5f, 1.0f, 0.0f, 0.0f,
    .3f, .5f, 1.0f, 0.0f, 0.0f,
    .3f, .3f, 1.0f, 0.0f, 0.0f,
    .3f, -.5f, 1.0f, 0.0f, 0.0f,
    .1f, -.5f, 1.0f, 0.0f, 0.0f,
    .1f, .3f, 1.0f, 0.0f, 0.0f,
    -.3f, .3f, 1.0f, 0.0f, 0.0f
  };


  unsigned int sevenIndices[] = {
    0, 6, 1,
    1, 6, 2,
    2, 5, 3,
    5, 4, 3
  };

  GLuint sevenVAO, sevenVBO, sevenEBO;

  glGenVertexArrays(1, &sevenVAO);
  glBindVertexArray(sevenVAO);

  glGenBuffers(1, &sevenVBO);
  glBindBuffer(GL_ARRAY_BUFFER, sevenVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(sevenVertices), sevenVertices, GL_STATIC_DRAW);

  glGenBuffers(1, &sevenEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sevenEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sevenIndices), sevenIndices, GL_STATIC_DRAW);


  glVertexAttribPointer(posAttrLoc, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
  glEnableVertexAttribArray(posAttrLoc);

  glVertexAttribPointer(colorAttrLoc, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
  glEnableVertexAttribArray(colorAttrLoc);

  glm::mat4 model = glm::mat4(1.0f);
  // model = glm::rotate(model, glm::radians(13.0f), glm::vec3(1.0f, 0.0f, 1.0f));

  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

  glm::mat4 projection;
  projection = glm::perspective(glm::radians(45.0f), ASPECT_RATIO, 0.1f, 100.0f);
  // todo try ortho?

  int modelLoc = glGetUniformLocation(shaderProgram, "model");
  int viewLoc = glGetUniformLocation(shaderProgram, "view");
  int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

  glUseProgram(shaderProgram);

  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

  int fps = 90; // todo: not that it matters, but setting this to 30 makes the game suuuuper sluggish, like more sluggish than 30fps should be.
  float secPerFrame = 1.0 / fps;
  float lastLoop = 0;
  float lastFrame = 0;
  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();

    glClearColor(0.0f, 0.6f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw number/text (WIP)
    glBindVertexArray(sevenVAO);
    // model = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, 0.0f, 0.0f));
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    // todo

    glUseProgram(shaderProgram);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, yPos, 0.0f));
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(playerVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);




    processKeyInput(window);
    double now = glfwGetTime();
    deltaTime = now - lastLoop;
    if ((now - lastFrame) >= secPerFrame) {
      glfwSwapBuffers(window);
      lastFrame = now;
    }
    lastLoop = now;
    
  }

  cleanup(window);
  return 0;
}

/*
WILO: got the seven to show up!!!
Questions/next steps:
- scale it down and position it.
- expand functionality to all numbers (make new file for this?)
- ?? why is the color 1. blue and 2. a weird fade from black to blue?
 * Things to do:
 - nvim: remove the auto * in these multiline comments when going to a new line.
 - keep rect from going off screen
 - after you get a decent 2D game going, add a button or something to switch to 3d, and add some cool effects and shit
 * */

