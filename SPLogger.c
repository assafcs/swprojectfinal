#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
#define NEW_LINE "\n"

// [R] messages consts
#define FILE_ROW_R_MSG "File: "
#define LINE_ROW_R_MSG "Line: "
#define MSG_ROW_R_MSG "Message: "
#define NEW_LINE "\n"


SP_LOGGER_MSG generalLoggerPrint(const char* header, const char* msg, const char* file,
		const char* function, const int line);
int loggerDetailLines(const char* header, const char* msg, const char* file, const char* function, const int line);
int loggerPrintRow(const char* prefix, const char* info);
int loggerPrintText(const char* text);
int loggerPrintLine(const char* prefix, const int line);

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
		int res = loggerPrintText(INFO_HEADER);
		if (res < 0){
			return SP_LOGGER_WRITE_FAIL;
		}
		res = loggerPrintRow(MSG_ROW_PREFIX, msg);
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
	int res = loggerPrintRow(MSG_ROW_PREFIX, msg);
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
	if ((logger->level == SP_LOGGER_ERROR_LEVEL && strcmp(header, ERROR_HEADER) == 0) || (
			logger->level == SP_LOGGER_WARNING_ERROR_LEVEL && (strcmp(header, WARNING_HEADER) == 0  || strcmp(header, ERROR_HEADER) == 0)) || (
					logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL)){
		int res = loggerDetailLines(header, msg, file, function, line);
		if (res == 0){
			return SP_LOGGER_WRITE_FAIL;
		}
	}
	return SP_LOGGER_SUCCESS;
}


// Method for printing in the assignment required format
int loggerDetailLines(const char* header, const char* msg, const char* file, const char* function, const int line) {
	int res = loggerPrintText(header);
	if (res < 0){
		return 0;
	}
	res = loggerPrintRow(FILE_ROW_PREFIX, file);
	if (res < 0){
		return 0;
	}
	res = loggerPrintRow(FUNCTION_ROW_PREFIX, function);
	if (res < 0){
		return 0;
	}
	res = loggerPrintLine(LINE_ROW_PREFIX, line);
	if (res < 0){
		return 0;
	}
	res = loggerPrintRow(MSG_ROW_PREFIX, msg);
	if (res < 0){
		return 0;
	}
	return 1;
}

// fprintf or fprint text (mostly headers)
int loggerPrintText(const char* text) {
	int res;
	if (logger->isStdOut){
		res = printf("%s%s", text, NEW_LINE);
	} else {
		res = fprintf(logger->outputChannel, "%s%s", text, NEW_LINE);
	}
	return res;
}

// fprintf or fprint row with prefix and info
int loggerPrintRow(const char* prefix, const char* info) {
	int res;
	if (logger->isStdOut){
		res = printf("%s%s%s", prefix, info, NEW_LINE);
	} else {
		res = fprintf(logger->outputChannel, "%s%s%s", prefix, info, NEW_LINE);
	}
	return res;
}

// fprintf or fprint line prefix and line number
int loggerPrintLine(const char* prefix, const int line) {
	int res;
	if (logger->isStdOut){
		res = printf("%s%d%s", prefix, line, NEW_LINE);
	} else {
		res = fprintf(logger->outputChannel, "%s%d%s", prefix, line, NEW_LINE);
	}
	return res;
}

void printRErrorMsg(const char* filename, const int line, const char* msg) {
	printf("%s%s%s", FILE_ROW_R_MSG, filename, NEW_LINE);
	printf("%s%d%s", LINE_ROW_R_MSG, line, NEW_LINE);
	printf("%s%s%s", MSG_ROW_R_MSG, msg, NEW_LINE);
}
