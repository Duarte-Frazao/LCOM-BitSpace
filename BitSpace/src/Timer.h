#ifndef TIMER_H
#define TIMER_H

/** @defgroup Timer Timer
 * @{
 *  Timer
 */


 /** @name Timer */
 /**@{
  *
  * Timer struct
  */
typedef struct{
	int numInterruptions; 		/**< @brief Number of timer interruptions */
	int timerTickedFlag; 		/**< @brief Set when timer generated an interrupt */
	int hook_id_timer; 		/**< @brief Timer hook id */
	int IRQ_SET_TIMER; 		/**< @brief Timer IRQ */
}Timer;
/** @} end of Timer struct*/

/**
 * @brief Subscribes timer with timer object's hook id
 *
 * @return Returns bif of hook id
 */
int timer_subscribe_int();

/**
 * @brief Unsubscribes timer with timer object's hook id
 *
 * @return Returns 0 on sucess, 1 on error
 */
int timer_unsubscribe_int();

/**
 * @brief Handles timer related interrupts
 *
 * Increments number of interruptions and setes timerTickedFlag
 *
 * @return
 */
void timer_int_handler();

/**
 * @brief Sets timer with specified frequency
 *
 * @param timer Timer to change frequency
 * @param freq Frequency to set
 *
 * @return Returns 0 on sucess, 1 on error
 */
int timer_set_frequency(unsigned char timer, unsigned long freq);

/**
 * @brief Gets specified timer configuration
 *
 * @param timer Timer to get configuration
 * @param st Pointer to return timer configuration
 *
 * @return Returns 0 on sucess, 1 on error
 */
int timer_get_conf(unsigned char timer, unsigned char *st);

/**
 * @brief Creates a new timer object
 *
 * @return Returns timer object
 */
Timer* newTimer();

/**
 * @brief Enables timer
 *
 * Subscribes timer and set timer object's irq
 *
 * @return Returns timer object
 */
Timer* enableTimer();

/**
 * @brief Gets timer object
 *
 * @return Returns timer object
 */
Timer* getTimer();

/**
 * @brief Resets timer object
 *
 * Resets timerTickedFlag and number of interruptions
 *
 * @return
 */
void resetTimer();

/**
 * @brief Erases timer object
 *
 * Unsubscribes timer object
 * Free timer object
 *
 * @return
 */
void eraseTimer();

/**
 * @brief Displays game timer on the screen
 *
 * Responsible for time count on the game, and the bottom right clock in the game
 *
 * @return Returns 0 on sucess, 1 on error
 */
void displayTimer();

/** @} end of Timer*/

#endif
