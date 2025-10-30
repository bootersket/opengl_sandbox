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
  unsigned int texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, channels;
  unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  glBindTexture(GL_TEXTURE_2D, 0);

  return texID;

}


void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

bool pause = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        std::cout << "q pressed" << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // Handle arrow inputs
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
      CUBE_INDEX--;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
      CUBE_INDEX++;
    }
    if (key == GLFW_KEY_DOWN) {
    }
    if (key == GLFW_KEY_UP) {
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
      pause = !pause;
    }

    if (CUBE_INDEX < 0) CUBE_INDEX = CUBE_INDEX_MAX;
    if (CUBE_INDEX > CUBE_INDEX_MAX) CUBE_INDEX = 0;


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

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
	
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glfwSetKeyCallback(window, key_callback);


    Shader shader("src/shaders/shader.vert", "src/shaders/shader.frag");

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
        // -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        //  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        //  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        //  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        // -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        // -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        //
        // -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        //  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        //  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        //  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        // -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        // -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        // -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        // -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        // -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        //
        //  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        //  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        //  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        //  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        //  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        //  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        //
        // -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        //  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        //  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        //  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        // -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        //
        // -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        //  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        //  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        //  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        // -0.5f,  0.5f, -0.5f,  0.0f, 1.0f


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


    // glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    // glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    // glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    // glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    // glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
    //
    // glm::mat4 view;
    // view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
    //                    glm::vec3(0.0f, 0.0f, 0.0f),
    //                    glm::vec3(0.0f, 1.0f, 0.0f));

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

  

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
      // todo: how to get pausing functionality to work?
      // Some issues: 1. this current commented out code causes
      // a weird shaking effect when paused and 2. the rotating
      // is still "happening" when paused so when we unpause, it jumps
      // to a different scene since things were still moving while 
      // visually it was frozen.
      // if (pause) {
      //   glfwPollEvents();
      //   glfwSwapBuffers(window);
      //   continue;
      // }
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
        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::mat4 view;
        // glm::vec3 cameraPos = glm::vec3(camX, 0.0, camZ);
WILO: why does the rotation work so wonky depending on the cube of focus??
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0, 30.0f);
        glm::vec3 target = cubePositions[CUBE_INDEX];
        view = glm::lookAt(cameraPos, target, glm::vec3(0.0, 1.0, 0.0));

        int viewLoc = glGetUniformLocation(shader.programID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        /* ==========================
         *      PROJECTION MATRIX 
         * ========================== */
        int projectionLoc = glGetUniformLocation(shader.programID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

// WILO: how to get black edges on cubes?
//         apply textures that have numbers (hell even make the textures yourself
//             in paint!)
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
          model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
