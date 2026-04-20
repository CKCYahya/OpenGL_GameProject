#include "Player.h"
#include "EBO.h"
#include "GameMap.h"
#include "Items.h"
#include "Panel.h"
#include "Shader.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"

Player::Player(glm::vec3 startPos, float size, float speed)
    : Position(startPos), size(size), speed(speed), direction(0) {
  selectedSlot = -1;
  slotAmount = sizeof(this->slots) / sizeof(this->slots[0]);
  interactionRadius =
      size * 1.5f; // Interaction radius is 1.5 times player size
  GLfloat playerVertices[] = {
      // Position            // Texture Coords
      size,  size,  0.0f, 1.0f, 1.0f, // Top Right
      size,  -size, 0.0f, 1.0f, 0.0f, // Bottom Right
      -size, -size, 0.0f, 0.0f, 0.0f, // Bottom Left
      -size, size,  0.0f, 0.0f, 1.0f  // Top Left
  };
  rayStart = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  rayEnd = glm::vec4(0.0f, -size * 2, 0.0f, 1.0f);

  GLuint indices[] = {0, 1, 2, 0, 2, 3};

  vao = std::make_unique<VAO>();
  vao->Bind();

  vbo = std::make_unique<VBO>(playerVertices, sizeof(playerVertices));
  ebo = std::make_unique<EBO>(indices, sizeof(indices));

  // Layout 0: Position (3 floats)
  vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void *)0);
  // Layout 1: TexCoord (2 floats)
  vao->LinkAttrib(*vbo, 1, 2, GL_FLOAT, 5 * sizeof(float),
                  (void *)(3 * sizeof(float)));

  vao->Unbind();
  vbo->Unbind();
  ebo->Unbind();
}

Player::~Player() {
  if (texUp)
    texUp->Delete();
  if (texDown)
    texDown->Delete();
  if (texLeft)
    texLeft->Delete();
  if (texRight)
    texRight->Delete();
}

void Player::LoadAssets(Shader &shader) {
  // Load Textures
  texDown = std::make_unique<Texture>("image/down.png", GL_TEXTURE_2D,
                                      GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
  texDown->texUnit(shader, "tex0", 0);

  texUp = std::make_unique<Texture>("image/up.png", GL_TEXTURE_2D, GL_TEXTURE0,
                                    GL_RGBA, GL_UNSIGNED_BYTE);
  texLeft = std::make_unique<Texture>("image/left.png", GL_TEXTURE_2D,
                                      GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
  texRight = std::make_unique<Texture>("image/right.png", GL_TEXTURE_2D,
                                       GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
}

void Player::Update(GLFWwindow *window, float dt, GameMap &gameMap) {
  glm::vec3 nextPos = Position;
  // WASD Movement
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    nextPos.y += speed * dt;
    direction = 1; // Up
    state = State::MOVING;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    nextPos.y -= speed * dt;
    direction = 0; // Down
    state = State::MOVING;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    nextPos.x -= speed * dt;
    direction = 2; // Left
    state = State::MOVING;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    nextPos.x += speed * dt;
    direction = 3; // Right
    state = State::MOVING;
  }

  float halfSize = size / 2.0f;

  // Collision offset to check at the character's feet instead of the center
  float yOffset = size * 0.6f;

  // Independent Axis Collision to allow Sliding
  // Check X movement
  if (nextPos.x != Position.x) {
    if (!gameMap.checkCollision(nextPos.x, Position.y - yOffset)) {
      Position.x = nextPos.x;
    }
  }

  // Check Y movement
  if (nextPos.y != Position.y) {
    if (!gameMap.checkCollision(Position.x, nextPos.y - yOffset)) {
      Position.y = nextPos.y;
    }
  }
  

  if (state == State::MOVING && glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_S) != GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_A) != GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_D) != GLFW_PRESS) {
    state = State::IDLE;
  }
  // Clamp to Map Bounds
  float minX = -gameMap.worldWidth / 2.0f + halfSize;
  float maxX = gameMap.worldWidth / 2.0f - halfSize;
  float minY = -gameMap.worldHeight / 2.0f + halfSize;
  float maxY = gameMap.worldHeight / 2.0f - halfSize;

  if (Position.x < minX)
    Position.x = minX;
  if (Position.x > maxX)
    Position.x = maxX;
  if (Position.y < minY)
    Position.y = minY;
  if (Position.y > maxY)
    Position.y = maxY;

  // Direction angles: 0=Down(270°), 1=Up(90°), 2=Left(180°), 3=Right(0°)
  static const float angles[] = { glm::radians(270.0f), glm::radians(90.0f), glm::radians(180.0f), glm::radians(0.0f) };
  float angle = angles[direction];
  float rayDistance = size * 2.0f;
  rayEnd = glm::vec4(cos(angle) * rayDistance, sin(angle) * rayDistance, 0.0f, 1.0f);

  newRayStart = glm::vec3(playerModel * rayStart);
  newRayEnd = glm::vec3(playerModel * rayEnd);
  rayDirection = glm::normalize(newRayEnd - newRayStart);
}

