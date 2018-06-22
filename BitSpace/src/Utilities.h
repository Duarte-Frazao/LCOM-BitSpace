#define BIT(n) (0x01<<(n))

/**
 * @brief Constrains a value within limits
 *
 * @param max Max limit
 * @param min Min limit
 * @param num Number to constrain
 *
 * @return Returns constrained number
 */
int constrain(int num, int min, int max);
