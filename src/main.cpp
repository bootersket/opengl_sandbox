/* Third party includes */
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include "shader.hpp"
#include <vector>


int CUBE_INDEX_MAX = 3;
int CUBE_INDEX = 0;

/* ==========================
*      USER INPUT VARIABLES 
* ========================== */
double MOUSE_SENSITIVITY_MULTIPLIER = 0.05;
int g_glfwCursorMode;
double yaw = -90.0;
double pitch = 0.0;
glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
bool isEscapePressed = false;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastUpdateTime = 0.0f; // number of seconds since last loop
float lastFrameTime = 0.0f; // number of seconds since last frame

/* =================================
*  FUNCTION DECLARATIONS 
* ================================== */
void wrapper_glfwSetInputMode(GLFWwindow* window, int mode, int value);
int wrapper_glfwGetInputMode(int mode);
void printVec3(glm::vec3 v);

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
  static double lastXPos;
  static double lastYPos;

  double xPosDelta = xPos - lastXPos;
  double yPosDelta = yPos - lastYPos;
  lastXPos = xPos;
  lastYPos = yPos;

  yaw += xPosDelta * MOUSE_SENSITIVITY_MULTIPLIER;
  // lastXPos = xPos;

  // double yPosDelta = yPos - lastYPos;
  pitch -= yPosDelta * MOUSE_SENSITIVITY_MULTIPLIER;
  // lastYPos = yPos;
  
  if (pitch > 89.0) pitch = 89.0;
  if (pitch < -89.0) pitch = -89.0;

  /* This logic is to avoid the camera jitter that happens
   * when first running the program */
  static bool firstMouse = true;
  if (firstMouse) {
    lastXPos = xPos;
    lastYPos = yPos;
    firstMouse = false;
  }

}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  int cursorMode = wrapper_glfwGetInputMode(GLFW_CURSOR);
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && cursorMode == GLFW_CURSOR_NORMAL) {
      wrapper_glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
}

WILO: now need to use the new camera.hpp class code.
float fov = 45.0f;
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
  float MIN_FOV = 1.0f;
  float MAX_FOV = 90.0f;
  fov -= (float)yOffset;
  if (fov < MIN_FOV) fov = MIN_FOV;
  if (fov > MAX_FOV) fov = MAX_FOV;
}

float camX = 0.0f;
float camZ = 10.0f;
float CAM_MAX = 10.0f;
float CAM_TICK = 0.1f;

#define KEYPRESSED(goalKey) ( key == goalKey && action == GLFW_PRESS )
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }


    // Handle arrow inputs
    if (KEYPRESSED(GLFW_KEY_LEFT)) {
      CUBE_INDEX--;
    }
    if (KEYPRESSED(GLFW_KEY_RIGHT)) {
      CUBE_INDEX++;
    }
    if (CUBE_INDEX < 0) CUBE_INDEX = CUBE_INDEX_MAX;
    if (CUBE_INDEX > CUBE_INDEX_MAX) CUBE_INDEX = 0;

    if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
      if (camX >= CAM_MAX) return;
      camX += CAM_TICK;
      camZ = -camX + CAM_MAX;
    }
    if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
      if (camX <= -CAM_MAX) return;
      camX -= CAM_TICK;
      camZ = camX + CAM_MAX;
    }
}




void processInput(GLFWwindow* window) {
  const float cameraSpeed = 10 * deltaTime;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    // cameraPos += cameraSpeed * cameraFront;
    cameraPos += cameraSpeed * direction;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    // cameraPos -= cameraSpeed * cameraFront;
    cameraPos -= cameraSpeed * direction;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    // cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    cameraPos -= glm::normalize(glm::cross(direction, cameraUp)) * cameraSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    // cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    cameraPos += glm::normalize(glm::cross(direction, cameraUp)) * cameraSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    cameraPos -= cameraSpeed * cameraUp;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    cameraPos += cameraSpeed * cameraUp;
  }

  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
    yaw -= 0.001;
  }
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
    yaw += 0.001;
  }
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    std::cout << "yaw = " << yaw << std::endl;
    printVec3(direction);
  }

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !isEscapePressed) {
    std::cout << "escape pressed" << std::endl;
    isEscapePressed = true;
    wrapper_glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
    isEscapePressed = false;
  }
}

