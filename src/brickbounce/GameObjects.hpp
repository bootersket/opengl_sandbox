#ifndef GAME_OBJECTS_INCLUDE_GUARD
#define GAME_OBJECTS_INCLUDE_GUARD

#include "Geometry.hpp"

class Ball {
  public:
    unsigned int id;
    CirclePosition pos;
    float speed;
    int xDir;
    int yDir;
    glm::vec3 color;

    Ball(float x, float y, float radius, float speed, int xDir, int yDir) {
      static unsigned int _id = 0;
      id = _id++;

      this->pos.x = x;
      this->pos.y = y;
      this->pos.radius = radius;
      this->speed = speed;
      this->xDir = xDir;
      this->yDir = yDir;
    }
    void setColor(float r, float g, float b) {
      this->color = glm::vec3(r, g, b);
    }

    friend std::ostream &operator<<(std::ostream &os, Ball const &ball) {
      return os << "<Ball: pos=(" << ball.pos.x << ", " << ball.pos.y << ", r=" << ball.pos.radius << "), " << "speed=" << ball.speed << ", xDir=" << ball.xDir << ", yDir=" << ball.yDir << std::endl;
    }

    friend bool operator==(const Ball& lhs, const Ball& rhs) {
      return lhs.id == rhs.id;
    }

};

enum BrickState {
  SOLID,
  HIT
};

class Brick {
  public:
    unsigned int id;
    RectPosition pos;
    enum BrickState state;

    Brick(float x, float y, float width, float height) {
      static unsigned int _id = 0;
      id = _id++;

      this->pos.x = x;
      this->pos.y = y;
      this->pos.width = width;
      this->pos.height = height;
      this->state = SOLID;
    }

    void hit() {
      state = HIT;
    }
};

#endif /* GAME_OBJECTS_INCLUDE_GUARD */
