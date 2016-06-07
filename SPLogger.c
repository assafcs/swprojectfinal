#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//File open mode
#define SP_LOGGER_OPEN_MODE "w"

#define ERROR_HEADER "---ERROR---"
#define WARNING_HEADER "---WARNING---"
#define INFO_HEADER "---INFO---"
#define DEBUG_HEADER "---DEBUG---"
#define FILE_ROW_PREFIX "- file: "
#define FUNCTION_ROW_PREFIX "- function: "
#define LINE_ROW_PREFIX "- line: "
#define MSG_ROW_PREFIX "- message: "
#define NEW_LINE "/n"

SP_LOGGER_MSG generalLoggerPrint(const char* header, const char* msg, const char* file,
		const char* function, const int line);

// Global variable holding the logger
SPLogger logger = NULL;

struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(*logger));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	if (!logger) {
		return;
	}
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}
	free(logger);//free allocation
	logger = NULL;
}

SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file,
		const char* function, const int line) {
	return generalLoggerPrint(ERROR_HEADER, msg, file, function, line);
}

SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file,
		const char* function, const int line) {
	return generalLoggerPrint(WARNING_HEADER, msg, file, function, line);
}

SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file,
		const char* function, const int line) {
	return generalLoggerPrint(DEBUG_HEADER, msg, file, function, line);
}

SP_LOGGER_MSG spLoggerPrintInfo(const char* msg) {
	if (logger == NULL){
		return SP_LOGGER_UNDIFINED;
	}
	if (msg == NULL){
		return SP_LOGGER_INVAlID_ARGUMENT;
	}
	if (logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL || logger->level == SP_LOGGER_INFO_WARNING_ERROR_LEVEL){
		int res = fprintf(logger->outputChannel, INFO_HEADER, NEW_LINE);
		if (res < 0){
			return SP_LOGGER_WRITE_FAIL;
		}
		res = fprintf(logger->outputChannel, MSG_ROW_PREFIX, msg, NEW_LINE);
		if (res < 0){
			return SP_LOGGER_WRITE_FAIL;
		}
		res = fprintf(logger->outputChannel, NEW_LINE);
		if (res < 0){
			return SP_LOGGER_WRITE_FAIL;
		}
	}
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintMsg(const char* msg) {
	if (logger == NULL){
		return SP_LOGGER_UNDIFINED;
	}
	if (msg == NULL){
		return SP_LOGGER_INVAlID_ARGUMENT;
	}
	int res = fprintf(logger->outputChannel, MSG_ROW_PREFIX, msg, NEW_LINE);
	if (res < 0){
		return SP_LOGGER_WRITE_FAIL;
	}
	res = fprintf(logger->outputChannel, NEW_LINE);
	if (res < 0){
		return SP_LOGGER_WRITE_FAIL;
	}
	return SP_LOGGER_SUCCESS;
}

// In order to reduce code lines, mutual logic of ERROR, WARNING and DEBUG is
// implemented in one place.
// This method validates the input and prints whats needed.
SP_LOGGER_MSG generalLoggerPrint(const char* header, const char* msg, const char* file,
		const char* function, const int line) {
	if (logger == NULL){
		return SP_LOGGER_UNDIFINED;
	}
	if (msg == NULL || file == NULL || function == NULL || line < 0){
		return SP_LOGGER_INVAlID_ARGUMENT;
	}
	if ((logger->level == SP_LOGGER_ERROR_LEVEL && header == ERROR_HEADER) || (
			logger->level == SP_LOGGER_WARNING_ERROR_LEVEL && (header == WARNING_HEADER || header == ERROR_HEADER)) || (
					logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL)){
		int res = loggerDetailLines(header, msg, file, function, line);
		if (res == 0){
			return SP_LOGGER_WRITE_FAIL;
		}
	}
	return SP_LOGGER_SUCCESS;
}


// Method for printing in the assignment required format
int loggerDetailLines(const char* header, const char* msg, const char* file, const char* function, const int line){
	int res = fprintf(logger->outputChannel, header, NEW_LINE);
	if (res < 0){
		return 0;
	}
	res = fprintf(logger->outputChannel, FILE_ROW_PREFIX, file, NEW_LINE);
	if (res < 0){
		return 0;
	}
	res = fprintf(logger->outputChannel, FUNCTION_ROW_PREFIX, function, NEW_LINE);
	if (res < 0){
		return 0;
	}
	res = fprintf(logger->outputChannel, LINE_ROW_PREFIX, line, NEW_LINE);
	if (res < 0){
		return 0;
	}
	res = fprintf(logger->outputChannel, MSG_ROW_PREFIX, msg, NEW_LINE);
	if (res < 0){
		return 0;
	}
	res = fprintf(logger->outputChannel, NEW_LINE);
	if (res < 0){
		return 0;
	}
	return 1;
}
