#include "main.h"

// io
#include <stdio.h>

// winapi
#include <Windows.h>
#include <WinUser.h>

// wiiuse
#include "wiiuse.h"
#include "nccf.h"

// mouse pos
static int mouse_x = 0;
static int mouse_y = 0;
static int loop_nunchuk = 0;
nccf_config configs = {0};

void no_op()
{
    loop_nunchuk = 0;
}

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
}

void send_key_input(int key, int is_down)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.dwExtraInfo = 0;
    input.ki.time = 0;
    input.ki.wVk = 0;
    input.ki.wScan = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
    input.ki.dwFlags = is_down; // 0 || KEYEVENTF_EXTENDEDKEY
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
    INPUT input = {0};

    // mouse move input
    if (dx >= 4000000)
        dx = mouse_x;

    if (dy >= 4000000)
        dy = mouse_y;

    // set change
    if (configs.mouse_pos == NUNCHUK)
    {
        // set dx
        input.mi.dx = (dx - mouse_x) * 1.5;
        input.mi.dy = (dy - mouse_y) * 1.5;
    }
    else if (configs.mouse_pos == WIIMOTE)
    {
        // right half
        if (mouse_x >= (1920 / 2) * 1.5)
        input.mi.dx = 5;
        else if (mouse_x >= (1920 / 2) * 1.75)
        input.mi.dx = 10;
        // left half
        else if (mouse_x <= (1920 / 2) * .75)
        input.mi.dx = -5;
        else if (mouse_x <= (1920 / 2) * .5)
        input.mi.dx = -10;
        else
        input.mi.dx = 0;
        // set dy
        // bottom half
        if (mouse_y >= (1080 / 2) * 1.5)
        input.mi.dy = 5;
        else if (mouse_y >= (1080 / 2) * 1.75)
        input.mi.dy = 10;
        // top half
        else if (mouse_y <= (1080 / 2) * .75)
        input.mi.dy = -5;
        else if (mouse_y <= (1080 / 2) * .5)
        input.mi.dy = -10;
        else
        input.mi.dy = 0;
    }

    // set cursor pos
    mouse_x = dx;
    mouse_y = dy;
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    // input.mi.dx = -1 * (mouse_x - dx);
    // input.mi.dy = -1 * (mouse_y - dy);
    input.mi.time = 0;
    SendInput(1, &input, sizeof(INPUT));

    SetCursorPos(dx, dy);
    // mouse_x = dx;
    // mouse_y = dy;
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
    wiiuse_set_leds(remote, 0b1001);

    return remote;
}

int handle_joystick_mouse(float x, float y)
{
    // held states
    static short held_north = 0;
    static short held_east = 0;
    static short held_south = 0;
    static short held_west = 0;

    // thresholds
    float threshhold = 0.5;
    float speed = (float)configs.nunchuck_conf.SPEED * 0.1;
    // get mouse pos
    POINT cursor;
    // get_pos(&cursor);
    int dx = mouse_x;
    int dy = mouse_y;

    // x pos held
    if (x > threshhold)
    {
        held_east = 1;
        dx += speed;
    }
    if (x < -1 * threshhold)
    {
        held_west = 1;
        dx -= speed;
    }

    // x pos released
    if (x <= threshhold && x >= -1 * threshhold)
    {
        if (held_west)
        {
            held_west = 0;
        }
        if (held_east)
        {
            held_east = 0;
        }
    }

    // y pos held
    if (y > threshhold)
    {
        held_north = 1;
        dy -= speed;
    }
    if (y < -1 * threshhold)
    {
        held_south = 1;
        dy += speed;
    }

    // y pos released
    if (y <= threshhold && y >= -1 * threshhold)
    {
        if (held_north)
        {
            held_north = 0;
        }
        if (held_south)
        {
            held_south = 0;
        }
    }

    if (dx != mouse_x || dy != mouse_y)
        move_mouse(dx, dy);

    return 1;
}

