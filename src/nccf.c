#include "nccf.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// winapi
#include <Windows.h>
#include <WinUser.h>

// returns EOF or \n
int read_line(FILE *p, char *BUFFER)
{
    int i = 0;
    char c = 0;
    do
    {
        c = fgetc(p);
        if (c == '\n' || c == EOF)
        {
            BUFFER[i++] = 0;
            break;
        }
        BUFFER[i++] = c;
    } while (c != '\n' && c != EOF);
    return c;
}

int strip_line(char *BUFFER)
{
    // start at end work way back
    int len = strlen(BUFFER);
    int removed = 0;
    while (BUFFER[len - 1] == ' ')
    {
        BUFFER[len - 1] = BUFFER[len];
        len--;
        removed++;
    }
    return removed;
}

PARSE_STATE parse_state(char *BUFFER)
{
    if (!strcmp(BUFFER, "MOUSE_POS:"))
        return IN_POS;
    else if (!strcmp(BUFFER, "WIIMOTE:"))
        return IN_WIIMOTE;
    else if (!strcmp(BUFFER, "NUNCHUK:"))
        return IN_NUNCHUCK;
    else if (!strcmp(BUFFER, "LED_SETTINGS:"))
        return IN_LED;
    else
        return NO_PARSE;
}

void get_option(char *BUFFER, char *NAME, char *VAL)
{
    int i = 1;
    int option_i = 0;
    while (BUFFER[i] != ':')
    {
        NAME[option_i++] = BUFFER[i++];
    }
    NAME[option_i] = 0;
    i += 2;
    option_i = 0;
    while (BUFFER[i] != '\0')
    {
        VAL[option_i++] = BUFFER[i++];
    }
    VAL[option_i] = 0;
}

int parse_vk_option(char *BUFFER)
{
    int option = 0;
    // TODO: disallowed until i figure out whats wrong
    // if (!strcmp(BUFFER, "DOWN_ARROW"))
    // {
    //     option = VK_DOWN;
    // }
    // else if (!strcmp(BUFFER, "LEFT_ARROW"))
    // {
    //     option = VK_LEFT;
    // }
    // else if (!strcmp(BUFFER, "RIGHT_ARROW"))
    // {
    //     option = VK_RIGHT;
    // }
    // else if (!strcmp(BUFFER, "UP_ARROW"))
    // {
    //     option = VK_UP;
    // }
    if (!strcmp(BUFFER, "ESCAPE"))
    {
        option = VK_ESCAPE;
    }
    else if (!strcmp(BUFFER, "SPACE"))
    {
        option = VK_SPACE;
    }
    else if (!strcmp(BUFFER, "LSHIFT"))
    {
        option = VK_LSHIFT;
    }
    else if (!strcmp(BUFFER, "RSHIFT"))
    {
        option = VK_RSHIFT;
    }
    else if (!strcmp(BUFFER, "LCONTROL"))
    {
        option = VK_LCONTROL;
    }
    else if (!strcmp(BUFFER, "RCONTROL"))
    {
        option = VK_RCONTROL;
    }
    else if (!strcmp(BUFFER, "LALT"))
    {
        option = VK_LMENU;
    }
    else if (!strcmp(BUFFER, "RALT"))
    {
        option = VK_RMENU;
    }
    else if (!strcmp(BUFFER, "MOUSE_LEFT"))
    {
        option = MOUSE_LEFT;
    }
    else if (!strcmp(BUFFER, "MOUSE_RIGHT"))
    {
        option = MOUSE_RIGHT;
    }
    else if (!strcmp(BUFFER, "SCROLL_UP_MOUSE"))
    {
        option = SCROLL_UP_MOUSE;
    }
    else if (!strcmp(BUFFER, "SCROLL_DOWN_MOUSE"))
    {
        option = SCROLL_DOWN_MOUSE;
    }
    else if (!strcmp(BUFFER, "NO_INPUT"))
    {
        option = NO_INPUT;
    }

    return option;
}

