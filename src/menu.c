#include "../include/ct2.h"

#define FUNCS_PER_PAGE 8
#define UNITS_BETWEEN_LINES 12
#define X_COORD_PER_LETTER 9

enum Toggles {
    NO_TOGGLE = 0,
    //page 0
    TOGGLE_HIDE_SAVESTATE_TEXT
};

s8 toggles[] = {
    2,  // NO_TOGGLE
    //page 0
    1,  // TOGGLE_HIDE_SAVESTATE_TEXT
};

typedef struct menuPage {
    /* 0x00 */ s32 optionCount;
    /* 0x04 */ s32 pageIndex;
    /* 0x08 */ char* options[FUNCS_PER_PAGE];
    /* 0x28 */ s32 (*menuProc[FUNCS_PER_PAGE]) (void);
    /* 0x48 */ s8 flags[FUNCS_PER_PAGE];
    /* 0x50 */ char*** selectionText;
} menuPage;

s32 toggleHideSavestateText(void) {
    toggles[TOGGLE_HIDE_SAVESTATE_TEXT] ^= 1;
    return 1;
}

char string_ON[] = {
    "ON"
};

char string_OFF[] = {
    "OFF"
};

char* ONAndOFF[] = {
    string_OFF,
    string_ON,
};

char** page0Strings[] = {
    ONAndOFF, //Savestate Text Active text
};

menuPage page0 = {
    1, //optionCount
    0, //pageIndex
    { //options
        "Savestate Text",
    },
    { //menuProc
        &toggleHideSavestateText,
    },
    { //flags
        TOGGLE_HIDE_SAVESTATE_TEXT,
    },

    page0Strings,
};

menuPage* pageList[] = {
    &page0,
};

s32 pageListTotal = ARRAY_COUNT(pageList);

TextPosition MenuRoot = {100, 120};

char menuOptionBuffer[100] = { 0 };  // Buffer for menu options text


void updateMenuInput(void) {
    if (currentlyPressedButtons & CONT_UP) {
        if (currOptionNo > 0) {
            currOptionNo--;
            //playSound(0x2A, (void*)0x80168DA8, 0);
        } else {
            currOptionNo = pageList[currPageNo]->optionCount - 1; //wrap menu
        }
    }
    else if (currentlyPressedButtons & CONT_DOWN) {
        if (currOptionNo < pageList[currPageNo]->optionCount - 1) {
            currOptionNo++;
            //playSound(0x2A, (void*)0x80168DA8, 0);
        } else {
            currOptionNo = 0; //wrap menu
        }
    }
    else if (currentlyPressedButtons & CONT_LEFT) {
        if (currPageNo > 0) {
            currPageNo--;
            currOptionNo = 0;
        }
    }
    else if (currentlyPressedButtons & CONT_RIGHT) {
        if (currPageNo < pageListTotal - 1) {
            currPageNo++;
            currOptionNo = 0;
        }
    }
    else if (currentlyPressedButtons & B_BUTTON) {
        isMenuActive = 0;
    }
    else if (currentlyPressedButtons & A_BUTTON) {
        pageList[currPageNo]->menuProc[currOptionNo]();
    }
}

void pageMainDisplay(s32 currPageNo, s32 currOptionNo) {
    int i;
    menuPage* currPage = pageList[currPageNo];
    s32 strLength;
    TextPosition menu = MenuRoot;
    
    for (i = 0; i < currPage->optionCount; i++) {
        _bzero(&menuOptionBuffer, sizeof(menuOptionBuffer));
        _sprintf(menuOptionBuffer, "%s", currPage->options[i]);
        strLength = _strlen((u8*)&menuOptionBuffer);

        //print cursor current option as cyan
        if (i == currOptionNo) {
            SetDefaultTextParametersWithColor(&Cyan, menu.xPos, menu.yPos);
        } else {
            SetDefaultTextParametersWithColor(&White, menu.xPos, menu.yPos);
        }

        //if menu option has no toggle; it executes something when used immediately
        //therefore, only print the option and not any extra text
        if (currPage->flags[i] == -1) {
            printDebugText(menuOptionBuffer);
            menu.yPos += UNITS_BETWEEN_LINES; //each line is 12 units below the previous
            continue;
        }

        //print option text
        printDebugText(menuOptionBuffer);

        if (toggles[currPage->flags[i]] == 0) {
            SetDefaultTextParametersWithColor(&RedOrange, menu.xPos + (strLength * X_COORD_PER_LETTER), menu.yPos);
        } else {
            SetDefaultTextParametersWithColor(&Green, menu.xPos + (strLength * X_COORD_PER_LETTER), menu.yPos);
        }

        _bzero(&menuOptionBuffer, sizeof(menuOptionBuffer));
        _sprintf(menuOptionBuffer, currPage->selectionText[i][toggles[currPage->flags[i]]]);

        //print ON/OFF or other text next to option
        printDebugText(menuOptionBuffer);
        menu.yPos += UNITS_BETWEEN_LINES; //each line is 12 units below the previous  
    }
}