#include "main.h"

// io
#include <stdio.h>

// winapi
#include <Windows.h>
#include <WinUser.h>

// wiiuse
#include "wiiuse.h"

// mouse pos
static int mouse_x = 0;
static int mouse_y = 0;

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

void send_mouse_wheel_scroll(int direction)
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.mouseData = WHEEL_DELTA * direction;
    // set mouse position
    SendInput(1, &input, sizeof(INPUT));
    SetCursorPos(mouse_x, mouse_y);
}


void send_key_input(int key, int is_down)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = is_down; // 0 || KEYEVENTF_EXTENDEDKEY
    input.ki.time = 0;
    SendInput(1, &input, sizeof(INPUT));
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
    INPUT input;

    // mouse move input
    if (dx >= 4000000)
        dx = 0;

    if (dy >= 4000000)
        dy = 0;

    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.dx = -1 * (mouse_x - dx);
    input.mi.dy = -1 * (mouse_y - dy);
    input.mi.time = 0;
    SendInput(1, &input, sizeof(INPUT));

    SetCursorPos(dx, dy);
    mouse_x = dx;
    mouse_y = dy;
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
    if (IS_JUST_PRESSED(remote, 0x0008)) // A
        send_key_input(VK_SPACE, 0);
    else if (IS_RELEASED(remote, 0x00008))
        send_key_input(VK_SPACE, KEYEVENTF_KEYUP);

    if (IS_JUST_PRESSED(remote, 0x0004)) // B
        send_key_input(VK_LSHIFT, 0);
    else if (IS_RELEASED(remote, 0x0004))
        send_key_input(VK_LSHIFT, KEYEVENTF_KEYUP);

    // inven key
    if (IS_JUST_PRESSED(remote, 0x0080)) // HOME
        send_key_input('E', 0);
    else if (IS_RELEASED(remote, 0x0080))
        send_key_input('E', KEYEVENTF_KEYUP);

    // wasd
    if (IS_JUST_PRESSED(remote, 0x0800)) // UP
        send_key_input('W', 0);
    else if (IS_RELEASED(remote, 0x0800))
        send_key_input('W', KEYEVENTF_KEYUP);
    if (IS_JUST_PRESSED(remote, 0x0400)) // DOWN
        send_key_input('S', 0);
    else if (IS_RELEASED(remote, 0x0400))
        send_key_input('S', KEYEVENTF_KEYUP);
    if (IS_JUST_PRESSED(remote, 0x0100)) // LEFT
        send_key_input('A', 0);
    else if (IS_RELEASED(remote, 0x0100))
        send_key_input('A', KEYEVENTF_KEYUP);
    if (IS_JUST_PRESSED(remote, 0x0200)) // RIGHT
        send_key_input('D', 0);
    else if (IS_RELEASED(remote, 0x0200))
        send_key_input('D', KEYEVENTF_KEYUP);

    // scroll weel
    if (IS_JUST_PRESSED(remote, 0x0002)) // 1
        send_mouse_wheel_scroll(1);
    if (IS_JUST_PRESSED(remote, 0x0001)) // 2
        send_mouse_wheel_scroll(-1);

    // left click
    if (IS_JUST_PRESSED(remote, 0x0010)) // minus
        left_mouse_down();
    else if (IS_RELEASED(remote, 0x0010))
        left_mouse_up();
    // mouse click
    if (IS_JUST_PRESSED(remote, 0x1000)) // plus
        right_mouse_down();
    else if (IS_RELEASED(remote, 0x1000))
        right_mouse_up();

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
            move_mouse((int)dx,(int)dy);
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