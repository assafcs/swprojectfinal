/*
 * common_test_util.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#ifndef UNIT_TESTS_COMMON_TEST_UTIL_H_
#define UNIT_TESTS_COMMON_TEST_UTIL_H_

#include <stdbool.h>
#include "../SPPoint.h"

bool pointsArrayEqualNotSame(SPPoint* aPointsArray, SPPoint *bPointsArray, int size);
bool pointsEqualNotSame(SPPoint aPoint, SPPoint bPoint);
SPPoint twoDPoint(double x, double y);
SPPoint threeDPoint(double x, double y, double z);

#endif /* UNIT_TESTS_COMMON_TEST_UTIL_H_ */
