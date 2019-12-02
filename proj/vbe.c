#include <lcom/lcf.h>
#include <math.h>

#include <vbe.h>

static char *video_mem;
extern uint16_t hres;
extern uint16_t vres;
extern uint16_t bits_per_pixel;
extern uint32_t RedMaskSize;
extern uint32_t RedFieldPosition;
extern uint32_t GreenMaskSize;
extern uint32_t GreenFieldPosition;
extern uint32_t BlueMaskSize;
extern uint32_t BlueFieldPosition;
extern uint32_t MemoryModel;

int video_init(uint16_t mode) {

  size_t size = 1024 * 1024;
  mmap_t map;
  map.phys = 0x00;
  map.size = size;

  struct minix_mem_range mr;
  int r;

  lm_alloc(size, &map);

  vbe_mode_info_t vmi_p;

  vbe_get_mode_info(mode, &vmi_p);

  hres = vmi_p.XResolution;
  vres = vmi_p.YResolution;
  bits_per_pixel = vmi_p.BitsPerPixel;
  RedMaskSize = vmi_p.RedMaskSize;
  RedFieldPosition = vmi_p.RedFieldPosition;
  GreenMaskSize = vmi_p.GreenMaskSize;
  GreenFieldPosition = vmi_p.GreenFieldPosition;
  BlueMaskSize = vmi_p.BlueMaskSize;
  BlueFieldPosition = vmi_p.BlueFieldPosition;
  MemoryModel = vmi_p.MemoryModel;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vmi_p.PhysBasePtr;
  mr.mr_limit = mr.mr_base + (vres) * (hres) * ((vmi_p.BitsPerPixel + 7) / 8);

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, (vres) * (hres) * ((vmi_p.BitsPerPixel + 7) / 8));

  if (video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  reg86_t interrupt_vector;

  memset(&interrupt_vector, 0, sizeof(interrupt_vector));

  interrupt_vector.intno = 0x10;
  interrupt_vector.ax = 0x4F02;
  interrupt_vector.bx = 1 << 14 | mode;

  sys_int86(&interrupt_vector);

  if (interrupt_vector.ah == 0x01 || interrupt_vector.ah == 0x02 || interrupt_vector.ah == 0x03) {
    return 1;
  }

  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  char *v_ptr = video_mem;
  v_ptr += (y * hres + x) * ((bits_per_pixel + 7) / 8);

  for (int i = 0; i < len; i++) {
    vg_draw_pixel(x + i, y, &color);
  }
  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint32_t color) {
  for (int y1 = y; y1 < height + y; y1++) {

    (vg_draw_hline)(x, y1, width, color);
  }

  return 0;
}

int vg_draw_pixel(uint16_t j, uint16_t i, uint32_t *color) {
  uint32_t aux = *color;
  uint16_t bytesUsed = (bits_per_pixel + 7) / 8;
  char *pixel = video_mem + (i * hres + j) * bytesUsed;
  for (int k = 0; k < ((bits_per_pixel + 7) / 8); k++) {
    pixel[k] = (char) aux;
    aux >>= 8;
  }
  return 0;
}
