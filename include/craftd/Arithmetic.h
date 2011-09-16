/*
* Adapted by Kevin M. Bowling for compiler inlining
*
* Contains significant code and influence from the book:
* "C Interfaces and Implementations" by David R. Hanson (ISBN 0-201-49841-3)
*
* See https://github.com/kev009/cii/blob/master/LICENSE for the original MIT
* license.
*/

#ifndef CRAFTD_ARITHMETIC_H
#define CRAFTD_ARITHMETIC_H

#define ABS(x) ((x < 0) ? -x : x)

/**
 * Return the maximum of two values
 *
 * @param x an integer value
 * @param y an integer value
 *
 * @return greater of the two parameters
 */
static inline
int
CD_Max (int x, int y)
{
  return x > y ? x : y;
}

/**
 * Return the minimum of two values
 *
 * @param x an integer value
 * @param y an integer value
 *
 * @return smaller of the two parameters
 */
static inline
int
CD_Min (int x, int y)
{
	return x > y ? y : x;
}

/**
 * Perform well defined integer division
 *
 * The C standard has goofy semantics wrt negative numbers and integer division
 * This one always truncates toward left on number line and works as expected
 *
 * @param x the dividend
 * @param y the divisor
 *
 * @return the integer quotient
 */
static inline
int
CD_Div (int x, int y)
{
	if (-13 / 5 == -2 && (x < 0) != (y < 0) && x % y != 0) {
		return x / y - 1;
	}
	else {
		return x / y;
	}
}

/**
 * Perform well defined modulo division
 *
 * The C standard has goofy semantics wrt negative numbers and modulo division
 * This one always truncates toward left on number line and works as expected
 *
 * @param x the dividend
 * @param y the divisor
 *
 * @return the integer quotient
 */
static inline
int
CD_Mod (int x, int y)
{
	if (-13 / 5 == -2 && (x < 0) != (y < 0) && x % y != 0) {
		return x % y + y;
	}
	else {
		return x % y;
	}
}

/**
 * Perform well defined integer floor
 *
 * In the x/y, returns the integer to the left on the number line
 *
 * @param x dividend
 * @param y divisior
 *
 * @return floor of x/y
 */
static inline
int
CD_Floor (int x, int y)
{
	return CD_Div(x, y);
}

/**
 * Perform well defined integer ceiling
 * In the x/y, returns the integer to the right on the number line
 *
 * @param x dividend
 * @param y divisior
 *
 * @return ceiling of x/y
 */
static inline
int
CD_Ceiling (int x, int y)
{
	return CD_Div(x, y) + (x % y != 0);
}

#endif
