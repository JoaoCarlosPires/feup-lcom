#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = (uint8_t)((val << 8) >> 8);
  return 0;
  // It will return non-zero if any error occurs
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = (uint8_t)(val >> 8);
  return 0;
  // It will return non-zero if any error occurs
}

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t new_var;
  sys_inb(port, &new_var);
  *value = (uint8_t) new_var;
  return 0;
  // It will return non-zero if any error occurs
}
