#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initialization of VBE with the parameter mode
 *
 * @param mode mode which VBE will be initialized with
 * @return Return 0 upon success and non-zero otherwise
 */
int video_init(uint16_t mode);

/**
 * @brief Draws in the position (x,y) the line of lenght len and color
 *
 * @param x coordinate x where the line will be draw
 * @param y coordinate y where the line will be draw
 * @param len lenght of the line
 * @param color color of the line
 * @return Return 0 upon success and non-zero otherwise
 */
int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 * @brief Draws in the position (x,y) the rectangle (height x width) filled with color
 *
 * @param x coordinate x where the rectangle will be draw
 * @param y coordinate y where the rectangle will be draw
 * @param height height of the rectangle
 * @param width width of the rectangle
 * @param color color of the rectangle
 * @return Return 0 upon success and non-zero otherwise
 */
int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint32_t color);

/**
 * @brief Draws in the position (j,i) the pixel with color
 *
 * @param j coordinate x of the pixel
 * @param i coordinate y of the pixel
 * @param color color of the pixel
 * @return Return 0 upon success and non-zero otherwise
 */
int vg_draw_pixel(uint16_t j, uint16_t i, uint32_t *color);
