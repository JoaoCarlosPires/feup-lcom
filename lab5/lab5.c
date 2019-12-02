// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include <minix/driver.h>
#include <sys/mman.h>

// Any header files included below this line should have been created by you
#include "kbd.h"
#include "vbe.h"

uint8_t scancode;
uint16_t hres;
uint16_t vres;
uint16_t bits_per_pixel;
uint32_t RedMaskSize;
uint32_t RedFieldPosition;
uint32_t GreenMaskSize;
uint32_t GreenFieldPosition;
uint32_t BlueMaskSize;
uint32_t BlueFieldPosition;
uint32_t MemoryModel;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {

  video_init(mode);

  sleep(delay);

  vg_exit();

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

  video_init(mode);

  int ipc_status, hook_id = 2, r;

  message msg;
  int irq_set = BIT(hook_id);

  sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &hook_id);

  (vg_draw_rectangle)(x, y, height, width, color);

  while (scancode != 0x81) {

    // If the ESC key is released, which presupposes it 
    // was previously pressed, the while loop ends

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

  // Returns to text mode
  vg_exit();

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  video_init(mode);

  int ipc_status, hook_id = 2, r;

  message msg;
  int irq_set = BIT(hook_id);

  sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &hook_id);

  uint32_t color;

  for (uint16_t i = 0; i < no_rectangles; i++) {
    for (uint16_t j = 0; j < no_rectangles; j++) {
      if (MemoryModel == 0x06) {
        uint32_t R_first = first >> RedFieldPosition & (BIT(RedMaskSize) - 1);
        uint32_t G_first = first >> GreenFieldPosition & (BIT(GreenMaskSize) - 1);
        uint32_t B_first = first >> BlueFieldPosition & (BIT(BlueMaskSize) - 1);

        uint32_t R = (R_first + j * step) % (1 << RedMaskSize);
        uint32_t G = (G_first + i * step) % (1 << GreenMaskSize);
        uint32_t B = (B_first + (i + j) * step) % (1 << BlueMaskSize);

        color = R << RedFieldPosition | G << GreenFieldPosition | B << BlueFieldPosition;
      }
      else {
        color = (first + (i * no_rectangles + j) * step) % (1 << bits_per_pixel);
      }

      (vg_draw_rectangle)(j * (hres / no_rectangles), i * (vres / no_rectangles), vres / no_rectangles, hres / no_rectangles, color);
    }
  }
  while (scancode != 0x81) {

    // If the ESC key is released, which presupposes it 
    // was previously pressed, the while loop ends

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

  vg_exit(); // Returns to text mode

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {

  uint16_t mode = 0x105;
  video_init(mode);

  enum xpm_image_type type = XPM_INDEXED;
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

  int ipc_status, hook_id = 2, r;
  int irq_set = BIT(hook_id);
  message msg;

  sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &hook_id);

  while (scancode != 0x81) {

    // If the ESC key is released, which presupposes it was previously
    // pressed, the while loop ends

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
  
  sys_irqrmpolicy(&hook_id); // cancels the subscription of the KBC interrupt before terminating

  vg_exit(); // Returns to text mode

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