int handle_input_wiimote(wiimote *remote, int button, int button_mask)
{
    switch (button)
    {
    case MOUSE_LEFT:
        if (IS_JUST_PRESSED(remote, button_mask))
            left_mouse_down();
        else if (IS_RELEASED(remote, button_mask))
            left_mouse_up();
        break;
    case MOUSE_RIGHT:
        if (IS_JUST_PRESSED(remote, button_mask))
            right_mouse_down();
        else if (IS_RELEASED(remote, button_mask))
            right_mouse_up();
        break;
    case SCROLL_UP_MOUSE:
        if (IS_JUST_PRESSED(remote, button_mask))
            send_mouse_wheel_scroll(1);
        break;
    case SCROLL_DOWN_MOUSE:
        if (IS_JUST_PRESSED(remote, button_mask))
            send_mouse_wheel_scroll(-1);
        break;
    case NO_INPUT:
        break;
    default:
        if (IS_JUST_PRESSED(remote, button_mask))
            send_key_input(button, KEYEVENTF_SCANCODE);
        else if (IS_RELEASED(remote, button_mask))
            send_key_input(button, KEYEVENTF_KEYUP);
        break;
    }
}

int handle_input_sans_wiimote(wiimote *remote, int button, int released)
{
    switch (button)
    {
    case MOUSE_LEFT:
        if (!released)
            left_mouse_down();
        else
            left_mouse_up();
        break;
    case MOUSE_RIGHT:
        if (!released)
            right_mouse_down();
        else
            right_mouse_up();
        break;
    case SCROLL_UP_MOUSE:
        if (!released)
            send_mouse_wheel_scroll(1);
        break;
    case SCROLL_DOWN_MOUSE:
        if (!released)
            send_mouse_wheel_scroll(-1);
        break;
    case NO_INPUT:
        break;
    default:
        if (!released)
            send_key_input(button, KEYEVENTF_SCANCODE);
        else
            send_key_input(button, KEYEVENTF_KEYUP);
        break;
    }
}

int handle_joystick_keys(float x, float y)
{
    // held states
    static short held_north = 0;
    char north_key = configs.nunchuck_conf.UP_ANALOG;
    static short held_east = 0;
    char east_key = configs.nunchuck_conf.RIGHT_ANALOG;
    static short held_south = 0;
    char south_key = configs.nunchuck_conf.DOWN_ANALOG;
    static short held_west = 0;
    char west_key = configs.nunchuck_conf.LEFT_ANALOG;

    // thresholds
    float threshhold = 0.5;

    // x pos held
    if (x > threshhold)
    {
        held_east = 1;
        send_key_input(east_key, KEYEVENTF_SCANCODE);
    }
    if (x < -1 * threshhold)
    {
        held_west = 1;
        send_key_input(west_key, KEYEVENTF_SCANCODE);
    }

    // x pos released
    if (x <= threshhold && x >= -1 * threshhold)
    {
        if (held_west)
        {
            held_west = 0;
            send_key_input(west_key, KEYEVENTF_KEYUP);
        }
        if (held_east)
        {
            held_east = 0;
            send_key_input(east_key, KEYEVENTF_KEYUP);
        }
    }

    // y pos held
    if (y > threshhold)
    {
        held_north = 1;
        send_key_input(north_key, KEYEVENTF_SCANCODE);
    }
    if (y < -1 * threshhold)
    {
        held_south = 1;
        send_key_input(south_key, KEYEVENTF_SCANCODE);
    }

    // y pos released
    if (y <= threshhold && y >= -1 * threshhold)
    {
        if (held_north)
        {
            held_north = 0;
            send_key_input(north_key, KEYEVENTF_KEYUP);
        }
        if (held_south)
        {
            held_south = 0;
            send_key_input(south_key, KEYEVENTF_KEYUP);
        }
    }

    return 1;
}

