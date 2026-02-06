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


#define MOVE_SPEED_X 50.0f
float playerXPos = 0.0f;
float playerYPos = 10.0f;
float deltaTime = 0.0f;
int num = 0;
int fpsValue = 0;

float worldWidth;
float worldHeight;
#define WORLD_CENTER_X (worldWidth/2)
#define WORLD_CENTER_Y (worldHeight/2)
float BALL_RADIUS = 3.0f;
float ballXPos = WORLD_CENTER_X;
float ballYPos = WORLD_CENTER_Y;


float SCREEN_WIDTH = 100.0f;
float SCREEN_HEIGHT = 100.0f;



bool keyStates[500]; // GLFW keys are 32 - 348

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_Q) glfwSetWindowShouldClose(window, GLFW_TRUE);
}


void updatePlayerPos() {
  // todo change this check to a func/macro called like keyPressed?
  if (keyStates[GLFW_KEY_D]) playerXPos += MOVE_SPEED_X * deltaTime;
  if (keyStates[GLFW_KEY_A]) playerXPos -= MOVE_SPEED_X * deltaTime;
  // todo get more robust way to keep player in bounds of screen
  playerXPos = std::min(SCREEN_WIDTH/2, playerXPos);
  playerXPos = std::max(-SCREEN_WIDTH/2, playerXPos);
}

