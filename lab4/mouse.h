#include <stdbool.h>
#include <stdint.h>

#define STATUS_REG 0x64

#define MOUSE_IRQ 12

#define OUT_BUFF 0x60

#define IN_BUFF 0x60

#define PAR_ERR BIT(7)

#define TMT_ERR BIT(6)

#define AUX BIT(5)

#define INH BIT(4)

#define A2 BIT(3)

#define SYS BIT(2)

#define IBF BIT(1)

#define OBF BIT(0)

#define DELAY_US 20000

#define KBC_CMD_REG 0x64

#define KBC_READ_CMD 0x20

#define KBC_WRITE_CMD 0x60

#define KBC_WRITE_MOUSE 0xD4

#define KBC_EN_DATA 0xF4

#define KBC_DIS_DATA 0xF5

#define KBC_SET_STREAM 0xEA

#define KBC_READ_MOUSE 0xEB

#define KBC_SET_REMOTE 0xF0

#define SE_8_TO_16 0xFF00

void mouse_command_stream(uint8_t arg);

void(mouse_ih)();

void(mouse_subscribe)();

void(mouse_unsubscribe)();

void disable_mouse_ih();

void enable_mouse_ih();

void parse(uint8_t packets[], struct packet *print_packet);

int(util_sys_inb)(int port, uint8_t *value);

int get_MSB(uint8_t val, uint8_t *msb);

int get_LSB(uint16_t val, uint8_t *lsb);

/*
struct packet {
  uint8_t bytes[3];
  bool rb;
  bool mb;
  bool lb;
  int16_t delta_x;
  int16_t delta_y;
  bool x_obv;
  bool y_obv;
};*/
