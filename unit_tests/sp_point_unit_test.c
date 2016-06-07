#include "../SPPoint.h"
#include "unit_test_util.h"
#include <stdbool.h>

//Checks if copy Works
bool pointBasicCopyTest() {
	double data[2] = { 1.0, 1.0 };
	int dim = 2;
	int index = 1;
	SPPoint p = spPointCreate(data, dim, index);
	SPPoint q = spPointCopy(p);
	ASSERT_TRUE(spPointGetIndex(p) == spPointGetIndex(q));
	ASSERT_TRUE(spPointGetDimension(p) == spPointGetDimension(q));
	int i;
	for (i = 0; i < spPointGetDimension(p); i++) {
		ASSERT_TRUE(spPointGetAxisCoor(p, i) == spPointGetAxisCoor(q, i));
	}
	spPointDestroy(p);
	spPointDestroy(q);
	return true;
}

bool pointBasicL2Distance() {
	double data1[2] = { 1.0, 1.0 };
	double data2[2] = { 1.0, 0.0 };
	int dim1 = 2;
	int dim2 = 2;
	int index1 = 1;
	int index2 = 1;
	SPPoint p = spPointCreate((double *)data1, dim1, index1);
	SPPoint q = spPointCreate((double *)data2, dim2, index2);
	ASSERT_TRUE(spPointL2SquaredDistance(p,p) == 0.0);
	ASSERT_TRUE(spPointL2SquaredDistance(q,q) == 0.0);
	ASSERT_FALSE(spPointL2SquaredDistance(p,q) == 0.0);
	spPointDestroy(p);
	spPointDestroy(q);
	return true;
}

bool pointGettersTest() {
	double data1[2] = { 1.0, 1.0, 2.0 };
	int dim1 = 3;
	int index1 = 1;
	SPPoint p = spPointCreate((double *)data1, dim1, index1);
	ASSERT_TRUE(spPointGetAxisCoor(p, 0) == 1.0);
	ASSERT_TRUE(spPointGetAxisCoor(p, 1) == 1.0);
	ASSERT_FALSE(spPointGetAxisCoor(p, 2) == 1.0);
	ASSERT_TRUE(spPointGetAxisCoor(p, 2) == 2.0);
	ASSERT_TRUE(spPointGetIndex(p) == 1);
	ASSERT_TRUE(spPointGetDimension(p) == 3);
	spPointDestroy(p);
	return true;
}

bool pointDestroyTest() {
	double data1[2] = { 1.0, 1.0, 2.0 };
	int dim1 = 3;
	int index1 = 1;
	SPPoint p = spPointCreate((double *)data1, dim1, index1);
	spPointDestroy(p);
	ASSERT_TRUE(p == NULL);
	return true;
}



int main() {
	RUN_TEST(pointBasicCopyTest);
	RUN_TEST(pointBasicL2Distance);
	RUN_TEST(pointGettersTest);
	RUN_TEST(pointDestroyTest);

	return 0;
}
