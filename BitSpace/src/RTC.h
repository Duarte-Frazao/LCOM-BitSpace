#ifndef __RTC_H
#define __RTC_H

/** @defgroup RTC RTC
 * @{
 *  RTC
 */

/** @name RTC */
/**@{RTC struct
 */
typedef struct {
	bool alarm_int;		/**< @brief Set when occur an alarm interrupt*/
	int IRQ_SET_RTC;	/**< @brief Set the bit */
	int hook_id_rtc;	/**< @brief RTC hook id*/
} Rtc;
/** @} end of RTC struct*/

/**
 * @brief Create RTC object
 *
 * Allocate memory for a new RTC object and return it
 *
 * @param
 * @return RTC object that was created
 */
Rtc* newRtc();

/**
 * @brief Enable RTC interrupts
 *
 * Subscribe RTC interrupts
 *
 * @param
 * @return RTC object that was enabled
 */
Rtc* enableRtc();

/**
 * @brief Returns RTC
 * @param
 * @return RTC object or NULL if it wasn't created
 */
Rtc* getRtc();

/**
 * @brief Reset RTC object
 *
 * Reset RTC object interrupts flags (alarm_int)
 *
 * @param
 * @return
 */
void resetRtc();

/**
 * @brief Erase RTC object
 *
 * Free memory and unsubscribe RTC interrupts
 *
 * @param
 * @return
 */
void eraseRtc();


/**
 * @brief Returns date
 *
 * Returns date in the following format:
 * "HH MM SS DD MM YY"
 *
 * @param
 * @return String date
 */
char * getDate();

/**
 * @brief Set Alarm Interrupt
 *
 * Set the time when will be issued an alarm interrupt
 * @param hrs Alarm hour or RTC_DTC to accept any value
 * @param min Alarm minute or RTC_DTC to accept any value
 * @param sec Alarm seconds or RTC_DTC to accept any value
 * @return Return 0 if successful or 1 otherwise
 */
int rtc_set_alarm(unsigned int hrs, unsigned int min, unsigned int sec);

/**
 * @brief RTC Interrupt Handler
 *
 * Determine interrupt (update, alarm, periodic) and call respective handler
 * @param
 * @return
 */
void rtc_ih(void);


void rtc_asm_handler(void);
/**
 * @brief RTC Subscribe Interrupts
 *
 * @param
 * @return Bit that indicates RTC interrupts set (IRQ_SET_RTC)
 */
int rtc_subscribe_int();

/**
 * @brief RTC Unsubscribe Interrupts
 *
 * @param
 * @return Return 0 if successful or 1 otherwise
 */
int rtc_unsubscribe();
/** @} end of RTC*/
#endif
