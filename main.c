#include "main.h"

// io
#include <stdio.h>

// winapi
#include <Windows.h>
#include <WinUser.h>

// wiiuse
#include "lib/wiiuse.h"

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

void move_mouse();

int main() {
    return 1;
}