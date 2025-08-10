#include "../include/ct2.h"

#define FUNCS_PER_PAGE 8
#define UNITS_BETWEEN_LINES 14
#define X_COORD_PER_LETTER 9

void SaveSettings(void);

typedef struct menuPage {
    /* 0x00 */ s32 optionCount;
    /* 0x04 */ s32 pageIndex;
    /* 0x08 */ char* options[FUNCS_PER_PAGE];
    /* 0x28 */ s32 (*menuProc[FUNCS_PER_PAGE]) (void);
    /* 0x48 */ s8 flags[FUNCS_PER_PAGE];
    /* 0x50 */ char*** selectionText;
} menuPage;

s32 toggleDisplaySpeed(void) {
    toggles[TOGGLE_DISPLAY_SPEED] ^= 1;
    return 1;
}

s32 toggleDisplayPosition(void) {
    toggles[TOGGLE_DISPLAY_POSITION] ^= 1;
    return 1;
}

s32 togglePowerupLock(void) {
    toggles[TOGGLE_POWERUP_LOCK]++;
    if (toggles[TOGGLE_POWERUP_LOCK] > 8) {
        toggles[TOGGLE_POWERUP_LOCK] = 0;
    }
    return 1;
}

s32 toggleDisplayFlutterFrames(void) {
    toggles[TOGGLE_DISPLAY_FLUTTER_FRAMES] ^= 1;
    return 1;
}

s32 toggleFpsDisplay(void) {
    toggles[TOGGLE_DISPLAY_FPS] ^= 1;
    return 1;
}

s32 toggleLagFramesDisplay(void) {
    toggles[TOGGLE_DISPLAY_LAG_FRAMES] ^= 1;
    return 1;
}

s32 toggleInfHealth(void) {
    toggles[TOGGLE_INF_HEALTH] ^= 1;
    return 1;
}

s32 toggleZoneDisplay(void) {
    toggles[TOGGLE_DISPLAY_ZONE] ^= 1;
    return 1;
}

extern s32 menuCurRespawnZone;
extern s32 zoneLockout;
u32 gCurZone = 0;

s32 ZoneWarp(void) {
    gRespawnZone = menuCurRespawnZone;
    gZoneChange = 1;
    zoneLockout = 4;
    gPlayerActors[0].pos.y = -10000.0f;
    return 1;
}

s32 ToggleTimer(void) {
    toggles[TOGGLE_TIMER_MAIN] ^= 1;
    return 1;    
}

s32 ToggleTimerIfDisplayOnVoid(void) {
    toggles[TOGGLE_TIMER_DISPLAY_VOID] ^= 1;
    return 1;    
}

s32 ToggleTimerIfDisplayedZoneChange(void) {
    toggles[TOGGLE_TIMER_DISPLAY_ZONE_CHANGE] ^= 1;
    return 1; 
}

u32 gTimer = 0;
u32 gTimerCopy = 0;

void TimerTickMainC(void) {
    gTimer++;
}

void TimerResetMainC(void) {
    gTimer = 0;
}

char string_blank[] = {
    ""
};

char string_ON[] = {
    "On"
};

char string_OFF[] = {
    "Off"
};

char string_Powerup_Big[] = { //0
    "Big"
};

char string_Powerup_Speed_Up[] = { //1
    "Speed Up"
};

char string_Powerup_Speed_Down[] = { //2
    "Speed Down"
};

char string_Powerup_x2[] = { //3
    "x2"
};

char string_Powerup_x3[] = { //4
    "x3"
};

char string_Powerup_Invicible[] = { //5
    "Invicible"
};

char string_Powerup_Short_Tongue[] = { //6
    "Short Tongue"
};

char string_Powerup_Nothing[] = { //7
    "Nothing"
};

char string_Powerup_Normal[] = { //8
    "Normal"
};

char* PowerupStrings[] = {
    string_Powerup_Big,
    string_Powerup_Speed_Up,
    string_Powerup_Speed_Down,
    string_Powerup_x2,
    string_Powerup_x3,
    string_Powerup_Invicible,
    string_Powerup_Short_Tongue,
    string_Powerup_Nothing,
    string_Powerup_Normal
};

char* ONAndOFF[] = {
    string_OFF,
    string_ON,
};

char* blankEntry[] = {
    string_blank,
    string_blank,
};

char** page0Strings[] = {
    ONAndOFF, //Display speed
    ONAndOFF, //Display position
    ONAndOFF, //Display flutter frames
    blankEntry, //save settings option
};

