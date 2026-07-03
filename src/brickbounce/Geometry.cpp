#include "Geometry.hpp"
#include <cmath>
#include <iostream>


/*==================================
RectPosition methods implementations
==================================*/
float RectPosition::getLeftX() {
  return x;
}
float RectPosition::getRightX() {
  return x + width;
}
float RectPosition::getBottomY() {
  return y;
}
float RectPosition::getTopY() {
  return y + height;
}
void RectPosition::setLeftX(float newX) {
  x = newX;
}
void RectPosition::setRightX(float newX) {
  x = newX - width;
}
void RectPosition::setBottomY(float newY) {
  y = newY;
}
void RectPosition::setTopY(float newY) {
  y = newY - height;
}
void RectPosition::setCenterX(float newX) {
  x = newX - width/2;
}
void RectPosition::incrementX(float value) {
  x += value;
}
void RectPosition::decrementX(float value) {
  x -= value;
}


/*====================================
CirclePosition method implementations
====================================*/
float CirclePosition::getCenterX() {
  return x;
}
float CirclePosition::getCenterY() {
  return y;
}
float CirclePosition::getLeftX() {
  return x - radius;
}
float CirclePosition::getRightX() {
  return x + radius;
}
float CirclePosition::getBottomY() {
  return y - radius;
}
float CirclePosition::getTopY() {
  return y + radius;
}
float CirclePosition::getDiameter() {
  return radius * 2;
}
void CirclePosition::setLeftX(float newX) {
  x = newX + radius;
}
void CirclePosition::setRightX(float newX) {
  x = newX - radius;
}
void CirclePosition::setBottomY(float newY) {
  y = newY + radius;
}
void CirclePosition::setTopY(float newY) {
  y = newY - radius;
}
void CirclePosition::setCenterX(float newX) {
  x = newX;
}
void CirclePosition::setCenterY(float newY) {
  y = newY;
}
void CirclePosition::setCenter(float newX, float newY) {
  x = newX;
  y = newY;
}


/*====================================
Function implementations
====================================*/

CollisionType checkCircleRectCollision(CirclePosition circlePos, RectPosition rectPos) {
  /* Use rect hitbox around circle to approximate collision */
  RectPosition circleRectHitbox(circlePos.getLeftX(), circlePos.getBottomY(), circlePos.getDiameter(), circlePos.getDiameter());
  return checkRectRectCollision(circleRectHitbox, rectPos);

}

void printCollisionType(CollisionType ct) {
  std::string s;
  switch (ct) {
    // case NONE:
    //   s = "none";
    //   break;
    case RIGHT:
      s = "right";
      break;
    case LEFT:
      s = "left";
      break;
    case TOP:
      s = "top";
      break;
    case BOTTOM:
      s = "bottom";
      break;
  }
  std::cout << "collision type: " << s << std::endl;
}

CollisionType checkRectRectCollision(RectPosition r1, RectPosition r2) {
  CollisionType collisionType = NONE;
  float yDelta;
  float xDelta;

  std::cout << "r1: " << r1 << std::endl;
  std::cout << "r2: " << r2 << std::endl;

  /* Check bounds of bottom left corner */
  if (r1.getLeftX() < r2.getRightX() && r1.getLeftX() > r2.getLeftX()
   && r1.getBottomY() < r2.getTopY() && r1.getBottomY() > r2.getBottomY()){
    xDelta = std::abs(r1.getLeftX() - r2.getRightX());
    yDelta = std::abs(r1.getBottomY() - r2.getTopY());
    if (yDelta > xDelta) collisionType = RIGHT;
    else collisionType = TOP;
  }

  /* Check bounds of bottom right */
  else if (r1.getRightX() > r2.getLeftX() && r1.getRightX() < r2.getRightX()
        && r1.getBottomY() < r2.getTopY() && r1.getBottomY() > r2.getBottomY()) {
    xDelta = std::abs(r1.getRightX() - r2.getLeftX());
    yDelta = std::abs(r1.getBottomY() - r2.getTopY());
    if (yDelta > xDelta) collisionType = LEFT;
    else collisionType = TOP;
  }

  /* Check bounds of top left */
  else if (r1.getLeftX() < r2.getRightX() && r1.getLeftX() > r2.getLeftX()
        && r1.getTopY() < r2.getTopY() && r1.getTopY() > r2.getBottomY()) {
    xDelta = std::abs(r1.getLeftX() - r2.getRightX());
    yDelta = std::abs(r1.getTopY() - r2.getBottomY());
    if (yDelta > xDelta) collisionType = RIGHT;
    else collisionType = BOTTOM;
  }

  /* Check bounds of top right */
  else if (r1.getRightX() > r2.getLeftX() && r1.getRightX() < r2.getRightX()
        && r1.getTopY() < r2.getTopY() && r1.getTopY() > r2.getBottomY()) {
    xDelta = std::abs(r1.getRightX() - r2.getLeftX());
    yDelta = std::abs(r1.getTopY() - r2.getBottomY());
    if (yDelta > xDelta) collisionType = LEFT;
    else collisionType = BOTTOM;
  }
  return collisionType;
}
