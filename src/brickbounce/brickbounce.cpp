#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "numbers.hpp"

typedef struct {
  float x;
  float y;
} Position;

// todo move to its own file
class RectPosition {
  public:
    RectPosition(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
    float getLeftX() {
      return x;
    }
    float getRightX() {
      return x + width;
    }
    float getBottomY() {
      return y;
    }
    float getTopY() {
      return y + height;
    }
    void setLeftX(float newX) {
      x = newX;
    }
    void setRightX(float newX) {
      x = newX - width;
    }
    void setBottomY(float newY) {
      y = newY;
    }
    void setTopY(float newY) {
      y = newY - height;
    }
    void setCenterX(float newX) {
      x = newX - width/2;
    }
    void incrementX(float value) {
      x += value;
    }
    void decrementX(float value) {
      x -= value;
    }
  private:
    // Origin of object is bottom left
    float x; // left side
    float y; // bottom
    float width;
    float height;
};

#define MOVE_SPEED_X 50.0f
const float playerScale = 1.0f;
const float playerBaseWidth = 5.0f;
const float playerBaseHeight = 1.0f;
RectPosition playerPos(0.0f, 0.0f, playerBaseWidth*playerScale, playerBaseHeight*playerScale);


float deltaTime = 0.0f;
int fpsValue = 0;

float worldWidth;
float worldHeight;
#define WORLD_CENTER_X (worldWidth/2)
#define WORLD_CENTER_Y (worldHeight/2)
const float BALL_RADIUS = 3.0f;
Position ballPos;


bool keyStates[500]; // GLFW keys are 32 - 348

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_Q) glfwSetWindowShouldClose(window, GLFW_TRUE);
}


void updatePlayerPos() {
  if (keyStates[GLFW_KEY_D]) playerPos.incrementX(MOVE_SPEED_X * deltaTime);
  if (keyStates[GLFW_KEY_A]) playerPos.decrementX(MOVE_SPEED_X * deltaTime);

  float minX = 0;
  float maxX = worldWidth;
  float minY = 0;
  float maxY = worldHeight;
  if (playerPos.getRightX() > maxX) playerPos.setRightX(maxX);
  if (playerPos.getLeftX() < minX) playerPos.setLeftX(minX);
}


int ballXDir = 1;
int ballYDir = 1;
float ballSpeed = 50.0f;
#define BALL_BOTTOM_Y (ballPos.y - BALL_RADIUS)
#define BALL_TOP_Y (ballPos.y + BALL_RADIUS)
#define BALL_RIGHT_X (ballPos.x + BALL_RADIUS)
#define BALL_LEFT_X (ballPos.x - BALL_RADIUS)
void updateBallPos() {
  float minX = 0;
  float maxX = worldWidth;
  float minY = 0;
  float maxY = worldHeight;
  if (keyStates[GLFW_KEY_RIGHT] && BALL_RIGHT_X < maxX) {
    ballPos.x += ballSpeed * deltaTime;
  }
  if (keyStates[GLFW_KEY_LEFT] && BALL_LEFT_X > minX) {
    ballPos.x -= ballSpeed * deltaTime;
  }
  if (keyStates[GLFW_KEY_DOWN] && BALL_BOTTOM_Y > minY) {
    ballPos.y -= ballSpeed * deltaTime;
  }
  if (keyStates[GLFW_KEY_UP] && BALL_TOP_Y < maxY) {
    ballPos.y += ballSpeed * deltaTime;
  }

  /* This code makes the ball bounce around without user input */
  // ballPos.x += ballSpeed * deltaTime * ballXDir;
  // ballPos.y += ballSpeed * deltaTime * ballYDir;
  // // todo do some slick math here to calculate exact collisions
  // if (ballPos.x > maxX) ballXDir = -1;
  // if (ballPos.x < -maxX) ballXDir = 1;
  //
  // // ballPos.y += ballSpeed * deltaTime * ballYDir;
  // if (ballPos.y > maxY) ballYDir = -1;
  // if (ballPos.y < -maxY) ballYDir = 1;

  // Check for collision with player
  // std::cout << "playerPos.y: " << playerPos.y << std::endl;
  // std::cout << "ballPos.x: " << ballPos.x << std::endl;
  // todo: fully implement ball player collision
  // if (BALL_BOTTOM_Y <= playerPos.y) { // todo update after rewriting playerPos logic
  //   std::cout << "collide" << std::endl;
  // }
}

