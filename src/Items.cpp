#include "Items.h"
#include "EBO.h"
#include "GameMap.h"
#include "Panel.h"
#include "Player.h"
#include "Shader.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "Vendor.h"
#include "Window.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

std::vector<glm::vec2> Items::atlasWH;
std::vector<std::shared_ptr<Texture>> Items::loadedAtlases;
std::map<int, int> Items::itemValueTable;
std::vector<std::string> images = {"spritesheet_32x32", "fishing rod"};

Items::Items(const std::string &name, const glm::vec3 &position, int itemID)
    : name(name), position(position), ID(itemID) {
  slotIndex = -1;
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

Items::~Items() {
  if (itemTexture) {
    itemTexture->Delete();
  }
}

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

bool Items::isItemInRange(Player &player) {
  glm::vec3 diff = player.Position - this->position;
  return glm::length(diff) < player.interactionRadius;
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
                std::string propValueStr = "";
                int propValueInt = 0;
                for (const auto &prop : item["properties"]) {
                  std::string propName = prop.value("name", "");

                  if (propName == "item") {
                    propValueStr = prop.value("value", "");
                  } else if (propName == "value") {
                    propValueInt = prop.value("value", 0);
                  }
                }

                if (!propValueStr.empty()) {
                  // Fiyat bilgisini her zaman kaydet
                  itemValueTable[localID] = propValueInt;

                  // Balıkları spawn etme
                  if (propValueStr == "palamut" || propValueStr == "levrek" ||
                      propValueStr == "istavrit" || propValueStr == "uskumru" ||
                      propValueStr == "lufer") {
                    continue;
                  }

                  // Sadece Olta'yı belirli bir yere spawn et
                  if (propValueStr == "olta") {
                    auto newItem = std::make_unique<Items>(
                        propValueStr, glm::vec3(300.0f, 300.0f, 0.0f), localID);
                    newItem->atlasIndex = i;
                    newItem->value = propValueInt;
                    itemType[globalID] = std::move(newItem);
                  }
                  // Diğer itemleri (eğer varsa) buraya ekleyebilirsin veya 
                  // JSON'daki objeler katmanından okuyabiliriz.
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
    for (auto &item : itemList) {
      if (item.second->atlasIndex != i) {
        continue;
      }
      if (!item.second->isActive) {
        continue;
      }
      glm::vec2 uv = CalculateUV(item.second->ID, item.second->atlasIndex);
      item.second->uOffset = uv.x;
      item.second->vOffset = uv.y;
      glUniform2f(glGetUniformLocation(shader.ID, "texOffset"),
                  item.second->uOffset, item.second->vOffset);
      glm::vec2 texScale(32.0f / atlasWH[i].x, 32.0f / atlasWH[i].y);
      glUniform2fv(glGetUniformLocation(shader.ID, "texScale"), 1,
                   glm::value_ptr(texScale));
      float base_y = item.second->position.y - 16.0f; // item size assumed 16
      float zDepth = -base_y * 0.001f;
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(item.second->position.x, item.second->position.y, zDepth));
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

nlohmann::json Items::ToJson(std::map<int, std::unique_ptr<Items>> &itemList) {
  nlohmann::json j;
  j["items"] = nlohmann::json::array();
  for (auto &item : itemList) {
    nlohmann::json itemJson;
    itemJson["ID"] = item.second->ID;
    itemJson["name"] = item.second->name;
    itemJson["value"] = item.second->value;
    itemJson["atlasIndex"] = item.second->atlasIndex;
    itemJson["position"]["x"] = item.second->position.x;
    itemJson["position"]["y"] = item.second->position.y;
    itemJson["position"]["z"] = item.second->position.z;
    itemJson["slotIndex"] = item.second->slotIndex;
    itemJson["isActive"] = item.second->isActive;
    j["items"].push_back(itemJson);
  }
  return j;
}

void Items::Reset(std::map<int, std::unique_ptr<Items>> &itemList) {
  itemList.clear();
}

void Items::FromJson(std::map<int, std::unique_ptr<Items>> &itemList,
                     nlohmann::json j) {
  itemList.clear();
  for (auto &item : j["items"]) {
    auto newItem = std::make_unique<Items>(
        item["name"], glm::vec3(0.0f, 0.0f, 0.0f), item["ID"]);
    newItem->ID = item["ID"];
    newItem->value = item["value"];
    newItem->atlasIndex = item["atlasIndex"];
    newItem->position.x = item["position"]["x"];
    newItem->position.y = item["position"]["y"];
    newItem->position.z = item["position"]["z"];
    newItem->slotIndex = item["slotIndex"];
    newItem->isActive = item["isActive"];
    itemList[newItem->ID] = std::move(newItem);
  }
}

glm::vec2 Items::CalculateUV(int itemID, int atlasIndex) {
  int cols = static_cast<int>(atlasWH[atlasIndex].x / 32);
  int totalRows = static_cast<int>(atlasWH[atlasIndex].y / 32);
  int col = itemID % cols;
  int row = itemID / cols;
  int texRow = totalRows - 1 - row;
  float uOffset = (col * 32.0f) / atlasWH[atlasIndex].x;
  float vOffset = (texRow * 32.0f) / atlasWH[atlasIndex].y;
  return glm::vec2(uOffset, vOffset);
}

int Items::GetAtlasIndex(int itemID) {
  int offset = 0;
  for (int i = 0; i < loadedAtlases.size(); i++) {
    int cols = static_cast<int>(atlasWH[i].x / 32);
    int rows = static_cast<int>(atlasWH[i].y / 32);
    int tileCount = cols * rows;
    if (itemID >= offset && itemID < offset + tileCount) {
      return i;
    }
    offset += tileCount;
  }
  return -1;
}

bool Items::AddItem(Player &player, int itemID, std::string itemName) {
  if (itemID < 0) {
    std::cerr << "Error: Invalid item ID passed to AddItem (" << itemID << ")"
              << std::endl;
    return false;
  }
  for (auto &item : player.slots) {
    if (item.itemID == itemID && item.itemID != -1 && item.count < 3) {
      item.count += 1;
      std::cout << "You added a " << itemName << " to your inventory!"
                << std::endl;
      return true;
    }
  }
  for (int i = 0; i < 5; i++) {
    auto &item = player.slots[i];
    if (item.itemID == -1) {
      item.itemID = itemID;
      item.itemName = itemName;
      item.atlasIndex = Items::GetAtlasIndex(itemID);
      item.atlasID = Items::loadedAtlases[item.atlasIndex]->ID;
      item.uOffset = Items::CalculateUV(itemID, item.atlasIndex).x;
      item.vOffset = Items::CalculateUV(itemID, item.atlasIndex).y;
      item.uv0 =
          ImVec2(item.uOffset,
                 item.vOffset + 32.0f / Items::atlasWH[item.atlasIndex].y);
      item.uv1 =
          ImVec2(item.uOffset + 32.0f / Items::atlasWH[item.atlasIndex].x,
                 item.vOffset);
      item.count = 1;

      // Fiyat bilgisini tablodan al
      if (itemValueTable.find(itemID) != itemValueTable.end()) {
        item.itemValue = itemValueTable[itemID];
      }

      std::cout << "You added a " << itemName << " to your inventory!"
                << std::endl;
      return true;
    }
  }
  std::cout << "Inventory is full!" << std::endl;
  return false;
}

void Items::UpdateItemValue(Player &player,
                            std::map<int, std::unique_ptr<Items>> &worldItems,
                            Vendor &vendor) {
  for (auto &item : worldItems) {
    if (vendor.tradeLevel != UpgradeLevel::MAX &&
        item.second->name != "UNKNOWN" && item.second->value != 0) {
      item.second->value = item.second->value + 50;
    }
  }
  for (auto &item : player.slots) {
    if (vendor.tradeLevel != UpgradeLevel::MAX && item.itemName != "UNKNOWN" &&
        item.itemValue != 0) {
      item.itemValue = item.itemValue + 50;
    }
  }
}