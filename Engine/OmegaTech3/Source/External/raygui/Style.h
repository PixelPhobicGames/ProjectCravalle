
#include <cstring>
#include <cstdlib>

#include "raygui.h"

static const GuiStyleProp StyleProps[22] = {
    { 0, 0, 0xffffffff },    // DEFAULT_BORDER_COLOR_NORMAL
    { 0, 1, 0x030000ff },    // DEFAULT_BASE_COLOR_NORMAL
    { 0, 2, 0xffffffff },    // DEFAULT_TEXT_COLOR_NORMAL
    { 0, 3, 0x5d5d5dff },    // DEFAULT_BORDER_COLOR_FOCUSED
    { 0, 4, 0xffffffff },    // DEFAULT_BASE_COLOR_FOCUSED
    { 0, 5, 0x4e4c4cff },    // DEFAULT_TEXT_COLOR_FOCUSED
    { 0, 6, 0xffffffff },    // DEFAULT_BORDER_COLOR_PRESSED
    { 0, 7, 0x434343ff },    // DEFAULT_BASE_COLOR_PRESSED
    { 0, 8, 0x010101ff },    // DEFAULT_TEXT_COLOR_PRESSED
    { 0, 9, 0x707070FF },    // DEFAULT_BORDER_COLOR_DISABLED
    { 0, 10, 0x999999FF },   // DEFAULT_BASE_COLOR_DISABLED
    { 0, 11, 0x8C8C8CFF },   // DEFAULT_TEXT_COLOR_DISABLED
    { 0, 16, 1},           // DEFAULT_TEXT_SIZE
    { 0, 17, 1 },            // DEFAULT_TEXT_SPACING
    { 0, 18, 0xffffffff },   // DEFAULT_LINE_COLOR
    { 0, 19, 0x252525ff },   // DEFAULT_BACKGROUND_COLOR
    { 1, 5, 0xB0B0B0FF },    // LABEL_TEXT_COLOR_FOCUSED
    { 1, 8, 0xC0C0C0FF },    // LABEL_TEXT_COLOR_PRESSED
    { 4, 5, 0xA0A0A0FF },    // SLIDER_TEXT_COLOR_FOCUSED
    { 5, 5, 0x909090FF },    // PROGRESSBAR_TEXT_COLOR_FOCUSED
    { 9, 5, 0xD0D0D0FF },    // TEXTBOX_TEXT_COLOR_FOCUSED
    { 10, 5, 0xC8C8C8FF },   // VALUEBOX_TEXT_COLOR_FOCUSED
};

static void GuiLoadStyle(void)
{
    for (int i = 0; i < 22; i++)
    {
        GuiSetStyle(StyleProps[i].controlId, StyleProps[i].propertyId, StyleProps[i].propertyValue);
    }

    Font font = LoadFont("GameData/Global/GuiFont.ttf");

    GuiSetFont(font);

}
