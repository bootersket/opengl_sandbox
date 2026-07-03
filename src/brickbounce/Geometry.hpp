#ifndef GEOMETRY_INCLUDE_GUARD
#define GEOMETRY_INCLUDE_GUARD

#include <ostream>


class RectPosition {
  public:
    // Origin of object is bottom left
    float x; // left side
    float y; // bottom
    float width;
    float height;
    RectPosition() {}
    RectPosition(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}

    /* Methods declared here and defined outside of class so API reference is easy */
    float getLeftX();
    float getRightX();
    float getBottomY();
    float getTopY();

    void setLeftX(float newX);
    void setRightX(float newX);
    void setBottomY(float newY);
    void setTopY(float newY);
    void setCenterX(float newX);

    void incrementX(float value);
    void decrementX(float value);

    friend std::ostream &operator<<(std::ostream &os, RectPosition const &rectPos) {
      return os << "<RectPos: pos=(" << rectPos.x << ", " << rectPos.y << ", w=" << rectPos.width << ", h=" << rectPos.height << ")" << std::endl;
    }
};




class CirclePosition {
  public:
    // Origin of object is center
    float x; // center
    float y; // center
    float radius;

    CirclePosition() {
      x = 0.0f;
      y = 0.0f;
      radius = 0.0f;
    }
    CirclePosition(float x, float y, float radius) : x(x), y(y), radius(radius) {}
    float getCenterX();
    float getCenterY();
    float getLeftX();
    float getRightX();
    float getBottomY();
    float getTopY();
    float getDiameter();

    void setLeftX(float newX);
    void setRightX(float newX);
    void setBottomY(float newY);
    void setTopY(float newY);
    void setCenterX(float newX);
    void setCenterY(float newY);
    void setCenter(float newX, float newY);

    /* These getCenter*() funcs may seem redundnant (i.e. "why not get do obj.x or obj.y?")
     * but the idea is to keep it unambiguous as to what you're getting (i.e. future me
     * will forget that CirclePosition.x is the center, so this makes it obvious)*/
};



/* Based on the object being collided into
 * eg. ball colliding into the right side 
 * of the player rect */
enum CollisionType {
  NONE=0,
  RIGHT,
  LEFT,
  TOP,
  BOTTOM
};

CollisionType checkCircleRectCollision(CirclePosition circlePos, RectPosition rectPos);
CollisionType checkRectRectCollision(RectPosition r1, RectPosition r2);

#endif /* GEOMETRY_INCLUDE_GUARD */
