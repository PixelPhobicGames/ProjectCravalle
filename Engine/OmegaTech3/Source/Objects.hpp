#include "libPPG/PPGIO/PPGIO.hpp"



// Inventory System

// Working on a Remaster

class GameObject {
  public:
};

static GameObject OmegaTechGameObjects;


static int SelectedObject = 1;

void UpdateUI() {

    // Implement Your UI Here

    if (IsKeyPressed(KEY_LEFT)) {
        if (SelectedObject != 1)
            SelectedObject--;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        if (SelectedObject != 8)
            SelectedObject++;
    }

    if (GetMouseWheelMove() != 0) {
        if (GetMouseWheelMove() < 0) {
            if (SelectedObject != 8)
                SelectedObject++;
        } else {
            if (SelectedObject != 1)
                SelectedObject--;
        }
    }

    if (IsGamepadAvailable(0)) {
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
            if (SelectedObject != 8)
                SelectedObject++;
        }
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {
            if (SelectedObject != 1)
                SelectedObject--;
        }
    }
}