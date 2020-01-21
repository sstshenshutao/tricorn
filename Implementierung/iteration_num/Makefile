	testIterationNumMain = ./iteration_number.c
	testIterationNumMulticornCSrc = ./multicorn_c_test_double.c
	testIterationNumMulticornCDst = ./multicorn_c_test_double.o
	testIterationNumDstFile = ./testIterationNumOutput
.PHONY: all clean buildTestIterationNum runTestIterationNum
all: testIterationNum
buildTestIterationNum:
	gcc -c $(testIterationNumMulticornCSrc) -o $(testIterationNumMulticornCDst) -lm
	gcc $(testIterationNumMain) $(testIterationNumMulticornCDst) -lm -o $(testIterationNumDstFile)
runTestIterationNum:
	$(testIterationNumDstFile)
testIterationNum: buildTestIterationNum runTestIterationNum
clean:
	rm -f *.o $(testIterationNumDstFile)