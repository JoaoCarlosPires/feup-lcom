#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Draws in the position (x,y) the xpm passed as argument
 *
 * @param xpm xpm_map_t variable
 * @param x coordinate x where the xpm will be draw
 * @param y coordinate y where the xpm will be draw
 * @return Return 0 upon success and non-zero otherwise
 */
int draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);

/**
 * @brief Draws the home screen
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int home_screen();

/**
 * @brief Draws the game screen
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int game_screen();

/**
 * @brief Draws the game over screen
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int gameover_screen();
