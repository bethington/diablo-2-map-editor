#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// DirectInput device format constants - stub implementations
const DIDATAFORMAT c_dfDIMouse = {
    sizeof(DIDATAFORMAT),
    sizeof(DIOBJECTDATAFORMAT),
    0,
    0,
    0,
    NULL
};

const DIDATAFORMAT c_dfDIKeyboard = {
    sizeof(DIDATAFORMAT),
    sizeof(DIOBJECTDATAFORMAT),
    0,
    256,
    0,
    NULL
};

const DIDATAFORMAT c_dfDIJoystick = {
    sizeof(DIDATAFORMAT),
    sizeof(DIOBJECTDATAFORMAT),
    0,
    sizeof(DIJOYSTATE),
    0,
    NULL
};
