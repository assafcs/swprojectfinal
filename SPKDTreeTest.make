CC = gcc
OBJS = sp_kd_tree_unit_test.o SPKDTree.o SPKDArray.o SPPoint.o
EXEC = sp_kd_tree_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS) 
	$(CC) $(OBJS) -o $@
sp_kd_tree_unit_test.o: $(TESTS_DIR)/sp_kd_tree_unit_test.c $(TESTS_DIR)/unit_test_util.h SPPoint.h SPKDArray.h SPKDTree.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
SPKDTree.o: SPKDTree.c SPKDTree.h SPKDArray.h SPPoint.h
	$(CC) $(COMP_FLAG) -c $*.c
SPKDArray.o: SPKDArray.c SPKDArray.h SPPoint.h 
	$(CC) $(COMP_FLAG) -c $*.c
SPPoint.o: SPPoint.c SPPoint.h
	$(CC) $(COMP_FLAG) -c $*.c
clean: 
	rm -f $(OBJS) $(EXEC)
