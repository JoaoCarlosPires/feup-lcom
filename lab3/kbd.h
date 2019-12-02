#include <stdbool.h>
#include <stdint.h>

#define OUT_BUF 0x60

#define IN_BUF 0x60

#define STAT_REG 0x64

#define KBC_CMD_REG 0x64

#define KBD_IRQ 1

#define IBF BIT(1)

#define OBF BIT(0)

#define PAR_ERR BIT(7)

#define TO_ERR BIT(6)

#define DELAY_US 20000

void(kbc_ih)();

int(util_sys_inb)(int port, uint8_t *value);

int get_MSB(uint8_t val, uint8_t *msb);

int get_LSB(uint16_t val, uint8_t *lsb);
