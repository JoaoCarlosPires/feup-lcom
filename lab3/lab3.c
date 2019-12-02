#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>
#include <minix/sysutil.h>

#include <kbd.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

extern uint8_t scancode;
extern uint32_t cnt;
extern bool error;
extern int counter;

int(kbd_test_scan)() {

  int ipc_status, hook_id = 2, r;
  bool make, dup = false;
  uint8_t size, bytes[2];

  message msg;
  int irq_set = BIT(hook_id);

  sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &hook_id);

  while (scancode != 0x81) {

    // If the ESC key is released, which presupposes it was previously
    // pressed, the while loop ends as well as the kbd_test_scan function

    // Get a request message
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { // subscribed interrupt
            kbc_ih();

            if (dup) { // makecode && breakcode 2nd byte
              bytes[1] = scancode;
              dup = false;
              size = 2;
              uint8_t msb;
              get_MSB(scancode, &msb);
              make = (msb != 1);
              kbd_print_scancode(make, size, bytes);
              size = 1;
            }
            else if (scancode == 0xE0) { // makecode && breakcode 1st byte
              bytes[0] = scancode;
              dup = true;
            }
            else { // makecode & breakcode with just 1 byte
              bytes[0] = scancode;
              size = 1;
              uint8_t msb;
              get_MSB(scancode, &msb);
              make = (msb != 1);
              kbd_print_scancode(make, size, bytes);
            }
          }
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    }
    else { // received a standard message, not a notification
           // no standard messages expected: do nothing
    }
  }

  sys_irqrmpolicy(&hook_id); // cancels the subscription of the KBC interrupt before terminating

  kbd_print_no_sysinb(cnt); // prints the number of sys_inb kernel calls

  cnt = 0;

  return 0;
}

int(kbd_test_poll)() {

  uint8_t size, bytes[2];
  uint8_t stat, scan;
  bool make;
  bool dup = false;

  while (scancode != 0x81) {

    // Checks OB bit in the status (already on kbd.c)
    util_sys_inb(STAT_REG, &stat);
    if (stat & OBF) {               // loop while 8042 output buffer is empty //
      util_sys_inb(OUT_BUF, &scan); // assuming it returns OK //
      scancode = scan;
      if ((stat & (PAR_ERR | TO_ERR)) == 0) {
        error = false;
        if (dup) { // makecode && breakcode 2º byte
          bytes[1] = scancode;
          dup = false;
          size = 2;
          uint8_t msb;
          get_MSB(scancode, &msb);
          make = (msb != 1);
          kbd_print_scancode(make, size, bytes);
          size = 1;
        }
        else if (scancode == 0xE0) { // makecode && breakcode 1º byte
          bytes[0] = scancode;
          dup = true;
        }
        else { // makecode & breakcode with 1 byte
          bytes[0] = scancode;
          size = 1;
          uint8_t msb;
          get_MSB(scancode, &msb);
          make = (msb != 1);
          kbd_print_scancode(make, size, bytes);
        }
      }
      else {
        error = true;
      }
    }

    tickdelay(micros_to_ticks(DELAY_US));
  }

  // Prints the number of sysinb calls
  kbd_print_no_sysinb(cnt);

  // Enables the KBD interruption subscription
  uint8_t lsb = 0x01, aux;
  sys_outb(KBC_CMD_REG, 0x20);
  util_sys_inb(OUT_BUF, &aux);
  aux |= lsb;
  sys_outb(KBC_CMD_REG, 0x60);
  sys_outb(IN_BUF, aux);

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {

  int ipc_status, hook_id_kbd = 2, hook_id_timer = 1;
  int r;
  bool make, dup = false;
  uint8_t size, bytes[2], bit_no;

  message msg;
  int irq_set_kbd = BIT(hook_id_kbd);
  int irq_set_timer = BIT(hook_id_timer);

  sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &hook_id_kbd); // Subscribe kbd interrupts
  timer_subscribe_int(&bit_no); // Subscribe timer interrupts

  while ((scancode != 0x81) && (counter < n * 60)) {

    // If the ESC key is released, which presupposes it was previously
    // pressed, the while loop ends as well as the kbd_test_scan function

    // Get a request message
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set_kbd) { // subscribed interrupt
            kbc_ih();
            counter = 0;
            if (dup) { // makecode && breakcode 2º byte
              bytes[1] = scancode;
              dup = false;
              size = 2;
              uint8_t msb;
              get_MSB(scancode, &msb);
              make = (msb != 1);
              kbd_print_scancode(make, size, bytes);
              size = 1;
            }
            else if (scancode == 0xE0) { // makecode && breakcode 1º byte
              bytes[0] = scancode;
              dup = true;
            }
            else { // makecode & breakcode with 1 byte
              bytes[0] = scancode;
              size = 1;
              uint8_t msb;
              get_MSB(scancode, &msb);
              make = (msb != 1);
              kbd_print_scancode(make, size, bytes);
            }
          }
          if (msg.m_notify.interrupts & irq_set_timer) { // subscribed interrupt
            timer_int_handler();
          }
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    }
    else { // received a standard message, not a notification
           // no standard messages expected: do nothing
    }
  }

  sys_irqrmpolicy(&hook_id_kbd); // cancels the subscription of the kbc interrupt before terminating
  timer_unsubscribe_int(); // cancels the subscription of the timer interrupt before terminating

  kbd_print_no_sysinb(cnt); // prints the number of sys_inb kernel calls

  return 0;
}
