#include "main.h"

// io
#include <stdio.h>

// winapi
#include <Windows.h>
#include <WinUser.h>

// wiiuse
#include "wiiuse.h"

int get_pos(POINT *cursor)
{
    return GetCursorPos(cursor);
}

void update_mouse_button(INPUT *input)
{
    // get mouse pos
    POINT cursor;
    get_pos(&cursor);

    input->type = INPUT_MOUSE;
    input->mi.dx = cursor.x;
    input->mi.dy = cursor.y;
    input->mi.time = 0;
    SendInput(1, input, sizeof(INPUT));
}

void left_mouse_down()
{
    INPUT input;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    update_mouse_button(&input);
}

void left_mouse_up()
{
    INPUT input;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    update_mouse_button(&input);
}

void right_mouse_down()
{
    INPUT input;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    update_mouse_button(&input);
}

void right_mouse_up()
{
    INPUT input;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    update_mouse_button(&input);
}

void move_mouse(int dx, int dy)
{
    // mouse move input
    if (dx >= 4000000)
        dx = 0;

    if (dy >= 4000000)
        dy = 0;

    // printf("x: %u, y: %u\t", dx, dy);
    SetCursorPos(dx, dy);
}

wiimote *setup_remote()
{
    wiimote **wiimotes_found;
    int found, connected;
    wiimotes_found = wiiuse_init(1);
    found = wiiuse_find(wiimotes_found, 1, 5);

    if (!found)
    {
        printf("NO WIIMOTES FOUND\n");
        return 0;
    }

    // set wiimotes
    connected = wiiuse_connect(wiimotes_found, 1);
    if (!connected)
    {
        printf("Failed to connect to remote\n");
        return 0;
    }
    wiimote *remote = wiimotes_found[0];
    wiiuse_set_leds(remote, 0x00);

    return remote;
}

int handle_input(wiimote *remote)
{
    if (IS_JUST_PRESSED(remote, WIIMOTE_BUTTON_A))
    {
        left_mouse_down();
    }
    else if (IS_RELEASED(remote, WIIMOTE_BUTTON_A))
    {
        left_mouse_up();
    }
    if (IS_JUST_PRESSED(remote, WIIMOTE_BUTTON_B))
    {
        right_mouse_down();
    }
    else if (IS_RELEASED(remote, WIIMOTE_BUTTON_B))
    {
        right_mouse_up();
    }
    if (IS_PRESSED(remote, 0x0010))
    {
        return 1;
    }

    return 0;
}

short any_wiimote_connected(wiimote *wm, int wiimotes)
{
    if (!wm)
    {
        return 0;
    }
    if (wm && WIIMOTE_IS_CONNECTED(wm))
    {
        return 1;
    }

    return 0;
}

void read_remote(wiimote **remote)
{
    wiiuse_set_ir(remote[0], 1);
    wiiuse_motion_sensing(remote[0], 1);
    wiiuse_set_ir_vres(remote[0], 1920, 1080);
    while (any_wiimote_connected(remote[0], 1))
    {
        if (wiiuse_poll(remote, 1))
        {
            /*
             *  This happens if something happened on any wiimote.
             *  So go through each one and check if anything happened.
             */
            switch (remote[0]->event)
            {
            case WIIUSE_EVENT:
                /* a generic event occurred */
                if (handle_input(remote[0]))
                    return;
                break;

            default:
                break;
            }

            // handle movement
            int dx = remote[0]->ir.x;
            int dy = remote[0]->ir.y;
            move_mouse((int)dx, (int)dy);
        }
    }
}

int main()
{
    // connect wiimotes
    wiimote *remote = setup_remote();
    wiiuse_rumble(remote, 1);
    Sleep(200);
    wiiuse_rumble(remote, 0);
    read_remote(&remote);
    wiiuse_cleanup(&remote, 1);

    return 1;
}