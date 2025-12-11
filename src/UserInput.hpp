#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

double MOUSE_SENSITIVITY_MULTIPLIER = 0.05;
int g_glfwCursorMode;

void mouse_callback(GLFWwindow*, double xPos, double yPos);
void mouse_button_callback(GLFWwindow*, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);

/* just passes args through to glfwSetInputMode(), but also sets
 * the value of a global variable so we can get the current input mode */
void wrapper_glfwSetInputMode(GLFWwindow* window, int mode, int value);

namespace Camera {
  extern double yaw;
  extern double pitch;
  extern glm::vec3 direction;
}
