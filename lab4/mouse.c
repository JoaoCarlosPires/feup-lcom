#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <mouse.h>

uint32_t p_byte;
int hook_id = 2;
uint32_t cnt2;

// Mouse interrupt handler
void(mouse_ih)() {

  uint32_t status_reg;
  bool ih_return;

  sys_inb(STATUS_REG, &status_reg); // reads the status register and puts it in 'status_reg'

  // Checks for errors on timeout/parity and checks if aux coms from the correct device (mouse)
  // If there are no errors, move the ob contents to the packet byte 'p_byte'
  if (((TMT_ERR | PAR_ERR) & status_reg) || !(status_reg & AUX)) {

    uint32_t erro;

    sys_inb(OUT_BUFF, &erro);
    ih_return = 1;
  }
  else {

    sys_inb(OUT_BUFF, &p_byte);
    ih_return = 0;
  }
}

// Mouse subscribe policy
void(mouse_subscribe)() {

  sys_irqsetpolicy(MOUSE_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &hook_id);
}

// Mouse unsubscribe policy
void(mouse_unsubscribe)() {

  sys_irqrmpolicy(&hook_id);
}

// Disables the mouse interrupt handler
void disable_mouse_ih() {

  sys_irqdisable(&hook_id);
}

// Enables the mouse interrupt handler
void enable_mouse_ih() {

  sys_irqenable(&hook_id);
}

// For mouse commands
void mouse_command_stream(uint8_t arg) {

  uint32_t byte, status;

  sys_outb(KBC_CMD_REG, KBC_WRITE_MOUSE);
  sys_inb(STATUS_REG, &status);
  sys_outb(IN_BUFF, arg);
  sys_inb(OUT_BUFF, &byte);
}

// To parse the packets
void parse(uint8_t packets[], struct packet *print_packet) {

  print_packet->bytes[0] = packets[0];

  print_packet->bytes[1] = packets[1];

  print_packet->bytes[2] = packets[2];

  print_packet->lb = (packets[0] & BIT(0));

  print_packet->rb = (packets[0] & BIT(1)) >> 1;

  print_packet->mb = (packets[0] & BIT(2)) >> 2;

  print_packet->x_ov = (packets[0] & BIT(6)) >> 6;

  print_packet->y_ov = (packets[0] & BIT(7)) >> 7;

  if (packets[0] & BIT(4)) {
    print_packet->delta_x = (packets[1] | SE_8_TO_16);
  }
  else {
    print_packet->delta_x = packets[1];
  }

  if (packets[0] & BIT(5)) {
    print_packet->delta_y = (packets[2] | SE_8_TO_16);
  }
  else {
    print_packet->delta_y = packets[2];
  }
}

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t new_var;
  sys_inb(port, &new_var);
  *value = (uint8_t) new_var;
  cnt2++;
  return 0;
  // It will return non-zero if any error occurs
}

int get_MSB(uint8_t val, uint8_t *msb) {
  *msb = (uint8_t)((val) >> 7);
  return 0;
  // It will return non-zero if any error occurs
}

int get_LSB(uint16_t val, uint8_t *lsb) {
  *lsb = (uint8_t)((val << 7) >> 7);
  return 0;
  // It will return non-zero if any error occurs
}
