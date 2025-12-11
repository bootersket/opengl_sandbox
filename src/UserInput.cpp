#include <iostream>

#include "UserInput.hpp"




void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
  static double lastXPos;
  static double lastYPos;
  static float curCamAngle = 0.0f;

  double xPosDelta = xPos - lastXPos;
  Camera::yaw += xPosDelta * MOUSE_SENSITIVITY_MULTIPLIER;
  lastXPos = xPos;

  // std::cout << "yPos: " << yPos << std::endl;
  // std::cout << "pitch: " << pitch << std::endl;
  double yPosDelta = yPos - lastYPos;
  Camera::pitch -= yPosDelta * MOUSE_SENSITIVITY_MULTIPLIER;
  lastYPos = yPos;

}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  int cursorMode = wrapper_glfwGetInputMode(GLFW_CURSOR);
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && cursorMode == GLFW_CURSOR_NORMAL) {
      wrapper_glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
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



namespace Camera {
  double yaw = -90.0;
  double pitch = 0.0;
  glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
}

bool isEscapePressed = false;

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
    Camera::yaw -= 0.001;
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