void parse_pos(char *BUFFER, nccf_config *conf)
{
    MOUSE_POS pos = NONE;
    char NAME[MAX_BUFF_LEN];
    char VAL[MAX_BUFF_LEN];
    get_option(BUFFER, NAME, VAL);
    if (strcmp(NAME, "MODE"))
    {
        printf("%s is an invalid option for MOUSE_POS\n", NAME);
        return;
    }

    if (!strcmp(VAL, "WIIMOTE"))
    {
        printf("MOUSE_POS MODE set to WIIMOTE\n");
        pos = WIIMOTE;
    }
    else if (!strcmp(VAL, "NUNCHUK"))
    {
        printf("MOUSE_POS MODE set to NUNCHUK\n");
        pos = NUNCHUK;
    }
    else if (!strcmp(VAL, "NONE"))
    {
        printf("MOUSE_POS MODE set to NONE\n");
        pos = NONE;
    }
    else
    {
        printf("MOUSE_POS MODE VALUE %s is an invalid option\n", VAL);
        pos = NONE;
    }

    conf->mouse_pos = pos;
}

void parse_wiimote(char *BUFFER, nccf_config *conf)
{
    MOUSE_POS pos = NONE;
    char NAME[MAX_BUFF_LEN];
    char VAL[MAX_BUFF_LEN];
    get_option(BUFFER, NAME, VAL);

    char *OPTIONS[] = {
        "UP_D",
        "LEFT_D",
        "DOWN_D",
        "RIGHT_D",
        "A",
        "B",
        "MINUS",
        "PLUS",
        "HOME",
        "ONE",
        "TWO",
    };

    // figure out which setting we're picking
    int *option = ((int *) conf);
    // loop for every item, the ptr should move in turn
    int i = 0;
    while (i < 12)
    {
        // this finds which field we r looking for
        if (!strcmp(NAME, OPTIONS[i++])) {
            break;
        }
    }
    // this means its an invalid field
    if (i >= 12) return;

    int setting = 0;
    if (strlen(VAL) == 1)
    {
        setting = toupper(VAL[0]);
    }
    // parse all other options
    else
    {
        setting = parse_vk_option(VAL);
    }

    option[i] = setting;
    printf("WIIMOTE OPTION %s SET TO %s\n", NAME, VAL);
}

void parse_nunchuk(char *BUFFER, nccf_config *conf)
{
    MOUSE_POS pos = NONE;
    char NAME[MAX_BUFF_LEN];
    char VAL[MAX_BUFF_LEN];
    get_option(BUFFER, NAME, VAL);

    char *OPTIONS[] = {
        "Z",
        "C",
        "UP_ANALOG",
        "LEFT_ANALOG",
        "DOWN_ANALOG",
        "RIGHT_ANALOG",
    };

    // figure out which setting we're picking
    int *option = ((int *) conf) + 11;
    // loop for every item, the ptr should move in turn
    int i = 0;
    while (i < 7)
    {
        // this finds which field we r looking for
        if (!strcmp(NAME, OPTIONS[i++])) {
            break;
        }
    }
    // this means its an invalid field
    if (i >= 7) return;

    int setting = 0;
    if (strlen(VAL) == 1)
    {
        setting = toupper(VAL[0]);
    }
    // parse all other options
    else
    {
        setting = parse_vk_option(VAL);
        if (setting == 0)
        {
            option[i] = 0;
            return;
        }
    }

    printf("NUNCHUK OPTION %s SET TO %s\n", NAME, VAL);
    option[i] = setting;
}

