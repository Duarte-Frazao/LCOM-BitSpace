/** @defgroup
 * @{
 *  RTC Macros
 */

#define RTC_IRQ_LINE				8		/**< @brief RTC IRQ line */

#define RTC_ADDR_REG				0x70	/**< @brief input buffer address for RTC registers */
#define RTC_DATA_REG				0x71	/**< @brief input buffer address for RTC arguments */

//Address - Time
#define RTC_SEC						0x00	/**< @brief Seconds address */
#define RTC_MIN						0x02	/**< @brief Minutes address */
#define RTC_HRS						0x04	/**< @brief Hours address */
#define RTC_WKD						0x06	/**< @brief Week day address */
#define RTC_DAY						0x07	/**< @brief Day address */
#define RTC_MTH						0x08	/**< @brief Month address */
#define RTC_YRS						0x09	/**< @brief Year address */

//Address - Alarm
#define RTC_SEC_A					0x01	/**< @brief Alarm seconds address */
#define RTC_MIN_A					0x03	/**< @brief Alarm minutes address */
#define RTC_HRS_A					0x05	/**< @brief Alarm hours address */


#define RTC_REG_A					0x0A	/**< @brief RTC register A address */
#define RTC_REG_B					0x0B	/**< @brief RTC register B address */
#define RTC_REG_C					0x0C	/**< @brief RTC register C address */
#define RTC_REG_D					0x0D	/**< @brief RTC register D address */


//RTC_REG_A FLAGS
#define RTC_UIP						0x80	/**< @brief Mask update in progress */

//RTC REG_B interrupts
#define RTC_UIE						0x10	/**< @brief Enable update interrupts */
#define RTC_AIE						0x20	/**< @brief Enable alarm interrupts */
#define RTC_PIE						0x40	/**< @brief Enable periodic interrupts */

//RTC_REG_C FLAGS
#define RTC_UF						0x10	/**< @brief Mask occurred update interrupts */
#define RTC_AF						0x20	/**< @brief Mask occurred alarm interrupts */
#define RTC_PF						0x40	/**< @brief Mask occurred periodic interrupts */

#define RTC_DTC						0xFF	/**< @brief RTC don't care value */

/** @} end of RTC Macros*/
