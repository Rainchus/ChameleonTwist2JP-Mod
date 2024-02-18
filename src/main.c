#include "ct2.h"
#include <stdarg.h>

Vec3f prevPlayerPosition = {0};

s32 stateCooldown = 0;
volatile s32 isSaveOrLoadActive = 0;
s32 currPageNo = 0;
s32 currOptionNo = 0;
s32 isMenuActive = 0;
s32 menuCurRespawnZone = 1;
u32 callsEnteringSkyland = 0;
u32 calls = 0;
s32 zoneLockout = 4;

CustomThread gCustomThread = {0};

void savestateMain(void);
void loadstateMain(void);
void UpdateMenuInput(void);
void PageMainDisplay(s32 currPageNo, s32 currOptionNo);
void LimitZone(void);

void SetTextColor2(u8 red, u8 blue, u8 green, u8 alpha) {
    textRed = red;
    textBlue = blue;
    textGreen = green;
    textOpacity = alpha;
    ifTextColor = 1;
}

int __osPiDeviceBusy() {
    register u32 stat = IO_READ(PI_STATUS_REG);
    if (stat & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY))
        return 1;
    return 0;
}

void SetTextColor(TextColor* color) {
    SetTextGradient(0, color->zero.r, color->zero.g, color->zero.b, color->zero.a); //args: unk, r, g, b, a
    SetTextGradient(1, color->one.r, color->one.g, color->one.b, color->one.a); //args: unk, r, g, b, a
    SetTextGradient(2, color->two.r, color->two.g, color->two.b, color->two.a); //args: unk, r, g, b, a
    SetTextGradient(3, color->three.r, color->three.g, color->three.b, color->three.a); //args: unk, r, g, b, a
    ifTextColor = 1;
}

void SetTextWidthAndHeight(f32 width, f32 height) {
    textWidth = width;
    textHeight = height;
}

void SetDefaultTextParametersWithColor(TextColor* color, s32 x, s32 y) {
    textStyle = 1;
    textKerning = 1;
    SetDebugTextPosition(x, y, 0x32);
    SetTextWidthAndHeight(0.75f, 0.75f);
    SetTextColor(color);
}

void printCurrentRespawnZone(void) {
    char buffer[40];
    TextPosition textPos = {20, 196};

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "Zone: %02d", respawnZone);
    PrintDebugText(buffer);
}

float CalculateDistance(Vec3f* p1, Vec3f* p2);

void printCurrentSpeed(void) {
    char buffer[40];
    TextPosition textPos = {20, 210};
    f32 curSpeed = CalculateDistance(&prevPlayerPosition, &gPlayerActors[0].pos);
    f32* temp = (f32*)0x807FFFFC;
    *temp = curSpeed;


    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);

    _sprintf(buffer, "%2.2f", curSpeed);
    PrintDebugText(buffer);
}

void PrintPosition(void) {
    TextPosition textPos = {20, 87};
    char buffer[100];
    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&Purple, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "X: %2.2f", gPlayerActors[0].pos.x);
    PrintDebugText(buffer);

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&Purple, textPos.xPos, textPos.yPos + 12);
    _sprintf(buffer, "Y: %2.2f", gPlayerActors[0].pos.y);
    PrintDebugText(buffer); 

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&Purple, textPos.xPos, textPos.yPos + 24);
    _sprintf(buffer, "Z: %2.2f", gPlayerActors[0].pos.z);
    PrintDebugText(buffer); 
}

void PrintFps(s32 x, s32 y, TextColor* color) {
    f32 fps = calculate_and_update_fps();
    char buffer[16];

    _bzero(buffer, sizeof(buffer));
    _sprintf(buffer, "%2.2f", fps);
    SetDefaultTextParametersWithColor(color, x, y);
    PrintDebugText(buffer);
}

