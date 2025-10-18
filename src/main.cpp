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





class Movement {
  public:
    float directionX;
    float directionY;
    float velocity;

    Movement() {
      directionX = 0;
      directionY = 0;
    }
    Movement(float h, float v) {
      directionX = h;
      directionY = v;
    }
};

class KeyStates {
  public:
    bool left;
    bool right;
    bool down;
    bool up;

    KeyStates() {
      left = false;
      right = false;
      down = false;
      up = false;
    }

    void leftPress() {
      left = true;
    }
    void leftRelease() {
      left = false;
    }
    void rightPress() {
      right = true;
    }
    void rightRelease() {
      right = false;
    }
    void downPress() {
      down = true;
    }
    void downRelease() {
      down = false;
    }
    void upPress() {
      up = true;
    }
    void upRelease() {
      up = false;
    }
};

Movement movement;
KeyStates keyStates;

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        std::cout << "q pressed" << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // Handle arrow inputs
    if (key == GLFW_KEY_LEFT) {
      if (action == GLFW_PRESS) keyStates.leftPress();
      if (action == GLFW_RELEASE) keyStates.leftRelease();
    }
    if (key == GLFW_KEY_RIGHT) {
      if (action == GLFW_PRESS) keyStates.rightPress();
      if (action == GLFW_RELEASE) keyStates.rightRelease();
    }
    if (key == GLFW_KEY_DOWN) {
      if (action == GLFW_PRESS) keyStates.downPress();
      if (action == GLFW_RELEASE) keyStates.downRelease();
    }
    if (key == GLFW_KEY_UP) {
      if (action == GLFW_PRESS) keyStates.upPress();
      if (action == GLFW_RELEASE) keyStates.upRelease();
    }
    movement.directionX = 0;
    movement.directionY = 0;
    // todo: would like to do priority-based movement but seem to have
    // some weird issues when trying to implement that. Leaving it without
    // priority logic for now (which means when left and right are pressed together,
    // they cancel each other out leading to zero movement.)
    if (keyStates.left) movement.directionX = -1;
    if (keyStates.right) movement.directionX = 1;
    if (keyStates.down) movement.directionY = -1;
    if (keyStates.up) movement.directionY = 1;
    // if (keyStates.left) movement.directionX -= 1;
    // if (keyStates.right) movement.directionX += 1;
    // if (keyStates.down) movement.directionY -= 1;
    // if (keyStates.up) movement.directionY += 1;



    // Handle velocity adjustments
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
      movement.velocity--;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
      movement.velocity++;
    }

    // Other
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

    /* TEXTURE STUFF */

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int texImageWidth, texImageHeight, numOfChannels;
    unsigned char *data = stbi_load("resources/crate.jpg", &texImageWidth, &texImageHeight, &numOfChannels, 0);
    
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImageWidth, texImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


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
        //
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

        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

        // Top
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,

         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f
    };

    unsigned int vao, vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int posLoc, texCoordsLoc, colorLoc;
    posLoc = glGetAttribLocation(shader.programID, "aPos");
    // texCoordsLoc = glGetAttribLocation(shader.programID, "aTexCoords");
    colorLoc = glGetAttribLocation(shader.programID, "aColor");

    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // glVertexAttribPointer(texCoordsLoc, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(sizeof(float)*5));
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(sizeof(float)*3));

    glEnableVertexAttribArray(posLoc);
    // glEnableVertexAttribArray(texCoordsLoc);
    glEnableVertexAttribArray(colorLoc);


    // glm::mat4 trans = glm::mat4(1.0f);
    // trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    // glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    // vec = trans * vec;
    // std::cout << vec.x << vec.y << vec.z << vec.w << std::endl;
    // glm::mat4 trans = glm::mat4(1.0f);
    // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);





    glEnable(GL_DEPTH_TEST);


    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        int modelLoc = glGetUniformLocation(shader.programID, "model");
        model = glm::rotate(model, glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(shader.programID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projectionLoc = glGetUniformLocation(shader.programID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        

        // glm::mat4 trans = glm::mat4(1.0f);
        // trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        //
        // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        // glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 36);



        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