int handle_input(wiimote *remote)
{
    // WIIMOTE BUTTONS
    handle_input_wiimote(remote, configs.wiimote_conf.A, 0x0008);
    handle_input_wiimote(remote, configs.wiimote_conf.B, 0x0004);
    handle_input_wiimote(remote, configs.wiimote_conf.UP_D, 0x0800);
    handle_input_wiimote(remote, configs.wiimote_conf.DOWN_D, 0x0400);
    handle_input_wiimote(remote, configs.wiimote_conf.LEFT_D, 0x0100);
    handle_input_wiimote(remote, configs.wiimote_conf.RIGHT_D, 0x0200);
    handle_input_wiimote(remote, configs.wiimote_conf.ONE, 0x0002);
    handle_input_wiimote(remote, configs.wiimote_conf.HOME, 0x0080);
    handle_input_wiimote(remote, configs.wiimote_conf.TWO, 0x0001);
    handle_input_wiimote(remote, configs.wiimote_conf.MINUS, 0x0010);
    handle_input_wiimote(remote, configs.wiimote_conf.PLUS, 0x1000);

    // nunchuk buttons
    // holds if buttons were just released
    static short z_down = 0;
    static short c_down = 0;

    if ((remote->exp.nunchuk.btns_held) & 0x01) // z
    {
        if (!z_down)
            handle_input_sans_wiimote(remote, configs.nunchuck_conf.Z, z_down);
        z_down = 1;
    }
    else if (z_down)
    {
        z_down = 0;
        handle_input_sans_wiimote(remote, configs.nunchuck_conf.Z, z_down);
    }

    // handle the nunchuk joystick if not in NUNCHUK mode and c isn't down
    // c and joystick should never be handled if configs set mouse mode to NUNCHUK
    if (configs.mouse_pos == NUNCHUK && ((remote->exp.nunchuk.btns_held) & 0x02))
        return 0;

    if ((remote->exp.nunchuk.btns_held) & 0x02) // c
    {
        if (!c_down)
            handle_input_sans_wiimote(remote, configs.nunchuck_conf.C, c_down);
        c_down = 1;
    }
    else if (c_down)
    {
        c_down = 0;
        handle_input_sans_wiimote(remote, configs.nunchuck_conf.C, c_down);
    }

    handle_joystick_keys(remote->exp.nunchuk.js.x, remote->exp.nunchuk.js.y);

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

            switch (configs.mouse_pos)
            {
            // handle movement via IR sensor
            case WIIMOTE:
                int dx = remote[0]->ir.x;
                int dy = remote[0]->ir.y;
                move_mouse((int)dx, (int)dy);
                break;
            // handle movement via NUNCHUK sensor
            case NUNCHUK:
                // joystick inputs
                // to make it sem like the nunchuk is always moving, infinitely do this until new update occurs
                if ((remote[0]->exp.nunchuk.btns_held) & 0x02)
                {
                    handle_joystick_mouse(remote[0]->exp.nunchuk.js.x, remote[0]->exp.nunchuk.js.y);
                }
                break;
            }
        }

        // continue move outside of poll to make mouse seamless
        // joystick inputs
        if (configs.mouse_pos == NUNCHUK && (remote[0]->exp.nunchuk.btns_held) & 0x02)
        {
            handle_joystick_mouse(remote[0]->exp.nunchuk.js.x, remote[0]->exp.nunchuk.js.y);
        }
    }
}

void main(int argc, char *argv[])
{
    // connect wiimotes
    char *config_path = argv[argc - 1];
    parse_nccf(&configs, config_path);

    wiimote *remote = setup_remote();

    wiiuse_rumble(remote, 1);
    Sleep(200);
    wiiuse_rumble(remote, 0);
    wiiuse_set_leds(remote, configs.LED_STATE);
    read_remote(&remote);
    wiiuse_cleanup(&remote, 1);

    return 1;
}
