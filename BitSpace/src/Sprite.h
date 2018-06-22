#ifndef __SPRITE_H
#define __SPRITE_H


/** @defgroup
 * @{
 *  Sprite
 */


/** @name Sprite */
/**@{Sprite struct
 */
typedef struct {
	int x, y; 			/**< @brief Sprite position */
	int width, height; 	/**< @brief Image dimensions */
	int xspeed, yspeed; /**< @brief Sprite speed */
	int xace, yace;		/**< @brief Sprite aceleration */
	char *map; 			/**< @brief Pixmap */
} Sprite;
/** @} end of Sprite struct*/


/**
 * @brief Create sprite from Bitmap
 *
 * Construct a sprite from a given bitmap
 * @param filename Path of bitmap image
 * @param x Initial position in x axis
 * @param y Initial position in x axis
 * @param xspeed Initial velocity in x axis
 * @param yspeed Initial velocity in y axis
 * @return Pointer to sprite constructed
 */
Sprite *create_sprite_Bitmap(const char *filename, int x, int y, int xspeed, int yspeed);

/**
 * @brief Copy Sprite
 *
 * Allocate memory for a new Sprite and copy the content of spOri
 * @param spOri Poiter to sprite that will be copied
 * @return Pointer to sprite constructed
 */
Sprite *copySprite(Sprite *spOri);

/**
 * @brief Sprite destructor
 *
 * Free memory user by a sprite object
 * @param sp Sprite that will be destroyed
 * @return
 */
void destroy_sprite(Sprite *sp);

/**
 * @brief Draw sprite
 *
 * Copy pixmap to second buffer
 * @param sp Sprite that will be drawn
 * @return Return 0 if successful or 1 otherwise
 */
int draw_sprite(Sprite *sp);


/**
 * @brief Create a sprite rotated
 *
 * Creates a new Sprite with the image of sp rotated
 *
 * @param sp Sprite that contains image that will be rotated (sp image maintains equal)
 * @param angle Angle of rotation (Counter-clock wise starts at PI/2)
 * @param picBack Pixmap rotated
 * @return Return 0 if successful or 1 otherwise
 */
int create_rotate_sprite(Sprite *sp, float angle, unsigned short * picBack);

/**
 * @brief Correct sprite margins
 *
 * Correct sprite x and y value according to screen resolution
 *
 * @param image Sprite whose coordinates are to be corrected
 * @return
 */
void correctMargins(Sprite *image);

/**
 * @brief Collision between object
 *
 * Verify if occurs a collision between obj1 and obj2
 *
 * @param obj1 Sprite 1
 * @param obj2 Sprite 2
 * @return Return 1 if there is a collision or 0 otherwise
 */
int collision(Sprite *obj1, Sprite *obj2);

/**
 * @brief Resets asteroid for multiplayer
 *
 * @param sp Asteroid
 * @param obj2 spaceship Spaceship to test collision
 * @return
 */
void resetAsteroid2(Sprite *sp, Sprite * spaceship, int resetSpeed);

/**
 * @brief Updates asteroid in relation to spaceship
 *
 * @param sp Asteroid
 * @param spaceship Spaceship to test use
 * @return
 */
void Asteroid_update(Sprite * sp,Sprite * spaceship);

/**
 * @brief Resets asteroid
 *
 * @param sp Asteroid
 * @param obj2 spaceship Spaceship to test collision
 * @return
 */
void resetAsteroid(Sprite *sp, Sprite * spaceship, int resetSpeed);
/** @} end of Sprite*/

#endif
