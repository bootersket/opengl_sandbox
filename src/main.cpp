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

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastUpdateTime = 0.0f; // number of seconds since last loop
float lastFrameTime = 0.0f; // number of seconds since last frame

std::vector<glm::vec2> makeCircleVertices() {
  std::vector<glm::vec2> vertices;
  int segments = 64;
  float r = 1.0f;

  for (int i=0; i<segments; i++) {
    float t = 2.0f * M_PI * (float)i / segments;
    float x = r * cos(t);
    float y = r * sin(t);
    vertices.emplace_back(x, y);
  }
  return vertices;
}

void printVec3(glm::vec3 v) {
  std::cout << v.x << ", " << v.y << ", " << v.z << std::endl;
}

double yaw = -90.0f;
glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
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

float curCamAngle = 0.0f;
double lastXPos;
void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
  if (xPos == 0.0f) return;
  double xPosDelta = lastXPos - xPos;
  // std::cout << "xPosDelta: " << xPosDelta << std::endl;
  
  lastXPos = xPos;

}

float camX = 0.0f;
float camZ = 10.0f;
float CAM_MAX = 10.0f;
float CAM_TICK = 0.1f;

#define KEYPRESSED(goalKey) ( key == goalKey && action == GLFW_PRESS )
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        std::cout << "q pressed" << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // Handle arrow inputs
    // if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
    if (KEYPRESSED(GLFW_KEY_LEFT)) {
      CUBE_INDEX--;
    }
    if (KEYPRESSED(GLFW_KEY_RIGHT)) {
      CUBE_INDEX++;
    }
    if (CUBE_INDEX < 0) CUBE_INDEX = CUBE_INDEX_MAX;
    if (CUBE_INDEX > CUBE_INDEX_MAX) CUBE_INDEX = 0;
    // if (KEYPRESSED(GLFW_KEY_A)) {
    //   if (camX < 0 && camX <= CAM_MAX) {
    //     camX = 0.0f;
    //     camZ = -CAM_INCREMENT;
    //   }
    //   else if (camZ < 0) {
    //     camX = CAM_INCREMENT;
    //     camZ = 0.0f;
    //   }
    //   else if (camX > 0) {
    //     camX = 0.0f;
    //     camZ = CAM_INCREMENT;
    //   }
    //   else {
    //     camX = -CAM_INCREMENT;
    //     camZ = 0;
    //   }
    // }
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


int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    int WINDOW_WIDTH = 800;
    int WINDOW_HEIGHT = 800;

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
    projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
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
     *    CIRCLE MINIMAP SETUP
     * ========================== */
    Shader circleShader("src/shaders/circle.vert", "src/shaders/circle.frag");

    unsigned int circleVAO, circleVBO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);

    std::vector<glm::vec2> circleVertices = makeCircleVertices();
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(glm::vec2), circleVertices.data(), GL_STATIC_DRAW);


    posLoc = glGetAttribLocation(circleShader.programID, "aPos");
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(posLoc);





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
        direction.x = cos(glm::radians(yaw));
        direction.z = sin(glm::radians(yaw));
        // view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
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
          modelLoc = glGetUniformLocation(circleShader.programID, "model");
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


        /* ==========================
         *     DRAW CIRCLE MINIMAP
         * ========================== */
        circleShader.use();
        glBindVertexArray(circleVAO);
        int circleSegments = 64;
        glDrawArrays(GL_LINE_LOOP, 0, circleSegments); // ques: what is GL_LINE_LOOP?
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
        modelLoc = glGetUniformLocation(circleShader.programID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));



        processInput(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


/*
Current task:
- make circle not take up entire screen--model matrix?
^^^ got this to work! Now continue tinkering with it.


Next steps:
- Add in MVP matrix stuff so I can make the circle smaller
- implement the other "piece" of the mini map to indicate 
  what the current angle is (a line? a dot? idk)
- How to change thickness of circle outline?

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


