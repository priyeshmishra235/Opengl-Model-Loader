#define STB_IMAGE_IMPLEMENTATION

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Model.h"
#include "Shader.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GL Test Window",
                                        nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
  }

  stbi_set_flip_vertically_on_load(true);
  glEnable(GL_DEPTH_TEST);

  // ImGui context setup
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  Shader ourShader("Shaders/shader.vert", "Shaders/shader.frag");
  Shader lightShader("Shaders/lightsource.vert", "Shaders/lightsource.frag");

  Model ourModel("model/backpack.obj");

  // Light Cube setup
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f,
      0.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
      0.0f,  0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
      0.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
      0.0f,  0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      0.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,
      0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
      0.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f,
      0.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  0.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f,  0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      0.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  0.0f,
  };

  unsigned int lightVAO, lightVBO;
  glGenVertexArrays(1, &lightVAO);
  glGenBuffers(1, &lightVBO);
  glBindVertexArray(lightVAO);
  glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Light properties
  glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
  glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
  glm::vec3 ambient(0.2f);
  glm::vec3 diffuse(0.5f);
  glm::vec3 specular(1.0f);
  float shininess = 32.0f;
  float modelScale = 1.0f;

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    // Render Model
    ourShader.use();
    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);
    ourShader.setVec3("viewPos", camera.Position);
    ourShader.setVec3("lightPos", lightPos);
    ourShader.setVec3("lightColor", lightColor);
    ourShader.setVec3("materialSpecular", specular);
    ourShader.setFloat("materialShininess", shininess);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f));
    model = glm::scale(model, glm::vec3(modelScale));
    ourShader.setMat4("model", model);

    ourModel.Draw(ourShader);

    // Render Light Cube
    lightShader.use();
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    lightShader.setMat4("model", model);
    lightShader.setVec3("lightColor", lightColor);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Light Controls");
    ImGui::SliderFloat3("Light Position", glm::value_ptr(lightPos), -10.0f,
                        10.0f);
    ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));
    ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient));
    ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse));
    ImGui::ColorEdit3("Specular", glm::value_ptr(specular));
    ImGui::SliderFloat("Shininess", &shininess, 2.0f, 128.0f);
    ImGui::SliderFloat("Model Scale", &modelScale, 0.1f, 5.0f);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  return 0;
}
void processInput(GLFWwindow *window) {

  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureKeyboard)
    return;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
    return;

  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
