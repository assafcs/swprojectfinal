CC = gcc
OBJS = sp_util_unit_test.o sp_util.o
EXEC = sp_util_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS) 
	$(CC) $(OBJS) -o $@
sp_util_unit_test.o: $(TESTS_DIR)/sp_util_unit_test.c $(TESTS_DIR)/unit_test_util.h sp_util.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
sp_util.o: sp_util.c sp_util.h
	$(CC) $(COMP_FLAG) -c $*.c
clean: 
	rm -f $(OBJS) $(EXEC)