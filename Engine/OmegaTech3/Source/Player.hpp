#include "raylib.h"

class Player {
  public:
    bool ObjectCollision = false;
    float Height = 10.0f;
    float Width = 2.0f;

    int Health = 100;

    float OldX = 0.0f;
    float OldY = 9.0f;
    float OldZ = 0.0f;

    BoundingBox PlayerBounds;
};

static Player OTPlayer;