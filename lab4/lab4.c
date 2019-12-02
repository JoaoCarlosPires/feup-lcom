// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <minix/sysutil.h>
#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8254.h"
#include <mouse.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

//Global Variables needed
extern bool ih_return;
extern uint32_t p_byte;
extern int hook_id;
extern int counter_timer;
extern uint32_t cnt2;

int(mouse_test_packet)(uint32_t cnt) {
  uint8_t packets[100], counter = 0;
  int ipc_status, r;
  uint8_t irq_set_mouse = BIT(hook_id);
  message msg;

  mouse_subscribe();
  
  disable_mouse_ih();
  mouse_command_stream(KBC_EN_DATA);
  enable_mouse_ih();

  while (cnt > 0) {

    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set_mouse) {

            mouse_ih();

            switch (counter) {
              case 0:
                if (p_byte & BIT(3)) {
                  packets[0] = p_byte;
                  counter++;
                }
                break;

              case 1:
                packets[1] = p_byte;
                counter++;
                break;

              case 2:
                packets[2] = p_byte;
                counter++;
                break;
            }

            if (counter == 3) {
              counter = 0;
              struct packet print_packet;
              parse(packets, &print_packet);
              mouse_print_packet(&print_packet);
              cnt--;
            }
          }

          break;

        default:
          break;
      }
    }
    else { // received a standard message, not a notification
           // no standard messages expected: do nothing
    }
  }

  disable_mouse_ih();
  mouse_command_stream(KBC_DIS_DATA);
  enable_mouse_ih();

  mouse_unsubscribe();

  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  
  uint8_t packets[100], counter = 0;

  while (cnt > 0) {
    if (counter == 0) {
      mouse_command_stream(KBC_READ_MOUSE);
    }

    mouse_ih();

    switch (counter) {
      case 0:
        if (p_byte & BIT(3)) {
          packets[0] = p_byte;
          counter++;
        }
        break;
      case 1:
        packets[1] = p_byte;
        counter++;
        break;
      case 2:
        packets[2] = p_byte;
        counter++;
        break;
    }

    if (counter == 3) {
      counter = 0;
      struct packet print_packet;
      parse(packets, &print_packet);
      mouse_print_packet(&print_packet);
      tickdelay(micros_to_ticks(period * 1000));
      cnt--;
    }
  }

  mouse_command_stream(KBC_SET_STREAM);
  mouse_command_stream(KBC_DIS_DATA);
  sys_outb(KBC_CMD_REG, KBC_WRITE_CMD);
  sys_outb(KBC_WRITE_CMD, minix_get_dflt_kbc_cmd_byte());

  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  uint8_t packets[100], counter = 0;
  int ipc_status, r;
  int irq_set_mouse = BIT(hook_id);
  message msg;
  int hook_id_timer = 1;
  uint8_t bit_no;
  int irq_set_timer = BIT(hook_id_timer);

  mouse_subscribe();
  timer_subscribe_int(&bit_no);

  disable_mouse_ih();
  mouse_command_stream(KBC_EN_DATA);
  enable_mouse_ih();

  while ((counter_timer / 60) < (idle_time)) {

    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification

      switch (_ENDPOINT_P(msg.m_source)) {

        case HARDWARE: // hardware interrupt notification

          if (msg.m_notify.interrupts & irq_set_mouse) { // mouse interrupt

            mouse_ih();

            switch (counter) {
              case 0:
                if (p_byte & BIT(3)) {
                  packets[0] = p_byte;
                  counter++;
                }
                break;

              case 1:
                packets[1] = p_byte;
                counter++;
                break;

              case 2:
                packets[2] = p_byte;
                counter++;
                break;
            }

            if (counter == 3) {

              counter = 0;
              struct packet print_packet;
              parse(packets, &print_packet);
              mouse_print_packet(&print_packet);
              counter_timer = 0;
            }
          }

          if (msg.m_notify.interrupts & irq_set_timer) { // timer interrupt
            timer_int_handler();
          }
          break;

        default:

          break;
      }
    }
    else { // received a standard message, not a notification
           // no standard messages expected: do nothing
    }
  }

  disable_mouse_ih();
  mouse_command_stream(KBC_DIS_DATA);
  enable_mouse_ih();

  mouse_unsubscribe();
  timer_unsubscribe_int();

  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  /* To be completed */
  printf("%s: under construction\n", __func__);
  return 1;
}
