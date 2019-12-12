#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdbool.h>
#include <stdint.h>

#include "kbd.h"
#include "vbe.h"
#include "screen.h"
#include "timer.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("PT-PT");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  //lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  //lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

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

int hook_id_kbd;
int irq_set_kbd;
extern int hook_id_timer;
uint8_t bit_no;
int irq_set_timer;

int(proj_main_loop)(int argc, char *argv[]) {

  /* Initialization of video mode and keyboard */

  video_init(0x115);

  hook_id_kbd = 2;
  irq_set_kbd = BIT(hook_id_kbd);

  irq_set_timer = BIT(hook_id_timer);
  
  sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &hook_id_kbd);
  timer_subscribe_int(&bit_no);

  /* Start of the game */

  int menu_select = home_screen();

  if (menu_select == 1) { // if the key E (exit) is pressed
    game_screen();
  }
  else {
    vg_exit(); // Returns to text mode
    return 0;
  }

  // cancels the subscription of the KBC interrupt before terminating
  sys_irqrmpolicy(&hook_id_kbd);
  timer_unsubscribe_int();
  
  // Returns to text mode
  vg_exit();

  return 0;
}