int ballXDir = 1;
int ballYDir = 1;
float ballSpeed = 50.0f;
#define BALL_BOTTOM_Y (ballYPos - BALL_RADIUS)
#define BALL_TOP_Y (ballYPos + BALL_RADIUS)
#define BALL_RIGHT_X (ballXPos + BALL_RADIUS)
#define BALL_LEFT_X (ballXPos - BALL_RADIUS)
void updateBallPos() {
  float minX = 0;
  float maxX = worldWidth;
  float minY = 0;
  float maxY = worldHeight;
  if (keyStates[GLFW_KEY_RIGHT] && BALL_RIGHT_X < maxX) {
    ballXPos += ballSpeed * deltaTime;
  }
  if (keyStates[GLFW_KEY_LEFT] && BALL_LEFT_X > minX) {
    ballXPos -= ballSpeed * deltaTime;
  }
  if (keyStates[GLFW_KEY_DOWN] && BALL_BOTTOM_Y > minY) {
    ballYPos -= ballSpeed * deltaTime;
  }
  if (keyStates[GLFW_KEY_UP] && BALL_TOP_Y < maxY) {
    ballYPos += ballSpeed * deltaTime;
  }

  /* This code makes the ball bounce around without user input */
  // ballXPos += ballSpeed * deltaTime * ballXDir;
  // ballYPos += ballSpeed * deltaTime * ballYDir;
  // // todo do some slick math here to calculate exact collisions
  // if (ballXPos > maxX) ballXDir = -1;
  // if (ballXPos < -maxX) ballXDir = 1;
  //
  // // ballYPos += ballSpeed * deltaTime * ballYDir;
  // if (ballYPos > maxY) ballYDir = -1;
  // if (ballYPos < -maxY) ballYDir = 1;

  // Check for collision with player
  // std::cout << "playerYPos: " << playerYPos << std::endl;
  // std::cout << "ballXPos: " << ballXPos << std::endl;
// wilo: 2/4 refactoring some ball stuff, added BALL_BOTTOM_Y, working on ball & player collision logic. need to implement the math
//         for the size of the player rect; is there currently a player width/height anywhere? That would be useful here so
//           that we can calculate if the ball is "within" the rect. Because we don't currently have that, the current collision logic
//             is a bit imprecise (i.e. collide not detected until the ball is a few pixels inside the rect)
  // wilp 2/5: refactored such that we now define a width and height for the player; perhaps need to store these in globals so I can access them
  // for collision logic tho, so keep working on that. 
  // Last thing I did was remove color attrib from player logic (which involved removing it from the shaders, but this is a general purpose shader
  // that the cross also uses so now it's green). Update the shader to take a color as a uniform and make the bars black but the player green.
  if (BALL_BOTTOM_Y <= playerYPos) {
    std::cout << "collide" << std::endl;
  }
}
void updateNum() {
  if (keyStates[GLFW_KEY_0]) num = 0;
  if (keyStates[GLFW_KEY_1]) num = 1;
  if (keyStates[GLFW_KEY_2]) num = 2;
  if (keyStates[GLFW_KEY_3]) num = 3;
  if (keyStates[GLFW_KEY_4]) num = 4;
  if (keyStates[GLFW_KEY_5]) num = 5;
  if (keyStates[GLFW_KEY_6]) num = 6;
  if (keyStates[GLFW_KEY_7]) num = 7;
  if (keyStates[GLFW_KEY_8]) num = 8;
  if (keyStates[GLFW_KEY_9]) num = 9;
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
  updateNum();
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
  // GLint colorAttrLoc = glGetAttribLocation(shaderProgram, "aColor");

  GLuint numberShaderProgram = createShaderProgram("number");
  GLint numberPosAttrLoc = glGetAttribLocation(numberShaderProgram, "aPos");

  // todo 2/5:
  // remove color attribute from player vertex data. why is that even there? just use a uniform or something
  float playerBaseWidth = 5.0f;
  float playerBaseHeight = 1.0f;
  float playerVertices[] = {
    // 0.0f, playerBaseHeight,        0.0f, 1.0f, 0.0f,
    // 0.0f, 0.0f,                0.0f, 1.0f, 0.0f,
    // playerBaseWidth, 0.0f,         0.0f, 1.0f, 0.0f,
    // playerBaseWidth, playerBaseHeight, 0.0f, 1.0f, 0.0f,

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

  // glVertexAttribPointer(colorAttrLoc, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
  // glEnableVertexAttribArray(colorAttrLoc);



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

  int modelLoc = glGetUniformLocation(shaderProgram, "model");
  int viewLoc = glGetUniformLocation(shaderProgram, "view");
  int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

  /* Load uniforms to number shader */
  int numberModelLoc = glGetUniformLocation(numberShaderProgram, "model");
  int numberViewLoc = glGetUniformLocation(numberShaderProgram, "view");
  int numberProjectionLoc = glGetUniformLocation(numberShaderProgram, "projection");
  int colorUniformLoc = glGetUniformLocation(numberShaderProgram, "uColor");
  glUseProgram(numberShaderProgram);
  glUniform3f(colorUniformLoc, 1.0f, 0.1f, 0.1f);
  glUniformMatrix4fv(numberModelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(numberViewLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(numberProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

  /* Load uniforms to base shader */
  glUseProgram(shaderProgram);
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

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
  // glUseProgram(shaderProgram);
  glVertexAttribPointer(posAttrLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
  glEnableVertexAttribArray(posAttrLoc);
  // glVertexAttribPointer(colorAttrLoc);

  


  /*====================================
   *            RENDER LOOP
   * ==================================*/
  ballXPos = WORLD_CENTER_X;
  ballYPos = WORLD_CENTER_Y;
  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();

    glClearColor(0.0f, 0.6f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // /* Draw number/text */
    // glUseProgram(numberShaderProgram);
    // glUniform3f(colorUniformLoc, 1.0f, 0.1f, 0.1f);
    // // model = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, 0.0f, 0.0f));
    // model = glm::mat4(1.0f);
    // model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.0f));
    // glUniformMatrix4fv(numberModelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // glUniform3f(colorUniformLoc, 1.0f, 0.1f, 0.1f);
    // numberModels[num].draw();

    /* Draw ball */
    glUniform3f(colorUniformLoc, 0.0f, 0.0f, 0.0f);
    // model = glm::translate(glm::mat4(1.0f), glm::vec3(WORLD_CENTER_X + ballXPos, WORLD_CENTER_Y + ballYPos, 0.0f));
    model = glm::translate(glm::mat4(1.0f), glm::vec3(ballXPos, ballYPos, 0.0f));
    model = glm::scale(model, glm::vec3(BALL_RADIUS));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(ballVAO);
    glDrawArrays(GL_TRIANGLES, 0, ballSegments*3);

    /* Draw player */
    glUseProgram(shaderProgram);
    float playerScale = 1.0f;
    model = glm::translate(glm::mat4(1.0f), glm::vec3(WORLD_CENTER_X + playerXPos, playerYPos, 0.0f));
    model = glm::scale(model, glm::vec3(playerScale));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

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
    // model = glm::mat4(1.0f);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(WORLD_CENTER_X, WORLD_CENTER_Y, 0.0f));
// WILO: thought I couldn't get the cross bar, but I can; just that the thickness was so low that it wasn't appearing for the vertical line.
//         Now that they both appear, i can see one appears thicker than the other. Why? obviously has to do with the differening ratio of worldHeight
//         and worldWidth. But dive into it more and get a better understanding.
    // model = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 23.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(crossVAO);
    glDrawArrays(GL_TRIANGLES, 0, 12);



    /* Update FPS display (without class) */
    glUseProgram(numberShaderProgram);
    std::vector<int> digits = getDigits(fpsValue);
    float x, y;
    x = 0.125f;
    for (int i=0; i<digits.size(); i++) {
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-2.0f+x*i, 1.1f+y*i, 0.0f));
      model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.0f));
      glUniformMatrix4fv(numberModelLoc, 1, GL_FALSE, glm::value_ptr(model));
      glUniform3f(colorUniformLoc, 0.6f, 0.0f, 0.6f);
      numberModels[digits[i]].draw();
    }



    processKeyInput(window);
    updateBallPos();


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

// wilo: just as a random technical challenge, implement some logic to "zoom in" to the point where the cursor is. (perhaps much more challenging than
//           it seems)

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
WILO: got the one and two show up!! code is pretty gross, but just working on making it work for now. Next steps:
could implement all numbers. but I think I understand it well enough to pivot towards making this functionality
less dogshit and more DRY (like mentioned below, make a class for numbers that encapsulates the relevant data.)
Ideally some sort of draw function like drawNumber(1) would draw 1 (and of course implement it for double digits
after single digits is working, etc).
Then worry about positioning, scaling, etc.
Then hooking it up to the FPS calculated via the render loop
Questions/next steps:
- Refactor idea: make a class that encapsulates a single number (vertices, indices, vao, vbo, ebo, shader (??questionable))
- scale it down and position it.
- expand functionality to all numbers (make new file for this?)
- remove colors from numbers vertices
- write some sort of pattern parser so I can create an ASCII shape and turn it into a shape to be rendered in the scene
 * Things to do:
 - nvim: remove the auto * in these multiline comments when going to a new line.
 - keep rect from going off screen
 - after you get a decent 2D game going, add a button or something to switch to 3d, and add some cool effects and shit
 * */

