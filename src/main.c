#include "main.h"

// io
#include <stdio.h>

// winapi
#include <Windows.h>
#include <WinUser.h>

// wiiuse
#include "wiiuse.h"

int get_pos(POINT* cursor) {
    return GetCursorPos(cursor);
}

void update_mouse_button(INPUT* input) {
    // get mouse pos
    POINT cursor;
    get_pos(&cursor);

    input->type = INPUT_MOUSE;
    input->mi.dx = cursor.x;
    input->mi.dy = cursor.y;
    SendInput(1, input, sizeof(INPUT));
}

void left_mouse_down() {
    INPUT input;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    update_mouse_button(&input);
}

void left_mouse_up() {
    INPUT input;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    update_mouse_button(&input);
}

void right_mouse_down() {
    INPUT input;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    update_mouse_button(&input);
}

void right_mouse_up() {
    INPUT input;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    update_mouse_button(&input);
}

void move_mouse(int dx, int dy) {
    // mouse move input
    INPUT input;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    // get mouse pos
    POINT cursor;
    get_pos(&cursor);

    input.type = INPUT_MOUSE;
    input.mi.dx = dx;
    input.mi.dy = dy;
    SendInput(1, &input, sizeof(INPUT));
}

int main() {
    // connect wiimotes
    wiimote** wiimotes_found;
    int found, connected;
    wiimotes_found = wiiuse_init(1);
    // found = wiiuse_find(wiimotes_found, 1, 5);

    // if (!found) {
    //     printf("NO WIIMOTES FOUND\n");
    //     return 0;
    // }

    // // set wiimotes
    // wiimote* remote = wiimotes_found[0];
    // connected = wiiuse_connect(&remote, 1);
    // if (!connected) {
    //     printf("Failed to connect to remote\n");
    //     return 0;
    // }
    // wiiuse_set_leds(remote, 0x00);

    return 1;
}