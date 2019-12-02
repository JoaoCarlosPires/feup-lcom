#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int counter = 0;

int hook_id = 1;

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /*
  If the frequency is less or equal than 18Hz, the lowest possible is 19Hz, 
  or highest than the frequency of the timer, it should return 1, as error.

  Otherwise, it should return 0 as it is a valid frequency value. 
  */

  if (freq <= 18 || freq > TIMER_FREQ) {

    return 1;
  }

  uint8_t current_status;
  timer_get_conf(timer, &current_status);

  uint8_t current_status_mode;
  current_status_mode = current_status & TIMER_COUNT_MODE_MASK;
  uint8_t current_status_bcd;
  current_status_bcd = current_status & TIMER_BCD;
  uint8_t current_status_control_word;
  current_status_control_word = TIMER_LSB_MSB | current_status_mode | current_status_bcd;

  switch (timer) {
    case 0:
      current_status_control_word = current_status_control_word | TIMER_SEL0;
      break;

    case 1:
      current_status_control_word = current_status_control_word | TIMER_SEL1;
      break;

    case 2:
      current_status_control_word = current_status_control_word | TIMER_SEL2;
      break;
  }

  uint16_t divisor;
  divisor = TIMER_FREQ / freq;

  uint8_t left_bits;
  uint8_t right_bits;

  util_get_LSB(divisor, &left_bits);
  util_get_MSB(divisor, &right_bits);

  sys_outb(TIMER_CTRL, current_status_control_word);
  sys_outb(TIMER_0 + timer, left_bits);
  sys_outb(TIMER_0 + timer, right_bits);

  return 0;
}

int(timer_subscribe_int)(uint8_t *bit_no) {
  sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);
  return 0;
}

int(timer_unsubscribe_int)() {
  sys_irqrmpolicy(&hook_id);
  return 0;
}

void(timer_int_handler)() {
  counter++;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint32_t rb; // This variable will work as a Read-Back Command

  switch (timer) { // The switch will allow to select the timer (parameter)

    case 0: // TIMER_0
      rb = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(0));
      // Binary-or operation between Read-Back Command bits (6 & 7), Status bit (4)
      // and bit of the respective timer (1)
      sys_outb(TIMER_CTRL, rb);
      util_sys_inb(TIMER_0, st);
      // Use of the function implemented in utilis.c to
      // write in the memory adress st the timer configuration
      break;

    case 1: // TIMER_1
      rb = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(1));
      // Binary-or operation between Read-Back Command bits (6 & 7), Status bit (4)
      // and bit of the respective timer (2)
      sys_outb(TIMER_CTRL, rb);
      util_sys_inb(TIMER_1, st);
      // Use of the function implemented in utilis.c to
      // write in the memory adress st the timer configuration
      break;

    case 2: // TIMER_2
      rb = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(2));
      // Binary-or operation between Read-Back Command bits (6 & 7), Status bit (4)
      // and bit of the respective timer (3)
      sys_outb(TIMER_CTRL, rb);
      util_sys_inb(TIMER_2, st);
      // Use of the function implemented in utilis.c to
      // write in the memory adress st the timer configuration
      break;
  }

  return 0;
  // It will return non-zero if any error occurs (e.g., the parmeter timer is not 0, 1 or 2)
}

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  //Checks the Control Word and will print what is specified by field

  union timer_status_field_val imprimir;

  switch (field) {
    case tsf_all:

      //All bits of 'st' to 'imprimir'

      imprimir.byte = st;

      break;

    case tsf_initial:

      //Inicialization bits of 'st to 'imprimir'

      imprimir.in_mode = (st & TIMER_LSB_MSB) >> 4;

      break;

    case tsf_mode:

      //Counting bits of 'st' to 'imprimir'

      imprimir.count_mode = (st & TIMER_COUNT_MODE_MASK) >> 1;

      break;

    case tsf_base:

      //BCD bits of 'st' to 'imprimir'

      imprimir.bcd = st & TIMER_BCD;

      break;
  }

  //Calls the auxiliary function

  timer_print_config(timer, field, imprimir);

  return 0;
}