/* just passes args through to glfwSetInputMode(), but also sets
 * the value of a global variable so we can get the current input mode */
void wrapper_glfwSetInputMode(GLFWwindow* window, int mode, int value) {
  if (mode == GLFW_CURSOR) {
    g_glfwCursorMode = value;
    glfwSetInputMode(window, mode, value);
  }
  else {
    std::cout << "ERROR: wrapper_glfwSetInputMode() : unsupported glfw input mode" << std::endl;
  }
}




void printVec3(glm::vec3 v) {
  std::cout << v.x << ", " << v.y << ", " << v.z << std::endl;
}


/* Not technically a wrapper since there's not actually a glfwGetInputMode
 * but I think it's more consistent and less weird to do it this way.
 * Also, of course this doesn't really have the functionality to support
 * multiple windows or anything, but hey this program is a sandbox after all. */
int wrapper_glfwGetInputMode(int mode) {
  if (mode == GLFW_CURSOR) {
    return g_glfwCursorMode;
  }
  else {
    std::cout << "ERROR: wrapper_glfwGetInputMode() : unsupported glfw input mode" << std::endl;
    return -1;
  }
}



void parseCubePositions(std::vector<glm::vec3> &cubePositions) {
  std::ifstream file("cubePositions.txt");
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream in(line);

    float x, y, z;
    in >> x >> y >> z;
    glm::vec3 positions = glm::vec3(x, y, z);
    cubePositions.push_back(positions);
  }
}

unsigned int makeTexture(std::string filepath) {
  GLint FORMAT = GL_RGB;

  /*=====================================
  Texture setup
  ======================================*/
  unsigned int texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


  /*=====================================
  Load image data
  ======================================*/
  int width, height, channels;
  unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

  /*=====================================
  Check that data was read successfully
  ======================================*/
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, FORMAT, width, height, 0, FORMAT, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else {
    std::cout << "Failed to load texture" << std::endl;
  }

  /*=====================================
  Tear down
  ======================================*/
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);


  return texID;
}


void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}





