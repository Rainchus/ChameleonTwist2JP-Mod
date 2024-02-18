#include "ct2.h"

s32 prevOption = 0;
s32 holdDpadDirectionFrames = 0;

typedef struct MenuInstance {
    s16 cursor;
    s16 stageIndex;
    s16 unk_04;
    s16 unk_timer;
    u16 inputs;
} MenuInstance;

char* StageList[] = {
/* 0 */ "Sky Land",
/* 1 */ "Carnival Land",
/* 2 */ "Ice Land",
/* 3 */ "Great Edo Land",
/* 4 */ "Toy Land",
/* 5 */ "Pyramid Land"
/* 6 */ "",
/* 7 */ "",
/* 8 */ "",
/* 9 */ "",
/* 10 */ "",
/* 11 */ "Training Room 1",
/* 12 */ "Training Room 2",
/* 13 */ "Training Room 3",
/* 14 */ "Training Room 4",
/* 15 */ "Billiards",
/* 16 */ "Bowling",
/* 17 */ "Chess",
/* 18 */ "",
/* 19 */ "",
/* 20 */ "",
/* 21 */ "Mushroom Boss",
/* 22 */ "Burger Boss",
/* 23 */ "Wheelrus",
/* 24 */ "Frog Boss",
/* 25 */ "Robot Boss",
/* 26 */ "Sphynx Boss",
/* 27 */ "",
/* 28 */ "",
/* 29 */ "ContPak",
/* 30 */ "Unknown",
/* 31 */ "Credits 0",
/* 32 */ "Credits 1",
/* 33 */ "Credits 2",
/* 34 */ "Credits 3",
/* 35 */ "",
/* 36 */ "",
/* 37 */ "",
/* 38 */ "",
/* 39 */ "",
/* 40 */ "",
/* 41 */ "Sky Land Section 2",
/* 42 */ "Carnival Land Section 2",
/* 43 */ "Ice Hockey",
/* 44 */ "Edo Land Section 2",
/* 45 */ "Toy Land Section 2",
/* 46 */ "Pyramid Land Section 2",
/* 47 */ "",
/* 48 */ "Carnival After Teacup",
/* 49 */ "Ice Land Section 2",
};

char* MenuStrings0[] = {
    // "PROCESS MENU",
    "Game",
    "Object",
    "Sprite",
    "Texture",
    "Sound",
    "Opening",
    "Ending",
    "Title",
    "PackMenu",
    "FirstPack",
    "StageClr",
    "Minigame",
    "PackTest",
    "Game Over",
    "Intro",
    "Message",
    "Config",
};

TextPosition MenuTextPositions[] = {
// {20, 40},
{20, 45},
{20, 55},
{20, 65},
{20, 75},
{20, 85},
{20, 95},
{20, 105},
{20, 115},
{20, 125},
{20, 135},
{20, 145},
{20, 155},
{20, 165},
{20, 175},
{20, 185},
{20, 195},
{20, 205},
};

MenuInstance* MenuInstancePointer = 0;

void DisplayDebugMenu(void) {
    char buffer[100];
    int i;

    if (MenuInstancePointer == 0) {
        return;
    }

    if (MenuInstancePointer->stageIndex == 1 && prevOption != 0 && MenuInstancePointer->stageIndex != prevOption) {
        MenuInstancePointer->stageIndex = prevOption;
    }

    textStyle = 1;
    textKerning = 1;
    _bzero(buffer, sizeof(buffer));
    SetDebugTextPosition(MenuTextPositions[0].xPos, MenuTextPositions[0].yPos - 15, 0x50);
    SetTextWidthAndHeight(0.6f, 0.6f);
    SetTextColor2(255, 255, 255, 255);

    if (MenuInstancePointer->cursor == 0) {
        if (currentlyPressedButtons & L_JPAD) { //dpad left
            MenuInstancePointer->stageIndex--;
        } else if (currentlyPressedButtons & R_JPAD) { //dpad right
            MenuInstancePointer->stageIndex++;
        }

        if (currentlyHeldButtons & L_JPAD || currentlyHeldButtons & R_JPAD) {
            holdDpadDirectionFrames++;
        } else {
            holdDpadDirectionFrames = 0;
        }

        if (holdDpadDirectionFrames > 10) { //if held for 10 frames, quickly scroll
            if (currentlyHeldButtons & 0x200) { //dpad left
                MenuInstancePointer->stageIndex--;
            } else if (currentlyHeldButtons & 0x100) { //dpad right
                MenuInstancePointer->stageIndex++;
            }
        }


        if (currentlyHeldButtons & L_JPAD) { //dpad left
            if (MenuInstancePointer->stageIndex == 10) { //7, 8, 9, 10 are glitchy/crash
                MenuInstancePointer->stageIndex = 6;
            } else if (MenuInstancePointer->stageIndex == 20) {
                MenuInstancePointer->stageIndex = 17;
            } else if (MenuInstancePointer->stageIndex == 40) {
                MenuInstancePointer->stageIndex = 26;
            } else if (MenuInstancePointer->stageIndex == 47) {
                MenuInstancePointer->stageIndex = 46;
            }
        } else if (currentlyHeldButtons & R_JPAD) { //dpad right
            if (MenuInstancePointer->stageIndex == 7) { //8, 9, 10 are glitchy/crash
                MenuInstancePointer->stageIndex = 11;
            } else if (MenuInstancePointer->stageIndex == 18) {
                MenuInstancePointer->stageIndex = 21;
            } else if (MenuInstancePointer->stageIndex == 27) {
                MenuInstancePointer->stageIndex = 41;
            } else if (MenuInstancePointer->stageIndex == 47) {
                MenuInstancePointer->stageIndex = 48;
            }
        }
    }

    if (MenuInstancePointer->stageIndex > 49) {
        MenuInstancePointer->stageIndex = 49;
    }

    if (MenuInstancePointer->stageIndex < 1) {
        MenuInstancePointer->stageIndex = 1;
    }

    prevOption = MenuInstancePointer->stageIndex;

    _bzero(buffer, sizeof(buffer));
    SetDebugTextPosition(MenuTextPositions[0].xPos + 105, MenuTextPositions[0].yPos, 0x32);
    SetTextWidthAndHeight(0.6f, 0.6f);
    SetTextColor2(255, 255, 255, 255);

    _sprintf(buffer, "%s", StageList[MenuInstancePointer->stageIndex - 1]);
    PrintDebugText(buffer);
    
    for (i = 0; i < ARRAY_COUNT(MenuStrings0); i++) {
        _bzero(buffer, sizeof(buffer));
        SetDebugTextPosition(MenuTextPositions[i].xPos, MenuTextPositions[i].yPos, 0x32);
        SetTextWidthAndHeight(0.6f, 0.6f);
        SetTextColor2(255, 255, 255, 255);
    
        if (MenuInstancePointer->cursor == i) {
            SetTextColor2(255, 255, 0, 255);
        }
        
        if (i == 0) {
            _sprintf(buffer, "%s %s: %d", MenuStrings0[i], "Stage", MenuInstancePointer->stageIndex);
        } else {
            _sprintf(buffer, "%s", MenuStrings0[i]);
        }
        PrintDebugText(buffer); 
    }

    //setting it to 0 here will prevent a dangling pointer from occurring
    MenuInstancePointer = 0; //this gets set back to a pointer by some asm code.

}