char** page1Strings[] = {
    PowerupStrings,
    ONAndOFF,
    ONAndOFF,
    ONAndOFF,
    ONAndOFF
};

char** page2Strings[] = {
    ONAndOFF,
};

char** page3Strings[] = {
    ONAndOFF,
    ONAndOFF,
    ONAndOFF,
};

menuPage page0 = {
    4, //optionCount
    0, //pageIndex
    { //options
        "Show Speed",
        "Show Position",
        "Show Flutter Frames",
        "Save Settings"
    },
    { //menuProc
        &toggleDisplaySpeed,
        &toggleDisplayPosition,
        &toggleDisplayFlutterFrames,
        &SaveSettings,
    },
    { //flags
        TOGGLE_DISPLAY_SPEED,
        TOGGLE_DISPLAY_POSITION,
        TOGGLE_DISPLAY_FLUTTER_FRAMES,
        -1,
    },

    page0Strings,
};

menuPage page1 = {
    5, //optionCount
    1, //pageIndex
    { //options
        "PowerUp Lock",
        "Fps",
        "Lag Frames",
        "Show Zone",
        "Inf Health"
    },
    { //menuProc
        &togglePowerupLock,
        &toggleFpsDisplay,
        &toggleLagFramesDisplay,
        &toggleZoneDisplay,
        &toggleInfHealth
    },
    { //flags
        TOGGLE_POWERUP_LOCK,
        TOGGLE_DISPLAY_FPS,
        TOGGLE_DISPLAY_LAG_FRAMES,
        TOGGLE_DISPLAY_ZONE,
        TOGGLE_INF_HEALTH
    },

    page1Strings,
};

menuPage page2 = {
    1, //optionCount
    2, //pageIndex
    { //options
        "Warp To Zone",
    },
    { //menuProc
        &ZoneWarp,
    },
    { //flags
        -1,
    },

    page2Strings,
};

menuPage page3 = {
    3, //optionCount
    3, //pageIndex
    { //options
        "Timer Display",
        "Show Time on Void",
        "Timer 2",
    },
    { //menuProc
        &ToggleTimer,
        &ToggleTimerIfDisplayOnVoid,
        &ToggleTimerIfDisplayedZoneChange,
    },
    { //flags
        TOGGLE_TIMER_MAIN,
        TOGGLE_TIMER_DISPLAY_VOID,
        TOGGLE_TIMER_DISPLAY_ZONE_CHANGE
    },

    page3Strings,
};

menuPage* pageList[] = {
    &page0,
    &page1,
    &page2,
    &page3,
};

s32 pageListTotal = ARRAY_COUNT(pageList);

TextPosition MenuRoot = {20, 45};
char menuOptionBuffer[100] = { 0 };  // Buffer for menu options text

void updateMenuInput(void) {
    if (currentlyPressedButtons & CONT_UP) {
        if (currOptionNo > 0) {
            currOptionNo--;
        } else {
            currOptionNo = pageList[currPageNo]->optionCount - 1; //wrap menu
        }
    }
    else if (currentlyPressedButtons & CONT_DOWN) {
        if (currOptionNo < pageList[currPageNo]->optionCount - 1) {
            currOptionNo++;
        } else {
            currOptionNo = 0; //wrap menu
        }
    }
    else if (currentlyPressedButtons & CONT_LEFT && !(currentlyHeldButtons & R_TRIG) && !(currentlyHeldButtons & L_TRIG)) {
        if (currPageNo > 0) {
            currPageNo--;
            currOptionNo = 0;
        }
    }
   else if (currentlyPressedButtons & CONT_RIGHT && !(currentlyHeldButtons & R_TRIG) && !(currentlyHeldButtons & L_TRIG)) {
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

        // if (toggles[currPage->flags[i]] == 0) {
        //     SetDefaultTextParametersWithColor(&RedOrange, menu.xPos + (strLength * X_COORD_PER_LETTER), menu.yPos);
        // } else {
        //     SetDefaultTextParametersWithColor(&Green, menu.xPos + (strLength * X_COORD_PER_LETTER), menu.yPos);
        // }

        SetDefaultTextParametersWithColor(&Green, menu.xPos + (strLength * X_COORD_PER_LETTER), menu.yPos);

        _bzero(&menuOptionBuffer, sizeof(menuOptionBuffer));
        _sprintf(menuOptionBuffer, currPage->selectionText[i][toggles[currPage->flags[i]]]);

        //print ON/OFF or other text next to option
        printDebugText(menuOptionBuffer);
        menu.yPos += UNITS_BETWEEN_LINES; //each line is 12 units below the previous  
    }
}