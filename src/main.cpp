#include "../Header/Camera.h"
#include "../Header/GameMap.h"
#include "../Header/Items.h"
#include "../Header/Panel.h"
#include "../Header/Player.h"
#include "../Header/Shader.h"
#include "../Header/Texture.h"
#include "../Header/Window.h"
#include "../libs/imgui/backends/imgui_impl_glfw.h"
#include "../libs/imgui/backends/imgui_impl_opengl3.h"
#include "../libs/imgui/imgui.h"
#include <iostream>

using json = nlohmann::json;

const unsigned int width = 1920;
const unsigned int height = 1280;

Camera *camPtr = nullptr;

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  if (camPtr) {
    camPtr->ProcessScroll((float)yoffset);
  }
}
int main() {
  stbi_set_flip_vertically_on_load(true);

  Window window("Oyun Penceresi", width, height);
  glfwSetWindowSizeLimits(window.getGLFWWindow(), width / 2, height / 2, width,
                          height);

  Shader textureShader("texture.vert", "texture.frag");
  Shader mapShader("map.vert", "texture.frag");

  // --- GAME WORLD ---
  GameMap gameMap("newmap.json");
  gameMap.LoadTextures(mapShader);
  std::map<int, std::unique_ptr<Items>> itemList =
      Items::readJsonItems("newmap.json");
  Items::loadAtlas(textureShader);

  // --- PLAYER ---
  // Use map tile size for player size
  Player player(glm::vec3(0.0f, 0.0f, 0.0f), gameMap.tileSize,
                300.0f); // Speed=300

  player.LoadAssets(textureShader);
  // Start camera at (0,0)
  Camera camera(width, height, glm::vec3(0.0f, 0.0f, 0.0f));

  float zw = gameMap.worldWidth / (float)width;
  float zh = gameMap.worldHeight / (float)height;

  // Choose the smaller factor to ensure fitting both dimensions
  camera.maxZoom = (zw < zh) ? zw : zh;

  // Set default zoom to maximize view but keep within bounds (e.g. 0.5 or
  // maxZoom)
  camera.zoom = (camera.maxZoom < 0.5f) ? camera.maxZoom : 0.5f;

  camera.speed = 400.0f;
  camPtr = &camera; // Set global pointer

  glfwSetScrollCallback(window.getGLFWWindow(), scroll_callback);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // ImGui Setup
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window.getGLFWWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 330");

  int state = 0;

  float lastFrame = 0.0f;
  float deltaTime = 0.0f;

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  Panel panel;
  glfwSwapInterval(1);
  while (!window.ShouldClose()) {
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    textureShader.Activate();

    // Update camera width/height in case of window resize
    int winWidth, winHeight;
    glfwGetWindowSize(window.getGLFWWindow(), &winWidth, &winHeight);
    camera.width = winWidth;
    camera.height = winHeight;
    glViewport(0, 0, winWidth, winHeight);

    // Update Player
    player.Update(window.getGLFWWindow(), deltaTime, gameMap);

    // Toggle Camera Mode ("C" key)
    if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_C) == GLFW_PRESS) {
      state = 2;
    } else if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_C) == GLFW_RELEASE &&
               state == 2) {
      camera.mode = (camera.mode == CAMERA_FREE) ? CAMERA_LOCKED : CAMERA_FREE;
      state = 0;
    }

    // Toggle Item Interaction ("E" key)
    if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_E) == GLFW_PRESS) {
      state = 1;

    } else if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_E) == GLFW_RELEASE &&
               state == 1) {
      state = 0;
      for (const auto &itemPair : itemList) {
        if (itemPair.second->isActive) {
          player.Interact(*(itemPair.second));
        }
      }
    }

    if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_1) == GLFW_PRESS) {
      player.selectedSlot = 0;
    } else if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_2) == GLFW_PRESS) {
      player.selectedSlot = 1;
    } else if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_3) == GLFW_PRESS) {
      player.selectedSlot = 2;
    } else if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_4) == GLFW_PRESS) {
      player.selectedSlot = 3;
    } else if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_5) == GLFW_PRESS) {
      player.selectedSlot = 4;
    }

    if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_G) == GLFW_PRESS &&
        player.slots[player.selectedSlot].itemID != -1) {
      state = 3;
    } else if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_G) == GLFW_RELEASE &&
               state == 3) {
      state = 0;
      player.dropItem(player.selectedSlot, itemList);
    }

    // Update Camera
    camera.Inputs(
        window.getGLFWWindow(), deltaTime, player.Position,
        glm::vec4(-gameMap.worldWidth / 2.0f, -gameMap.worldHeight / 2.0f,
                  gameMap.worldWidth / 2.0f, gameMap.worldHeight / 2.0f));

    // --- RENDER ---

    // Activate Map Shader and Update Matrix
    mapShader.Activate();
    camera.updateMatrix(-100.0f, 100.0f, mapShader, "camMatrix");

    // Draw Map
    gameMap.Draw(mapShader, camera);

    // Activate Texture Shader and Update Matrix for entities
    textureShader.Activate();
    camera.updateMatrix(-100.0f, 100.0f, textureShader, "camMatrix");

    // Draw Player
    player.Draw(textureShader);

    // Draw Items

    if (!itemList.empty()) {
      itemList.begin()->second->drawAtlas(textureShader, itemList, winWidth,
                                          winHeight, camera);
    }

    // --- IMGUI RENDER ---
    const float PAD = 10.0f;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos;
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos;
    window_pos.x = work_pos.x + work_size.x - PAD;
    window_pos.y = work_pos.y + PAD;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));

    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin(
            "Camera Stats", NULL,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
      ImGui::Text("Camera Mode: %s",
                  (camera.mode == CAMERA_FREE) ? "FREE" : "LOCKED");
      ImGui::Separator();
      ImGui::Text("Press 'C' to toggle");
    }
    ImGui::End();

    panel.Update(window, player, itemList);

    // --- MINIMAP ---
    gameMap.DrawMinimap(player.Position, camera, winWidth, winHeight);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    window.SwapBuffers();
    window.PollEvents();
  }

  textureShader.Delete();
  mapShader.Delete();

  // Explicit ImGui Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}