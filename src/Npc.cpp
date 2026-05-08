#include "Npc.h"
#include "GameMap.h"
#include <iostream>
#include <cmath>

Npc::Npc() {
  this->position = glm::vec3(0.0f, 0.0f, 0.0f);
  this->scale = glm::vec3(32.0f, 32.0f, 0.0f);
  this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
  
  this->state = NpcState::IDLE;
  this->direction = 0;
  this->speed = 150.0f; 
  this->idleTimer = 0.0f;
  this->idleDuration = 1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0f));
  this->targetPosition = this->position;
  
  // 8 frames, 0.1f speed, 4 cols, 2 rows (like Player walk)
  this->anim = std::make_unique<Animations>(8, 0.1f, 4, 2);

  // Setup Mesh using scale as size
  GLfloat vertices[] = {
      // Position                      // Texture Coords
      scale.x,  scale.y,  0.0f, 1.0f, 1.0f, // Top Right
      scale.x,  -scale.y, 0.0f, 1.0f, 0.0f, // Bottom Right
      -scale.x, -scale.y, 0.0f, 0.0f, 0.0f, // Bottom Left
      -scale.x, scale.y,  0.0f, 0.0f, 1.0f  // Top Left
  };
  GLuint indices[] = {0, 1, 2, 0, 2, 3};

  vao = std::make_unique<VAO>();
  vao->Bind();

  vbo = std::make_unique<VBO>(vertices, sizeof(vertices));
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

Npc::~Npc() {
  for (auto &pair : textures) {
    for (auto &tex : pair.second) {
      if (tex)
        tex->Delete();
    }
  }
}

void Npc::LoadAssets(Shader &shader) {
  for (auto &texturePath : npcTexturePaths) {
    std::vector<std::unique_ptr<Texture>> texts;
    for (auto &path : texturePath.second) {
      texts.push_back(std::make_unique<Texture>(
          path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE));
      texts.back()->texUnit(shader, "tex0", 0);
    }
    this->textures[texturePath.first] = std::move(texts);
  }
}

void Npc::getAnimation(std::string animType, int direction) {
  if (textures[animType].size() > direction && textures[animType][direction])
    textures[animType][direction]->Bind();
}

void Npc::Draw(Shader &shader, Camera &camera) {
  if (state == NpcState::MOVING) {
    glm::vec2 uv0, uv1;
    anim->GetUVCoordinates(uv0, uv1);
    glUniform2f(glGetUniformLocation(shader.ID, "texScale"), uv1.x - uv0.x, uv1.y - uv0.y);
    glUniform2f(glGetUniformLocation(shader.ID, "texOffset"), uv0.x, uv0.y);
    getAnimation("walk", direction);
  } else if (state == NpcState::IDLE) {
    glUniform2f(glGetUniformLocation(shader.ID, "texScale"), 1.0f, 1.0f);
    glUniform2f(glGetUniformLocation(shader.ID, "texOffset"), 0.0f, 0.0f);
    getAnimation("idle", direction);
  }

  vao->Bind();

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model =
      glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  model =
      glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  model =
      glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

  // We don't scale the model matrix because the vertices are already sized by scale.x/scale.y
  // model = glm::scale(model, scale);

  glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

  // Player sets camMatrix from outside, Npc can too, so we don't need to manually pass view/projection
  // unless the shader specifically uses "view" and "projection" instead of "camMatrix"
  // Assuming it uses the same textureShader as Player which uses camMatrix.
  // So we remove setMat4 for view and projection.

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  vao->Unbind();
}

void Npc::Update(float deltaTime, GameMap &gameMap) {
  if (state == NpcState::IDLE) {
    anim->currentFrame = 0; // idle frame
    idleTimer += deltaTime;
    
    if (idleTimer >= idleDuration) {
      PickRandomTarget(gameMap);
      state = NpcState::MOVING;
      idleTimer = 0.0f;
    }
  } else if (state == NpcState::MOVING) {
    anim->Update(deltaTime);
    
    glm::vec3 dir = targetPosition - position;
    float distance = glm::length(dir);
    
    if (distance < 5.0f) { // Reached target
      position = targetPosition; 
      state = NpcState::IDLE;
      idleDuration = 1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0f));
      return;
    }
    
    dir = glm::normalize(dir);
    
    // Determine facing direction (0: Down, 1: Up, 2: Left, 3: Right)
    if (std::abs(dir.x) > std::abs(dir.y)) {
      direction = (dir.x > 0) ? 3 : 2;
    } else {
      direction = (dir.y > 0) ? 1 : 0;
    }
    
    glm::vec3 nextPos = position + dir * speed * deltaTime;
    float yOffset = scale.y * 0.6f;
    
    glm::vec3 oldPos = position;

    // Independent Axis Collision
    if (nextPos.x != position.x) {
      if (!gameMap.checkCollision(nextPos.x, position.y - yOffset)) {
        position.x = nextPos.x;
      }
    }

    if (nextPos.y != position.y) {
      if (!gameMap.checkCollision(position.x, nextPos.y - yOffset)) {
        position.y = nextPos.y;
      }
    }
    
    // If stuck (no movement happened), wait and pick new target
    if (position == oldPos) {
      state = NpcState::IDLE;
      idleDuration = 1.0f; 
    }
  }
}

void Npc::PickRandomTarget(GameMap &gameMap) {
    float halfSize = scale.x / 2.0f;
    float minX = -gameMap.worldWidth / 2.0f + halfSize;
    float maxX = gameMap.worldWidth / 2.0f - halfSize;
    float minY = -gameMap.worldHeight / 2.0f + halfSize;
    float maxY = gameMap.worldHeight / 2.0f - halfSize;
    
    float randomX = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
    float randomY = minY + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxY - minY)));
    
    targetPosition = glm::vec3(randomX, randomY, position.z);
}

nlohmann::json Npc::ToJson() {
  nlohmann::json j;
  j["position"]["x"] = position.x;
  j["position"]["y"] = position.y;
  j["position"]["z"] = position.z;
  j["targetPosition"]["x"] = targetPosition.x;
  j["targetPosition"]["y"] = targetPosition.y;
  j["targetPosition"]["z"] = targetPosition.z;
  j["direction"] = direction;
  j["state"] = static_cast<int>(state);
  j["idleTimer"] = idleTimer;
  j["idleDuration"] = idleDuration;
  return j;
}

void Npc::FromJson(nlohmann::json j) {
  if (j.contains("position")) {
    position.x = j["position"]["x"];
    position.y = j["position"]["y"];
    position.z = j["position"]["z"];
  }
  if (j.contains("targetPosition")) {
    targetPosition.x = j["targetPosition"]["x"];
    targetPosition.y = j["targetPosition"]["y"];
    targetPosition.z = j["targetPosition"]["z"];
  }
  if (j.contains("direction")) {
    direction = j["direction"];
  }
  if (j.contains("state")) {
    state = static_cast<NpcState>(j["state"]);
  }
  if (j.contains("idleTimer")) {
    idleTimer = j["idleTimer"];
  }
  if (j.contains("idleDuration")) {
    idleDuration = j["idleDuration"];
  }
}
