#ifndef SCORE_H
#define SCORE_H

#include "Sprite.h"

/** @defgroup Score Score
 * @{
 *  Score
 */

/** @name Score */
/**@{Score struct
 */

typedef struct {
	int actual_score;	/**< @brief Score of the the current game*/
	Sprite ** numeral;	/**< @brief Number sprites*/
}Score;
/** @} end of Score struct*/


/**
 * @brief Score constructor
 *
 * Allocate memory for Score object
 * @param
 * @return
 */
Score * initialize_score();

/**
 * @brief Update Score
 *
 * Increment by one the actual_score
 * @param
 * @return
 */
void update_score();

/**
 * @brief Reset Score
 *
 * Put actual_score equal to 0
 * @param
 * @return
 */
void reset_score();

/**
 * @brief Delete Score
 *
 * Free score memory
 * @param
 * @return
 */
void delete_score();

/**
 * @brief Draw Score
 *
 * Draw score in bottom right corner
 * @param
 * @return
 */
void draw_score();

/**
 * @brief Draw Date
 *
 * Draw date at a given location
 * @param date  Date in format "HH MM SS DD MM YY"
 * @param posX Position in x axis
 * @param posY Position in y axis
 * @return
 */
void draw_date(char * date, unsigned int posX, unsigned int posY);

/**
 * @brief Draw Number
 *
 * Draw number at a given location
 * @param num  Number to draw
 * @param posX Position in x axis
 * @param posY Position in y axis
 * @param size Number of characters Ex: Draw number 1 with size 4 : 0001
 * @return
 */
void draw_num(int num, unsigned int *posX, unsigned int *posY, unsigned int size);

/**
 * @brief Load numbers sprites
 *
 * @param
 * @return Array to Sprite pointers
 */
Sprite** loadNumbers();

/**
 * @brief Get Numbers
 *
 * @param
 * @return Array to Sprite pointers or Null if not loaded
 */
Sprite** getNumbers();

/** @} end of Score*/
#endif