void PrintMenuDisplays(void) {
    // //dpad down resets lag counter
    if (!(currentlyHeldButtons & R_TRIG) && currentlyPressedButtons & D_JPAD) {
        frame_count = __osVIIntrCount / 2; //reset lag counter
    }

    if (toggles[TOGGLE_DISPLAY_SPEED]) {
        printCurrentSpeed();
    }

    if (toggles[TOGGLE_DISPLAY_POSITION]) {
        PrintPosition();
    }

    if (toggles[TOGGLE_DISPLAY_FPS]) {
        PrintFps(260, 207, &Cyan);
    }

    if (toggles[TOGGLE_DISPLAY_LAG_FRAMES]) {
        print_lag_frames(200, 207, &Green);
    }

    if (toggles[TOGGLE_DISPLAY_ZONE]) {
        printCurrentRespawnZone();
    }

    prevPlayerPosition.x = gPlayerActors[0].pos.x;
    prevPlayerPosition.y = gPlayerActors[0].pos.y;
    prevPlayerPosition.z = gPlayerActors[0].pos.z;
}

void printCustomTextInC(void) {

    //MainTimer();
    PrintMenuDisplays();

    if (currPageNo == 2 && isMenuActive == 1) {
        char buffer[8];
        TextPosition textPos = {130, 45};

        _bzero(buffer, sizeof(buffer));
        SetDefaultTextParametersWithColor(&Green, textPos.xPos, textPos.yPos);

        _sprintf(buffer, "%d", menuCurRespawnZone);
        PrintDebugText(buffer);        
    }

    if (isMenuActive == 1) {
        UpdateMenuInput();
        PageMainDisplay(currPageNo, currOptionNo);
    }

    if (callsEnteringSkyland == 0) {
        callsEnteringSkyland = calls;
    }
}

void CheckZoneWarpButtons(void) {
    if (currPageNo == 2) { //is zone page
        if (currentlyHeldButtons & R_TRIG && currentlyPressedButtons & R_JPAD) {
            menuCurRespawnZone++;
        } else if (currentlyHeldButtons & R_TRIG && currentlyPressedButtons & L_JPAD) {
            menuCurRespawnZone--;
            if (menuCurRespawnZone == 0) {
                menuCurRespawnZone = 1;
            }
        } else if (currentlyHeldButtons & L_TRIG && currentlyHeldButtons & R_JPAD) {
            menuCurRespawnZone++;
        } else if (currentlyHeldButtons & L_TRIG && currentlyHeldButtons & L_JPAD) {
            menuCurRespawnZone--;
            if (menuCurRespawnZone == 0) {
                menuCurRespawnZone = 1;
            }
        }
        LimitZone();
    }
}

void BeforeMainLoop(void) {
    debugFlag = 1;
    D_80160648 = 0x3F; //unlock all levels

    CheckZoneWarpButtons();

    //R + dpad down toggles menu
    if (currentlyHeldButtons & R_TRIG && currentlyPressedButtons & CONT_DOWN) {
        isMenuActive ^= 1;
    }

    if (gGameMode != 0xC && isMenuActive == 0) { //if not in debug menu and custom menu not active, allow states
        if (currentlyPressedButtons & L_JPAD) {
            isSaveOrLoadActive = 1;
            osCreateThread(&gCustomThread.thread, 255, (void*)savestateMain, NULL,
                    gCustomThread.stack + sizeof(gCustomThread.stack), 255);
            osStartThread(&gCustomThread.thread);
            stateCooldown = 3;
        } else if (currentlyPressedButtons & R_JPAD) {
            isSaveOrLoadActive = 1;
            osCreateThread(&gCustomThread.thread, 255, (void*)loadstateMain, NULL,
                    gCustomThread.stack + sizeof(gCustomThread.stack), 255);
            osStartThread(&gCustomThread.thread);
            currentlyPressedButtons = 0;
            stateCooldown = 3;
        }
    }

    while (isSaveOrLoadActive == 1) {}
}
