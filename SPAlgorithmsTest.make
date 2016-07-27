CC = gcc
OBJS = sp_algorithms_unit_test.o common_test_util.o sp_algorithms.o SPBPriorityQueue.o SPKDTree.o SPKDArray.o SPPoint.o SPList.o SPListElement.o
EXEC = sp_algorithms_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS) 
	$(CC) $(OBJS) -o $@
sp_algorithms_unit_test.o: $(TESTS_DIR)/sp_algorithms_unit_test.c $(TESTS_DIR)/unit_test_util.h SPPoint.h SPKDArray.h SPKDTree.h SPBPriorityQueue.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
common_test_util.o: $(TESTS_DIR)/common_test_util.c $(TESTS_DIR)/common_test_util.h SPPoint.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/common_test_util.c
sp_algorithms.o: sp_algorithms.c sp_algorithms.h SPBPriorityQueue.h SPKDTree.h SPPoint.h
	$(CC) $(COMP_FLAG) -c $*.c
SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h SPList.h SPListElement.h
	$(CC) $(COMP_FLAG) -c $*.c
SPList.o: SPList.c SPList.h SPListElement.h
	$(CC) $(COMP_FLAG) -c $*.c
SPListElement.o: SPListElement.c SPListElement.h
	$(CC) $(COMP_FLAG) -c $*.c
SPKDTree.o: SPKDTree.c SPKDTree.h SPKDArray.h SPPoint.h
	$(CC) $(COMP_FLAG) -c $*.c
SPKDArray.o: SPKDArray.c SPKDArray.h SPPoint.h 
	$(CC) $(COMP_FLAG) -c $*.c
SPPoint.o: SPPoint.c SPPoint.h
	$(CC) $(COMP_FLAG) -c $*.c
clean: 
	rm -f $(OBJS) $(EXEC)
