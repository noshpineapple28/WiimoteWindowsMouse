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

void click_mouse(INPUT* input) {
    // get mouse pos
    POINT cursor;
    get_pos(&cursor);

    input->type = INPUT_MOUSE;
    input->mi.dx = cursor.x;
    input->mi.dy = cursor.y;
    SendInput(1, input, sizeof(INPUT));
}

void left_click_mouse() {
    INPUT input;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    click_mouse(&input);
}

void right_click_mouse() {
    INPUT input;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    click_mouse(&input);
}

void move_mouse();

int main() {
    right_click_mouse();

    return 1;
}