int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    float ASPECT_RATIO = 16.0/9.0;
    int WINDOW_WIDTH = 800;
    int WINDOW_HEIGHT = 800.0 / ASPECT_RATIO;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
	
        return -1;
    }
    glfwSetWindowPos(window, 2000, 100);
    glfwShowWindow(window);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    wrapper_glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    Shader shader("src/shaders/shader.vert", "src/shaders/shader.frag");

    stbi_set_flip_vertically_on_load(true);
    unsigned int tex1 = makeTexture("resources/one.jpg");
    unsigned int tex2 = makeTexture("resources/two.jpg");
    unsigned int tex3 = makeTexture("resources/three.jpg");
    unsigned int tex4 = makeTexture("resources/four.jpg");


    // float vertices[] = {
    //   // positions         // colors          // tex coords
    //   0.5f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top right
    //   0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 1.0f,   1.0f, 0.0f, // bottom right
    //   -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, .70f,   0.0f, 0.0f,  // bottom left
    //   -0.5f, 0.5f, 0.0f,  0.0f, 0.0f, 0.0f,   0.0f, 1.0f  // top left
    // };



    float vertices[] = {
        // Position           // Texture
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        // Top
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        // Back
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int vao, vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int posLoc, texCoordLoc, colorLoc;
    posLoc = glGetAttribLocation(shader.programID, "aPos");
    texCoordLoc = glGetAttribLocation(shader.programID, "aTexCoord");

    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(sizeof(float)*3));

    glEnableVertexAttribArray(posLoc);
    glEnableVertexAttribArray(texCoordLoc);


    // glm::mat4 model = glm::mat4(1.0f);

    // glm::mat4 view = glm::mat4(1.0f);
    // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection;
    // projection = glm::perspective(glm::radians(fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    // left right bottom top near far
    // todo how to get ortho to work? are my values eg. near & far wrong? or is something else amiss?
    // projection = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, 0.1f, 100.0f);



    std::vector<glm::vec3> cubePositions = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f)
    };

    std::vector<unsigned int> textures = {
      tex1,
      tex2,
      tex3,
      tex4
    };


    float fps = 60.0f;
    float secPerFrame = 1.0f/fps;


    /* ==========================
     *      RENDER LOOP
     * ========================== */
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {

      float now = glfwGetTime();
      deltaTime = now - lastUpdateTime;
      glfwPollEvents();


      if ((now - lastFrameTime) >= secPerFrame) {
        glfwSwapBuffers(window);
        lastFrameTime = now;
      }
      lastUpdateTime = now;

        projection = glm::perspective(glm::radians(fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glBindVertexArray(vao);



        int modelLoc = glGetUniformLocation(shader.programID, "model");
        int cubeColorLoc = glGetUniformLocation(shader.programID, "cubeColor");
        float cubeColors[] = {
          1.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 1.0f,
          0.0f, 1.0f, 1.0f,
        };

        /* ==========================
         *      VIEW MATRIX 
         * ========================== */
        glm::mat4 view;
        // direction.x = cos(glm::radians(yaw));
        // direction.z = sin(glm::radians(yaw));
        // view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // std::cout << std::endl;
        // std::cout << "yaw=" << yaw << std::endl;
        // std::cout << "cos(yaw)=" << cos(glm::radians(yaw)) << std::endl;
        // std::cout << "sin(yaw)=" << sin(glm::radians(yaw)) << std::endl;
        direction.x = cos(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        view = glm::lookAt(cameraPos, cameraPos + direction, cameraUp);

        int viewLoc = glGetUniformLocation(shader.programID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        /* ==========================
         *      PROJECTION MATRIX 
         * ========================== */
        int projectionLoc = glGetUniformLocation(shader.programID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

// WILO: how to get black edges on cubes?
//         then dig back into why the rotation logic is so different for each cube?
//         also see if you can use a vector of glm::vec3s for cubeColors instead of
//         a raw array of floats. Is there any reason, best practice, etc. for one
//         way vs the other?

        /* ==========================
         *      MODEL MATRIX 
         * ========================== */
        for (unsigned int i=0; i<cubePositions.size(); i++) {
          glm::mat4 model = glm::mat4(1.0f);
          model = glm::translate(model, cubePositions[i]);
          float angle = 10.0f * i;
          // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
          modelLoc = glGetUniformLocation(shader.programID, "model");
          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
          
          glUniform3fv(cubeColorLoc, 1, cubeColors+(i*3));

          if (i < textures.size()) {
            glBindTexture(GL_TEXTURE_2D, textures[i]);
          }
          else {
            glBindTexture(GL_TEXTURE_2D, 0);
          }

          glDrawArrays(GL_TRIANGLES, 0, 36);
        }




        processInput(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


/*
Current task:
* tinkering with mouse input


Next steps:
* I switched the window size to be 16:9, but because it's no longer a square,
  the different ratio between width and height causes the yaw and pitch mouse input
  to feel different. I assume aspect ratio needs to be taken into account for mouse input?
* tinker with and understand why pitch adjustment needs to be += while yaw is -=
    WILO: trying to understand why yaw is added to, while pitch is subtracted from.
            It may be related to how in graphics we "think" the camera is moving around
            a stationary scene, but in reality it's a stationary camera with the entire
            scene moving around it. So I was adding some yaw, cos, and sin output to main
            to better develop a mental map, but ran into a secondary issue where I couldn't
            move the GLFW window once the program is running, so I started adding logic
            to bring back the cursor so I can move the window. Got that working, just need
            to also make it so the cursor can disappear again.
* figure out how to make the "player" walk on a plane, rather than walk in the direction that
  the camera is pointing in.

Questions:
- What is GL_LINE_LOOP? What is the first arg to glDrawArrays() in general?
- Wonder if I can just do model * pos in vert shader? Or do I need empty/basic view and projection matrices?

Problems:

Solved:
- why is it not green? --> using the wrong shader
- how do I make it stay in place (like a ui element)? --> using the wrong shader makes it exist in 3d space.
- why is it not a circle? --> messed up telling opengl how to parse vertex data
- why was it not a full circle? --> c++ vector: giving the wrong number of bytes to glBufferData()
- why does the raw float array make it not a full circle? --> missed copying the first vertex from the print output
 */


