#include <lcom/lcf.h>
#include <vbe.h>
#include <screen.h>

// All the following header files are xpm
#include "screens/Game_Over.h"
#include "screens/Pos1.h"
#include "screens/Pos2.h"
#include "screens/Pos3.h"
#include "screens/Pos4.h"
#include "screens/Pos5.h"
#include "screens/PosF.h"
#include "screens/PosI.h"
#include "screens/PosM.h"
#include "screens/level_1.h"
#include "screens/level_2.h"
#include "screens/level_3.h"
#include "screens/level_4.h"
#include "screens/level_5.h"
#include "screens/score_000.h"
#include "screens/score_050.h"
#include "screens/score_100.h"
#include "screens/score_150.h"
#include "screens/score_200.h"
#include "screens/score_250.h"
#include "screens/screen_gameScreen.h"
#include "screens/screen_mainMenu.h"
#include "screens/time_0.h"
#include "screens/time_1.h"
#include "screens/time_10.h"
#include "screens/time_2.h"
#include "screens/time_3.h"
#include "screens/time_4.h"
#include "screens/time_5.h"
#include "screens/time_6.h"
#include "screens/time_7.h"
#include "screens/time_8.h"
#include "screens/time_9.h"

char *video_mem;
uint8_t *v_ptr;

uint8_t scancode;

uint16_t hres;
uint16_t vres;

uint16_t bits_per_pixel;

extern int hook_id_kbd;
extern int irq_set_kbd;
extern int irq_set_timer;

int draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y) {
  enum xpm_image_type type = XPM_8_8_8;
  xpm_image_t img;
  uint8_t *sprite = xpm_load(xpm, type, &img);
  uint16_t width = img.width, height = img.height;

  for (uint16_t i = 0; i < height; i++) {
    for (uint16_t j = 0; j < width; j++) {
      uint32_t *color;
      color = (uint32_t *) (sprite + ((i * width + j) * ((bits_per_pixel + 7) / 8)));
      vg_draw_pixel(x + j, y + i, color);
    }
  }

  return 0;
}

void draw_level(int l) {
  switch (l) {
    case 1:
      draw_xpm(level_1, 97, 39);
      break;
    case 2:
      draw_xpm(level_2, 97, 39);
      break;
    case 3:
      draw_xpm(level_3, 97, 39);
      break;
    case 4:
      draw_xpm(level_4, 97, 39);
      break;
    case 5:
      draw_xpm(level_5, 97, 39);
      break;
    default:
      break;
  }
}

void draw_score(int s) {
  switch (s) {
    case 0:
      draw_xpm(score_000, 660, 39);
      break;
    case 1:
      draw_xpm(score_050, 660, 39);
      break;
    case 2:
      draw_xpm(score_100, 660, 39);
      break;
    case 3:
      draw_xpm(score_150, 660, 39);
      break;
    case 4:
      draw_xpm(score_200, 660, 39);
      break;
    case 5:
      draw_xpm(score_250, 660, 39);
      break;
    default:
      break;
  }
}

void draw_time(int t) {
  switch (t) {
    case 0:
      draw_xpm(time_0, 350, 39);
      break;
    case 1:
      draw_xpm(time_1, 350, 39);
      break;
    case 2:
      draw_xpm(time_2, 350, 39);
      break;
    case 3:
      draw_xpm(time_3, 350, 39);
      break;
    case 4:
      draw_xpm(time_4, 350, 39);
      break;
    case 5:
      draw_xpm(time_5, 350, 39);
      break;
    case 6:
      draw_xpm(time_6, 350, 39);
      break;
    case 7:
      draw_xpm(time_7, 350, 39);
      break;
    case 8:
      draw_xpm(time_8, 350, 39);
      break;
    case 9:
      draw_xpm(time_9, 350, 39);
      break;
    case 10:
      draw_xpm(time_10, 350, 39);
      break;
    default:
    break;
  }
}

int home_screen() {

  int ipc_status;
  int r;
  message msg;

  // Draws home screen;
  draw_xpm(screen_mainMenu, 0, 0);

  while (scancode != 0x12 && scancode != 0x19) {

    // If the E key or the P key is pressed, the while loop ends

    // Get a request message.
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set_kbd) { // subscribed interrupt
            kbc_ih();
          }
          break;
        default: break; // no other notifications expected: do nothing
      }
    }
    else { // received a standard message, not a notification
           // no standard messages expected: do nothing
    }
  }

  if (scancode == 0x19) { // scancode P (play)
    return 1;
  }

  if (scancode == 0x12) { // scancode E (exit)
    return 0;
  }
  return 0;
}

int gameover_screen() {
  draw_xpm(Game_Over, 100, 260);
  tickdelay(300);
  if (home_screen() == 1) { // if the key E (exit) is pressed
    game_screen();
  }
  return 0;
}

void change_cannon(int c) {
  switch (c) {
    case 1:
      draw_xpm(Pos1, 0, 559);
      break;
    case 2:
      draw_xpm(Pos2, 0, 559);
      break;
    case 3:
      draw_xpm(Pos3, 0, 559);
      break;
    case 4:
      draw_xpm(Pos4, 0, 559);
      break;
    case 5:
      draw_xpm(Pos5, 0, 559);
      break;
    default:
      break;
  }
}

int game_screen() {

  int ipc_status;
  int r;
  message msg;

  extern int counter;


  int t = 10;
  int c = 1;
  int l = 1;
  int s = 0;

  

  // Draws game screen
  draw_xpm(screen_gameScreen, 0, 0);
  draw_time(t);
  change_cannon(c);
  draw_level(l);
  draw_score(s);

  while (scancode != 0x12 && t != -1) {

    // If the E key is pressed, the while loop ends

    // Get a request message.
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set_kbd) { // subscribed interrupt
            kbc_ih();
            if (scancode == 0x1f && c != 5) { // if the w key is pressed
              c++;
            }
            else if (scancode == 0x11 && c != 1) { // if the s key is pressed
              c--;
            }
            change_cannon(c);
          }
          if (msg.m_notify.interrupts & irq_set_timer) { // subscribed interrupt
            timer_int_handler();
            if ((counter % 60) == 0) {
              draw_time(t);
              t--;
            }
          }
          break;
        default: break; // no other notifications expected: do nothing
      }
    }
    else { // received a standard message, not a notification
           // no standard messages expected: do nothing
    }
  }

  if (scancode != 0x12) { // condition to print the gameover only if it was due to time
    gameover_screen();
  }



  return 0;
}
