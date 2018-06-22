#ifndef __SPACESHIP_H
#define __SPACESHIP_H

#include "Sprite.h"
#include "Timer.h"
#include <stdbool.h>
#include "Mouse.h"
#include "MouseMacros.h"

#define PI2  6.2831853
#define PI_2 1.5707963
#define n_rot 90

/** @defgroup
 * @{
 *  SpaceShip
 */

/** @name SpaceShip */
/**@{
 *
 * SpaceShip struct
 */
typedef struct{

	int done; 		/**< @brief Set when spaceship is destroyed */
	int angleIndex; 		/**< @brief SpaceShip angle index */
	Sprite* image;	 		/**< @brief SpaceShip sprite */
	bool initialized; 		/**< @brief Set when spaceship has been initializes */

	unsigned short ** SpaceShipRot; 		/**< @brief Spaceship rotations */
	Sprite * laser; 		/**< @brief Laser sprite */
	Sprite ** shoots; 		/**< @brief Bullets sprites */
	int shootsSize; 		/**< @brief Number of bullets*/

}SpaceShip;
/** @} end of SpaceShip struct*/


/**
 * @brief Creates a new spaceship
 *
 * Loads a set of rotated spaceships sprites to provide rotation on the game
 * Loads laser sprite
 * Initializes member variables
 *
 * @return Returns spaceship object
 */
SpaceShip* createSpaceShip();

/**
 * @brief Update specified spaceship
 *
 * Handles spaceship accelaration, velocity and position
 * If left button clicked it generates a new bullet
 *
 * @param spaceship Spaceship to update
 * @return
 */
void updateSpaceShip(SpaceShip* spaceship);

/**
 * @brief Draws specified spaceship
 *
 * Calculates index of rotation to select the correct spaceship sprite
 * Draws spaceship
 *
 * @param spaceship Spaceship to draw
 * @return
 */
void drawSpaceShip(SpaceShip* spaceship);

/**
 * @brief Erase spaceship object
 *
 * Free bullets
 * Free spaceship rotations
 * Free spaceship
 *
 * @param spaceship Spaceship to erase
 * @return
 */
void eraseSpaceShip(SpaceShip* spaceship);

/**
 * @brief Reset specified spaceship
 *
 * Resets spaceship member variables
 * Free bullets
 *
 * @param spaceship Spaceship to reset
 * @return
 */
void resetSpaceShip(SpaceShip* spaceship);

/**
 * @brief Initial game sequence
 *
 * Animation for the beggining of the game
 *
 * @param spaceship Spaceship to process
 * @return
 */
void initSequence(SpaceShip* spaceship);

/**
 * @brief Lost game sequence
 *
 * Animation for the end of the game
 *
 * @param spaceship Spaceship to process
 * @return Returns 0 on sucess, 1 on error
 */
int lostSequence(SpaceShip* spaceship);

/**
 * @brief Updates spaceship position
 *
 * @param spaceship Spaceship to update
 * @return 
 */
void updateSpaceShipPos(SpaceShip* spaceship);

/**
 * @brief Convert angle to index
 *
 * Convert angle to the respective index for SpaceShipRot array
 *
 * @param angle Angle
 * @return Index
 */
inline int AngleToIndex(float angle);

/**
 * @brief Convert index to angle
 *
 * Conver index of SpaceShipRot array to the respective angle
 *
 * @param angle Index
 * @return Angle
 */
inline float IndexToAngle(int index);

/** @} end of SpaceShip*/

#endif
