
#ifndef BRICK_SCENE_INCLUDE_GUARD
#define BRICK_SCENE_INCLUDE_GUARD

#include "Geometry.hpp"
#include "GameObjects.hpp"


class BrickScene {
  /*
   * A BrickScene is an encaspulation of a group of bricks
   * */
  public:
    // std::vector<RectPosition> bricks;
    std::vector<Brick> bricks;
    float sceneWidth;
    float sceneHeight;

    void addBrick(float x, float y, float width, float height) {
      Brick brick(x, y, width, height);
      this->bricks.push_back(brick);
    }

};

BrickScene createBrickScene(int rows, int cols) {
  const float brickPaddingX = 1.0f;
  const float brickPaddingY = 1.0f;

  BrickScene brickScene;

  for (int r=0; r<rows; r++) {
    for (int c=0; c<cols; c++) {
      float x = c*brickBaseWidth + c*brickPaddingX;
      float y = r*brickBaseHeight + r*brickPaddingY;
      float width = brickBaseWidth * brickScale;
      float height = brickBaseHeight * brickScale;
      // RectPosition brick(x, y, width, height);
      // brickScene.bricks.push_back(brick);
      brickScene.addBrick(x, y, width, height);
    }
  }

  /*
   Explanation of the below math since in a week's time this will look like voodoo.
   Let's say we have the follow details for our bricks (using horizontal/columns as the example;
   the logic is the same for vertical/rows)
   width (of a brick) = 5
   x pad (space between bricks) = 2
   (Asterisk indicates padding)
    _____    _____    _____
   |_____|**|_____|**|_____|
   0     5  7     12 14    19

   In this case, the "total brick width" is 19. As can be seen when drawn out, the total brick width
   is really just the x value of the right of the last brick.
   For each brick, the starting x value is calculated as: i*w + i*p (i=index of brick, w=width, p=pad)
   Therefore:
    0: 0*5 + 0*2 = 0
    1: 1*5 + 1*2 = 7
    2: 2*5 + 2*2 = 14

    So to get the desired "total brick width", we need to calculate the left x for the last brick
    and then add the width to get the right x:
    i*w + i*p + p
    Where in this scenario, i = columns-1:
    (c-1)*w + (c-1)*p + p
    This could be simplified to (c-1)*w + c*p, but it reflects the intuition a bit more to leave it unsimplified,
    so leaving it like that.
   */
  /* the "total brick width" is really just the x value of the right of the last column of bricks */
  float totalBrickWidth = (cols-1)*brickBaseWidth + (cols-1)*brickPaddingX + brickBaseWidth;
  /* the "total brick height" is really just the y value of the top of the last row of bricks */
  float totalBrickHeight = (rows-1)*brickBaseHeight + (rows-1)*brickPaddingY + brickBaseHeight;

  brickScene.sceneWidth = totalBrickWidth;
  brickScene.sceneHeight = totalBrickHeight;

  return brickScene;
}



BrickScene createBrickSceneFromFile(std::string filename) {
  std::string line;
  std::ifstream File(filename);
  std::vector<std::vector<bool>> layout; /* 2d list representing brick scene layout */
  while (getline(File, line)) {
    std::cout << line << std::endl;
    std::vector<bool> row;
    for (char& c : line) {
      bool isBrick = (c == '#');
      row.push_back(isBrick);
    }
    layout.push_back(row);
  }

  /* Reverse because (0,0) is bottom left of screen, so the verticality is reversed */
  std::reverse(layout.begin(), layout.end());

  const float brickPaddingX = 1.0f;
  const float brickPaddingY = 1.0f;

  int rows = layout.size();
  int cols = layout[0].size();

  BrickScene brickScene;
  for (int r=0; r<rows; r++) {
    for (int c=0; c<cols; c++) {
      if (!layout[r][c]) continue;

      float x = c*brickBaseWidth + c*brickPaddingX;
      float y = r*brickBaseHeight + r*brickPaddingY;
      float width = brickBaseWidth * brickScale;
      float height = brickBaseHeight * brickScale;

      // RectPosition brick(x, y, width, height);
      // brickScene.bricks.push_back(brick);
      brickScene.addBrick(x, y, width, height);
    }
  }

  float sceneWidth = (cols-1)*brickBaseWidth + (cols-1)*brickPaddingX + brickBaseWidth;
  float sceneHeight = (rows-1)*brickBaseHeight + (rows-1)*brickPaddingY + brickBaseHeight;

  brickScene.sceneWidth = sceneWidth;
  brickScene.sceneHeight = sceneHeight;

  return brickScene;

}

#endif /* BRICK_SCENE_INCLUDE_GUARD */