void Player::Draw(Shader &shader) {

  glUniform2f(glGetUniformLocation(shader.ID, "texScale"), 1.0f, 1.0f);
  glUniform2f(glGetUniformLocation(shader.ID, "texOffset"), 0.0f, 0.0f);

  if (direction == 0 && texDown)
    texDown->Bind();
  else if (direction == 1 && texUp)
    texUp->Bind();
  else if (direction == 2 && texLeft)
    texLeft->Bind();
  else if (direction == 3 && texRight)
    texRight->Bind();
  else if (texDown)
    texDown->Bind();

  // Create Model Matrix
  playerModel = glm::mat4(1.0f);
  playerModel = glm::translate(playerModel, Position);

  glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE,
                     glm::value_ptr(playerModel));
  vao->Bind();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

ImTextureID Player::Interact(Items &item) {
  int slotIndex = -1;
  bool itemAlreadyInInventory = false;
  ImTextureID result = 0;
  if (item.isItemInRange(*this)) {
    int emptySlotIndex = -1;
    for (int i = 0; i < slotAmount; i++) {
      if (this->slots[i].itemID == item.ID) {
        slotIndex = i; // if item already in inventory, get its index
        itemAlreadyInInventory = true;
        break;
      } else if (this->slots[i].itemID == -1 && emptySlotIndex == -1) {
        emptySlotIndex = i; // get first empty slot index
      }
    }
    if (!itemAlreadyInInventory && emptySlotIndex != -1) {
      slotIndex = emptySlotIndex;
    }
    std::cout << "Interacting with item: " << item.name << std::endl;
    if (itemAlreadyInInventory) {
      this->slots[slotIndex].count += 1;
    } else if (slotIndex != -1) {
      this->slots[slotIndex].itemID = item.ID;
      this->slots[slotIndex].itemName = item.name;
      this->slots[slotIndex].atlasID = item.loadedAtlases[item.atlasIndex]->ID;
      this->slots[slotIndex].count += 1;
      float atlasW = item.atlasWH[item.atlasIndex].x;
      float atlasH = item.atlasWH[item.atlasIndex].y;
      float uOff = item.uOffset;
      float vOff = item.vOffset;
      this->slots[slotIndex].uv0 = ImVec2(uOff, vOff + 32.0f / atlasH);
      this->slots[slotIndex].uv1 = ImVec2(uOff + 32.0f / atlasW, vOff);
    } else if (slotIndex == -1) {
      std::cout << "Inventory full! Item '" << item.name
                << "' could not be added." << std::endl;
      return 0; // No slot available, return 0 to indicate failure
    }
    item.isActive = !item.isActive; // Toggle item active state on interaction
    item.slotIndex = slotIndex;
    result = (ImTextureID)(intptr_t)item.loadedAtlases[item.atlasIndex]->ID;
  }

  return result;
}

void Player::dropItem(int selectedSlot,
                      std::map<int, std::unique_ptr<Items>> &itemList) {
  if (selectedSlot >= 0 && selectedSlot < slotAmount &&
      slots[selectedSlot].itemID != -1) {
    int droppedItemID = slots[selectedSlot].itemID;
    Items *item = Items::searchItems(itemList, selectedSlot);
    
    // If not found in the map create a new one
    if (item == nullptr) {
      static int dropCounter = 100000; 
      int entityID = dropCounter++;
      std::string dropName = slots[selectedSlot].itemName;
      itemList[entityID] = std::make_unique<Items>(dropName, this->Position, droppedItemID);
      item = itemList[entityID].get();
      item->atlasIndex = Items::GetAtlasIndex(droppedItemID);
    }

    if(slots[selectedSlot].count > 1){
      slots[selectedSlot].count -= 1;
      item->position = this->Position;
      item->isActive = true;  
      item->slotIndex = -1;   
      return;
    }
    
    item->slotIndex = -1;
    item->position = this->Position;
    item->isActive = true;
    
    slots[selectedSlot].itemID = -1;
    slots[selectedSlot].atlasID = 0;
    slots[selectedSlot].count = 0;
  }
}
