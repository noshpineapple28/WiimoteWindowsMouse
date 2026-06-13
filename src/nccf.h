#ifndef __NCCF_H
#define __NCCF_H

#define MAX_BUFF_LEN 200

// vals for mouse events
#define NO_INPUT 0
#define MOUSE_LEFT 0x6969
#define MOUSE_RIGHT 0x6970
#define SCROLL_UP_MOUSE 0x6971
#define SCROLL_DOWN_MOUSE 0x6972

typedef enum MOUSE_POS
{
    WIIMOTE = (int)0,
    NUNCHUK,
    NONE
} MOUSE_POS;

typedef enum PARSE_STATE
{
    IN_POS,
    IN_WIIMOTE,
    IN_NUNCHUCK,
    IN_LED,
    NO_PARSE
} PARSE_STATE;

typedef struct WIIMOTE_CONFIG
{
    int UP_D;
    int LEFT_D;
    int DOWN_D;
    int RIGHT_D;
    int A;
    int B;
    int MINUS;
    int PLUS;
    int HOME;
    int ONE;
    int TWO;
} WIIMOTE_CONFIG;

typedef struct NUNCHUK_CONFIG
{
    int Z;
    int C;
    int UP_ANALOG;
    int LEFT_ANALOG;
    int DOWN_ANALOG;
    int RIGHT_ANALOG;
    int SPEED;
} NUNCHUK_CONFIG;

typedef struct nccf_config
{
    MOUSE_POS mouse_pos;
    WIIMOTE_CONFIG wiimote_conf;
    NUNCHUK_CONFIG nunchuck_conf;
    int LED_STATE;
} nccf_config;

nccf_config *parse_nccf(nccf_config *conf, char *config_path);

#endif
