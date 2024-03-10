#ifndef __MAIN_H
#define __MAIN_H

#include <Windows.h>
#include <WinUser.h>

int get_mouse_position(POINT *cursor);

void left_mouse_down();

void left_mouse_up();

void right_mouse_down();

void right_mouse_up();

void move_mouse();

int main();

#endif