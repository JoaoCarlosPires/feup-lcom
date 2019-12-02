#include <lcom/lcf.h>

#include "kbd.h"
#include "vbe.h"

char *video_mem;
uint8_t *v_ptr;

uint8_t scancode;

uint16_t hres;
uint16_t vres;

uint16_t bits_per_pixel;

int draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y) {
  enum xpm_image_type type = XPM_8_8_8;
  xpm_image_t img;
  uint8_t *sprite = xpm_load(xpm, type, &img);
  uint16_t width = img.width, height = img.height;

  for (uint16_t i = 0; i < height; i++) {
    for (uint16_t j = 0; j < width; j++) {
      uint32_t *color;
      color = (uint32_t *) sprite + ((i * width + j) * ((bits_per_pixel + 7) / 8));
      vg_draw_pixel(x + j, y + i, color);
    }
  }

  return 0;
}

int home_screen() {
  int ipc_status;
  int hook_id = 2;
  int irq_set = BIT(hook_id);
  int r;
  message msg;

  sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &hook_id);

  // Draws home screen;

  while (scancode != 0x1f && scancode != 0x12) {

    // If the ESC key is released, the while loop ends

    // Get a request message.
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { // subscribed interrupt
            kbc_ih();
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

  // cancels the subscription of the KBC interrupt before terminating
  sys_irqrmpolicy(&hook_id);

  if (scancode == 0x1f) { // scancode S (start)
    return 1;
  }

  if (scancode == 0x12) { // scancode E (exit)
    return 0;
  }
  return 0;
}

int game_screen() {

  int ipc_status;
  int hook_id = 2;
  int irq_set = BIT(hook_id);
  int r;
  message msg;

  sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &hook_id);
  
  // Draws game screen

  while (scancode != 0x12) {

    // If the ESC key is released, the while loop ends

    // Get a request message.
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { // subscribed interrupt
            kbc_ih();
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

  // cancels the subscription of the KBC interrupt before terminating
  sys_irqrmpolicy(&hook_id);

  return 0;
}

int gameover_screen() {
  return 0;
}
