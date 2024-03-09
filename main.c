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

void left_click_mouse();

void right_click_mouse();

void move_mouse();

int main() {
    POINT cursor;
    if (get_pos(&cursor))
        printf("X: %u, Y: %u\n", cursor.x, cursor.y);

    return 1;
}