void updateKeyStates(GLFWwindow* window) {
  /* Update data structure that keeps track of which keys are currently
   * being pressed for all keys. Other gameplay functions then check
   * these key states to decide what actions should be performed
   * eg. move player, etc. */
  for (int key=0; key<sizeof(keyStates)/sizeof(keyStates[key]); key++) {
    if (glfwGetKey(window, key) == GLFW_PRESS) keyStates[key] = true;
    else if (glfwGetKey(window, key) == GLFW_RELEASE) keyStates[key] = false;
  }
}

void processKeyInput(GLFWwindow* window) {
  updateKeyStates(window);

  updatePlayerPos();
  updateBallPos();
}

void createNumber(float vertices[], int verticesSize, unsigned int indices[], int indicesSize, GLuint &vao, GLuint &vbo, GLuint &ebo) {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

  // glVertexAttribPointer(numberPosAttrLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
  // glEnableVertexAttribArray(numberPosAttrLoc);

}


#define DEG_TO_RAD(degrees) ((degrees) * M_PI / 180.0)
GLuint createBall(int segments) {

  std::vector<float> vertices;

  float aX, aY;
  float bX, bY;

  /* Calculate vertices */
  int degreeDelta = 360/segments;
  for (int deg=0; deg<=360; deg += degreeDelta) {
    aX = std::cos(DEG_TO_RAD(deg));
    aY = std::sin(DEG_TO_RAD(deg));

    bX = std::cos(DEG_TO_RAD(deg + degreeDelta));
    bY = std::sin(DEG_TO_RAD(deg + degreeDelta));

    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    vertices.push_back(aX);
    vertices.push_back(aY);

    vertices.push_back(bX);
    vertices.push_back(bY);
  }
  
  /* Setup vao and vbo */
  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  return vao;
}

