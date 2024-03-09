#ifndef __MAIN_H
#define __MAIN_H

#include <Windows.h>
#include <WinUser.h>

int get_mouse_position(POINT *cursor);

void left_click_mouse();

void right_click_mouse();

void move_mouse();

int main();

#endif