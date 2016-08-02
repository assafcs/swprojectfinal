/*
 * main.cpp

 *
 *  Created on: Jul 24, 2016
 *      Author: mataneilat
 */

#include <stdio.h>
#include <stdlib.h>
#include "main_aux.h"
extern "C" {
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
}

int main() {

	SP_CONFIG_MSG resultMSG;
	SPConfig config = spConfigCreate("spconfig.config", &resultMSG);

	SP_DATABASE_CREATION_MSG msg;
	SPPoint **database = createImagesDatabase(config, &msg);

	printf("Random Value: %.2f", spPointGetAxisCoor(database[10][65], 18));
	return 0;
}