void parse_led(char *BUFFER, nccf_config *conf)
{
    char NAME[MAX_BUFF_LEN];
    char VAL[MAX_BUFF_LEN];
    get_option(BUFFER, NAME, VAL);
    if (!strcmp(NAME, "LED_1"))
    {
        if (!strcmp(VAL, "ON")) {
            printf("LED1 set on\n");
            conf->LED_STATE += 1 << 4;
        }
    }
    else if (!strcmp(NAME, "LED_2"))
    {
        if (!strcmp(VAL, "ON")) {
            printf("LED2 set on\n");
            conf->LED_STATE += 1 << 5;
        }
    }
    else if (!strcmp(NAME, "LED_3"))
    {
        if (!strcmp(VAL, "ON")) {
            printf("LED3 set on\n");
            conf->LED_STATE += 1 << 6;
        }
    }
    else if (!strcmp(NAME, "LED_4"))
    {
        if (!strcmp(VAL, "ON")) {
            printf("LED4 set on\n");
            conf->LED_STATE += 1 << 7;
        }
    }
    else
    {
        printf("LED SETTING NAME %s is an invalid option\n", NAME);
    }
}

nccf_config *parse_nccf(nccf_config *conf, char *config_path)
{
    char BUFFER[MAX_BUFF_LEN];
    PARSE_STATE state = NO_PARSE;
    FILE *p = fopen(config_path, "r");
    int output = 0;
    do
    {
        output = read_line(p, BUFFER);
        // skip comments
        if ((BUFFER[0] == BUFFER[1] && BUFFER[0] == '/') || strlen(BUFFER) == 0) {
            continue;
        }
        // strip line
        strip_line(BUFFER);
        // if first of a config, read new buffer
        if (BUFFER[0] != ' ')
        {
            state = parse_state(BUFFER);
            continue;
        }
        // check if in config
        if (state != NO_PARSE && BUFFER[0] == ' ')
        {
            switch (state)
            {
            case IN_POS:
                parse_pos(BUFFER, conf);
                break;
            case IN_WIIMOTE:
                parse_wiimote(BUFFER, conf);
                break;
            case IN_NUNCHUCK:
                parse_nunchuk(BUFFER, conf);
                break;
            case IN_LED:
                parse_led(BUFFER, conf);
                break;
            default:
                break;
            }
        }

        // stop program on EOF
        if (output == EOF)
            break;
    } while (output != EOF);

    fclose(p);
    return conf;
}

// void main(int argc, char *argv[])
// {
//     nccf_config act = {0};
//     parse_nccf(&act, argv[argc - 1]);

//     printf("MOUSE_POS set to %ld\n\n", act.mouse_pos);

//     printf("WIIMOTE A set to %ld\n", act.wiimote_conf.A);
//     printf("WIIMOTE B set to %ld\n", act.wiimote_conf.B);
//     printf("WIIMOTE UP_D set to %ld\n", act.wiimote_conf.UP_D);
//     printf("WIIMOTE LEFT_D set to %ld\n", act.wiimote_conf.LEFT_D);
//     printf("WIIMOTE DOWN_D set to %ld\n", act.wiimote_conf.DOWN_D);
//     printf("WIIMOTE RIGHT_D set to %ld\n", act.wiimote_conf.RIGHT_D);
//     printf("WIIMOTE MINUS set to %ld\n", act.wiimote_conf.MINUS);
//     printf("WIIMOTE PLUS set to %ld\n", act.wiimote_conf.PLUS);
//     printf("WIIMOTE HOME set to %ld\n", act.wiimote_conf.HOME);
//     printf("WIIMOTE ONE set to %ld\n", act.wiimote_conf.ONE);
//     printf("WIIMOTE TWO set to %ld\n\n", act.wiimote_conf.TWO);

//     printf("NUNCHUK UP_ANALOG set to %ld\n", act.nunchuck_conf.UP_ANALOG);
//     printf("NUNCHUK LEFT_ANALOG set to %ld\n", act.nunchuck_conf.LEFT_ANALOG);
//     printf("NUNCHUK DOWN_ANALOG set to %ld\n", act.nunchuck_conf.DOWN_ANALOG);
//     printf("NUNCHUK RIGHT_ANALOG set to %ld\n", act.nunchuck_conf.RIGHT_ANALOG);
//     printf("NUNCHUK Z set to %ld\n", act.nunchuck_conf.Z);
//     printf("NUNCHUK C set to %ld\n\n", act.nunchuck_conf.C);

//     printf("LEDS SET TO %d\n", act.LED_STATE >> 4);
// }
