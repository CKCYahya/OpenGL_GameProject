#include "Items.h"
#include "EBO.h"
#include "GameMap.h"
#include "Panel.h"
#include "Player.h"
#include "Shader.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "Window.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

std::vector<glm::vec2> Items::atlasWH;
std::vector<std::shared_ptr<Texture>> Items::loadedAtlases;

Items::Items(const std::string &name, const glm::vec3 &position)
    : name(name), position(position) {
  float size = 16.0f;
  // Initialize VAO, VBO, EBO for the item
  // Load texture using texturePath
  // Set up item properties (name, position, slotIndex, value)
  GLfloat itemVertices[] = {
      // Define item vertices here
      size,  size,  0.0f, 1.0f, 1.0f, // Top Right
      size,  -size, 0.0f, 1.0f, 0.0f, // Bottom Right
      -size, -size, 0.0f, 0.0f, 0.0f, // Bottom Left
      -size, size,  0.0f, 0.0f, 1.0f  // Top Left
  };
  GLuint indices[] = {0, 1, 2, 0, 2, 3};

  vao = std::make_unique<VAO>();
  vao->Bind();
  vbo = std::make_unique<VBO>(itemVertices, sizeof(itemVertices));
  ebo = std::make_unique<EBO>(indices, sizeof(indices));
  vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void *)0);
  vao->LinkAttrib(*vbo, 1, 2, GL_FLOAT, 5 * sizeof(float),
                  (void *)(3 * sizeof(float)));

  vao->Unbind();
  vbo->Unbind();
  ebo->Unbind();
}

Items::~Items() { itemTexture->Delete(); }

void Items::Load(Shader &shader, const char *texturePath) {
  itemTexture = std::make_unique<Texture>(
      texturePath, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
  itemTexture->texUnit(shader, "tex0", 0);
}

void Items::Draw(Shader &shader, Player &player) {

  glUniform2f(glGetUniformLocation(shader.ID, "texScale"), 1.0f, 1.0f);
  glUniform2f(glGetUniformLocation(shader.ID, "texOffset"), 0.0f, 0.0f);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);

  glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE,
                     glm::value_ptr(model));
  itemTexture->Bind();
  vao->Bind();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Items::isItemInRange(const Player &player) {
  glm::vec3 diff = player.Position - this->position;
  if (glm::length(diff) < player.interactionRadius) {
    return true;
  }
  return false;
}

std::map<int, std::unique_ptr<Items>>
Items::readJsonItems(const char *jsonItems) {
  std::ifstream dataFile(jsonItems);
  std::map<int, std::unique_ptr<Items>> itemType;
  int min = 0;
  int max = 500;
  if (dataFile.is_open()) {
    try {
      json itemData = json::parse(dataFile);
      if (itemData.contains("tilesets")) {
        for (const auto &itemset : itemData["tilesets"]) {
          int firstGid = itemset.value("firstgid", 1);
          std::string itemsetName = itemset.value("name", "");
          for (int i = 0; i < images.size(); i++) {
            if (images[i] == itemsetName) {
              for (const auto &item : itemset["tiles"]) {
                int localID = item.value("id", -1);
                int globalID = firstGid + localID;
                for (const auto &prop : item["properties"]) {
                  std::string propName = prop.value("name", "");
                  std::string propValue = prop.value("value", "");
                  if (propName == "item") {
                    auto newItem = std::make_unique<Items>(
                        propValue, glm::vec3(0.0f, 0.0f, 0.0f));
                    newItem->ID = localID;
                    newItem->atlasIndex = i;
                    int rastgeleSayi = min + (std::rand() % (max - min + 1));
                    newItem->position =
                        glm::vec3(rastgeleSayi, rastgeleSayi, 0.0f);
                    std::cout << newItem->position.x << " "
                              << newItem->position.y << std::endl;
                    itemType[globalID] = std::move(newItem);
                  }
                }
              }
            }
          }
        }
      }
      std::cout << "Successfully loaded " << jsonItems << std::endl;

    } catch (json::parse_error &e) {
      std::cerr << "JSON Parse Error in " << jsonItems << ": " << e.what()
                << std::endl;
    }
  } else {
    std::cerr << "Error: " << jsonItems << " not found." << std::endl;
  }

  return itemType;
}
void Items::loadAtlas(Shader &shader) {

  for (int i = 0; i < images.size(); i++) {
    std::string textureName = "image/" + images[i] + ".png";
    auto tex =
        std::make_shared<Texture>(textureName.c_str(), GL_TEXTURE_2D,
                                  GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    tex->texUnit(shader, "tex0", 0);
    atlasWH.push_back(glm::vec2(tex->width, tex->height));
    loadedAtlases.push_back(tex);
  }
}

void Items::drawAtlas(Shader &shader,
                      std::map<int, std::unique_ptr<Items>> &itemList,
                      int winWidth, int winHeight, Camera &camera) {
  shader.Activate();
  float visibleWidth = winWidth * camera.zoom;
  float visibleHeight = winHeight * camera.zoom;

  float viewCenterX = camera.Position.x + visibleWidth / 2.0f;
  float viewCenterY = camera.Position.y + visibleHeight / 2.0f;
  for (int i = 0; i < loadedAtlases.size(); i++) {
    loadedAtlases[i]->Bind();
    int cols = static_cast<int>(atlasWH[i].x / 32);
    int totalRows = static_cast<int>(atlasWH[i].y / 32);
    for (auto &item : itemList) {
      if (item.second->atlasIndex != i) {
        continue;
      }
      if (!item.second->isActive) {
        continue;
      }
      int localID = item.second->ID;
      int col = localID % cols;
      int row = localID / cols;
      int texRow = totalRows - 1 - row;
      item.second->uOffset = (col * 32.0f) / atlasWH[i].x;
      item.second->vOffset = (texRow * 32.0f) / atlasWH[i].y;
      glUniform2f(glGetUniformLocation(shader.ID, "texOffset"),
                  item.second->uOffset, item.second->vOffset);
      glm::vec2 texScale(32.0f / atlasWH[i].x, 32.0f / atlasWH[i].y);
      glUniform2fv(glGetUniformLocation(shader.ID, "texScale"), 1,
                   glm::value_ptr(texScale));
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, item.second->position);
      glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE,
                         glm::value_ptr(model));
      item.second->vao->Bind();
      if (item.second->position.x < viewCenterX + visibleWidth / 2.0f &&
          item.second->position.x > viewCenterX - visibleWidth / 2.0f &&
          item.second->position.y < viewCenterY + visibleHeight / 2.0f &&
          item.second->position.y > viewCenterY - visibleHeight / 2.0f) {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
    }
  }
}

Items *Items::searchItems(std::map<int, std::unique_ptr<Items>> &itemList,
                          int slot) {
  for (auto &item : itemList) {
    if (item.second->slotIndex == slot) {
      return item.second.get();
    }
  }
  return nullptr;
}