std::vector<int> getDigits(int num) {
  /* Returns a vector of digits from num.
   * eg. 123 -> {1, 2, 3}
   * */
  std::vector<int> digits;
  while (num) {
    digits.push_back(num % 10);
    num /= 10;
  }
  std::reverse(digits.begin(), digits.end());

  return digits;
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

  /* Hide window initially so user doesn't see it pop up before the
   * window pos is set (below) */
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  GLFWwindow* window = glfwCreateWindow(width, height, "Brick Bounce", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create window\n";
    glfwTerminate();

    return NULL;
  }
  glfwSetWindowPos(window, 2000, 100);
  glfwShowWindow(window);

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

GLuint createShaderProgram(std::string shaderName) {
  std::string vertexShaderSourceCode;
  std::string fragShaderSourceCode;

  std::ifstream vertexShaderFile;
  std::ifstream fragShaderFile;

  std::stringstream vertexShaderStream;
  std::stringstream fragShaderStream;

  vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  
  try {
    vertexShaderFile.open("src/brickbounce/" + shaderName + ".vert");
    fragShaderFile.open("src/brickbounce/" + shaderName + ".frag");

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
  const int WINDOW_WIDTH = 1200;
  const int WINDOW_HEIGHT = WINDOW_WIDTH / ASPECT_RATIO;

  GLFWwindow* window = createWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
  if (window == NULL) return -1;

  GLuint shaderProgram = createShaderProgram("shader");
  GLint posAttrLoc = glGetAttribLocation(shaderProgram, "aPos");
  int colorUniformLoc = glGetUniformLocation(shaderProgram, "uColor");
  int modelUniformLoc = glGetUniformLocation(shaderProgram, "model");
  int viewUniformLoc = glGetUniformLocation(shaderProgram, "view");
  int projUniformLoc = glGetUniformLocation(shaderProgram, "projection");


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

  GLuint playerVAO, playerVBO, playerEBO;
  glGenVertexArrays(1, &playerVAO);
  glBindVertexArray(playerVAO);

  glGenBuffers(1, &playerVBO);
  glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_STATIC_DRAW);

  glGenBuffers(1, &playerEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(playerIndices), playerIndices, GL_STATIC_DRAW);

  glVertexAttribPointer(posAttrLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
  glEnableVertexAttribArray(posAttrLoc);


  glm::mat4 model = glm::mat4(1.0f);
  // model = glm::rotate(model, glm::radians(13.0f), glm::vec3(1.0f, 0.0f, 1.0f));

  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

  glm::mat4 projection;
  // projection = glm::perspective(glm::radians(45.0f), ASPECT_RATIO, 0.1f, 100.0f);

  worldWidth = 100.0f;
  worldHeight = worldWidth / ASPECT_RATIO;
  projection = glm::ortho(0.0f, worldWidth, 0.0f, worldHeight, -10.0f, 10.0f);
  std::cout << "worldWidth: " << worldWidth << std::endl;
  std::cout << "worldHeight: " << worldHeight << std::endl;

  /* Load uniforms to base shader */
  glUseProgram(shaderProgram);
  glUniformMatrix4fv(modelUniformLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(viewUniformLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projUniformLoc, 1, GL_FALSE, glm::value_ptr(projection));

  /* Create number models */
  NumberModel numberModels[] = {
    NumberModel(0),
    NumberModel(1),
    NumberModel(2),
    NumberModel(3),
    NumberModel(4),
    NumberModel(5),
    NumberModel(6),
    NumberModel(7),
    NumberModel(8),
    NumberModel(9),
  };



  /* Create ball */
  int ballSegments = 24;
  GLuint ballVAO = createBall(ballSegments);

  int fps = 5000; // todo: not that it matters, but setting this to 30 makes the game suuuuper sluggish, like more sluggish than 30fps should be.
  float secPerFrame = 1.0 / fps;
  float lastLoop = 0;
  float lastFrame = 0;

  FPSDisplay fpsDisplay;
  int framesCount = 0;
  float prevFPSUpdate = glfwGetTime();

  glfwSwapInterval(0); // disable vsync


  // Draw cross bars for tinkering
  float crossThickness = 0.100f;
  float width = 10.0f;
  float height = 10.0f;
  float crossVertices[] = {
    // Vertical bar
    crossThickness, worldHeight,
    crossThickness, -worldHeight,
    -crossThickness, -worldHeight,
    -crossThickness, -worldHeight,
    -crossThickness, worldHeight,
    crossThickness, worldHeight,

    // Horizontal bar
    -worldWidth, crossThickness,
    -worldWidth, -crossThickness,
    worldWidth, -crossThickness,

    -worldWidth, crossThickness,
    worldWidth, crossThickness,
    worldWidth, -crossThickness,
  };

  GLuint crossVAO, crossVBO;
  glGenVertexArrays(1, &crossVAO);
  glBindVertexArray(crossVAO);
  glGenBuffers(1, &crossVBO);
  glBindBuffer(GL_ARRAY_BUFFER, crossVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(crossVertices), crossVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(posAttrLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
  glEnableVertexAttribArray(posAttrLoc);

  


  /*====================================
   *            RENDER LOOP
   * ==================================*/
  ballPos.x = WORLD_CENTER_X;
  ballPos.y = WORLD_CENTER_Y;
  playerPos.setBottomY(10.0f);
  playerPos.setCenterX(WORLD_CENTER_X);
  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();

    glClearColor(0.0f, 0.6f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw ball */
    glUniform3f(colorUniformLoc, 0.0f, 0.0f, 0.0f);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(ballPos.x, ballPos.y, 0.0f));
    model = glm::scale(model, glm::vec3(BALL_RADIUS));
    glUniformMatrix4fv(modelUniformLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(ballVAO);
    glDrawArrays(GL_TRIANGLES, 0, ballSegments*3);

    /* Draw player */
    glUseProgram(shaderProgram);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(playerPos.getLeftX(), playerPos.getBottomY(), 0.0f));
    model = glm::scale(model, glm::vec3(playerScale));
    glUniformMatrix4fv(modelUniformLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(colorUniformLoc, 0.0f, 1.0f, 0.5f);

    glBindVertexArray(playerVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    /* Update FPS display */
    // glUseProgram(numberShaderProgram);
    // model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(-2.0f, 1.1f, 0.0f));
    // model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.0f));
    // glUniformMatrix4fv(numberModelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // glUniform3f(colorUniformLoc, 0.0f, 1.1f, 0.1f);
    // fpsDisplay.update(2);

    /* Draw cross */
    glUseProgram(shaderProgram);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(WORLD_CENTER_X, WORLD_CENTER_Y, 0.0f));
    glUniformMatrix4fv(modelUniformLoc, 1, GL_FALSE, glm::value_ptr(model));
    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
    glUniform3fv(colorUniformLoc, 1, glm::value_ptr(color));

    glBindVertexArray(crossVAO);
    glDrawArrays(GL_TRIANGLES, 0, 12);



    /* Update FPS display (without class) */
    // glUseProgram(numberShaderProgram);
    glUseProgram(shaderProgram);
    std::vector<int> digits = getDigits(fpsValue);
    // float x, y;
    float digitOffsetX, digitOffsetY;
    digitOffsetX = 0.125f; // this really should be called digitOffset
wilo: working on getting fps display to work again. refactored some of this to fix vague variable names; tinkering with translate & scale
        values to get something visible. seems like digits are getting drawn over top one another--likely due to digitOffsetX being far too small
        for the new scale we're using.
    float offsetX = 20.0f;
    float offsetY = 10.0f;
    std::cout << "\nfpsValue: " << fpsValue << std::endl;
    for (int i=0; i<digits.size(); i++) {
      std::cout << "digits[i]: " << digits[i];
      model = glm::mat4(1.0f);
      float posX = offsetX + (digitOffsetX * i);
      float posY = offsetY + digitOffsetY * i;
      // model = glm::translate(model, glm::vec3(-2.0f+x*i, 1.1f+y*i, 0.0f));
      model = glm::translate(model, glm::vec3(posX, posY, 0.0f));
      float scale = 1.0f;
      model = glm::scale(model, glm::vec3(scale));
      glUniformMatrix4fv(modelUniformLoc, 1, GL_FALSE, glm::value_ptr(model));
      glUniform3f(colorUniformLoc, 0.6f, 0.0f, 0.6f);
      numberModels[digits[i]].draw();
    }



    processKeyInput(window);


    /* Logic to display at the set FPS */
    double now = glfwGetTime();
    deltaTime = now - lastLoop;
    if ((now - lastFrame) >= secPerFrame) {
      glfwSwapBuffers(window);
      lastFrame = now;

      framesCount++;
    }
    lastLoop = now;

    // todo weird bug where sometimes the fps display will just be single digit
    /* Keep track of actual FPS */
    now = glfwGetTime();
    if (now - prevFPSUpdate >= 1) {
      std::cout << "fps: " << fpsValue << std::endl;
      prevFPSUpdate = now;
      fpsValue = framesCount;
      framesCount = 0;
    }

    
  }

  cleanup(window);
  return 0;
}


/*
 *todo thoughts on restructuring classes & shit:
 - have different types of classes:
    - model/mesh -> has draw() command, but no knowledge of shaders, transforms, etc
    - shader -> methods like use(), setMat4(), etc.
    - renderable/Renderer -> rolls up a model, shader, etc.
 * */

/*
 * Something to think about with C++ classes:

Q: any reason to put the methods outside of the class definition?


A:
Mainly code organization and compilation speed.

Readability / separation of interface vs implementation

Inside the class: good for tiny methods (e.g., getters, setters) or simple one-liners.

Outside the class: keeps the class definition clean, especially if draw() or constructors are long.

Compilation dependencies

Putting definitions in a .cpp file reduces header inclusion bloat. If you change the implementation, other files including the header don’t always need to recompile.

Inlining control

Methods defined inside the class are implicitly inline. Sometimes you don’t want that for bigger functions because it can increase compile time or binary size.*/

/*
Questions/next steps:
- write some sort of pattern parser so I can create an ASCII shape and turn it into a shape to be rendered in the scene
 * Things to do:
 - nvim: remove the auto * in these multiline comments when going to a new line.
 - after you get a decent 2D game going, add a button or something to switch to 3d, and add some cool effects and shit
 * */

