/*
 * main_aux.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

extern "C" {
#include "SPPoint.h"
#include "SPConfig.h"
#include "SPKDTree.h"
}

typedef enum sp_database_creation_msg_t {
	SP_DATABASE_CREATION_FEATURES_CONFIGUATION_ERROR,
	SP_DATABASE_CREATION_FEATURES_EXTRACTION_ERROR,
	SP_DATABASE_CREATION_INVALID_ARGUMENT,
	SP_DATABASE_CREATION_FEATURE_FILE_MISSING,
	SP_DATABASE_CREATION_CONFIG_ERROR,
	SP_DATABASE_CREATION_ALLOC_FAIL,
	SP_DATABASE_CREATION_WRITE_ERROR,
	SP_DATABASE_CREATION_LOAD_ERROR,
	SP_DATABASE_CREATION_SUCCESS
} SP_DATABASE_CREATION_MSG;

SPKDTreeNode createImagesSearchTree(const SPConfig config, SP_DATABASE_CREATION_MSG *msg);

int *findSimilarImagesIndices(const SPConfig config, const char *queryImagePath, const SPKDTreeNode searchTree, int *resultsCount, ImageProc *ip);


#endif /* MAIN_AUX_H_ */
