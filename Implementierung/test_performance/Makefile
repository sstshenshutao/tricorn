	testPerformanceMain = ./test_performance.c
	testPerformanceMulticornCSrc = ./multicorn_c_test_double.c
	testPerformanceMulticornSSrc = ./packed_isnan_isinf.S
	testPerformanceMulticornCDst = ./multicorn_c_test_double.o
	testPerformanceDstFile = ./testPerformanceOutput
.PHONY: all clean buildTestPerformance runTestPerformance
all: testPerformance
buildTestPerformance:
	gcc -c $(testPerformanceMulticornCSrc) -o $(testPerformanceMulticornCDst) -lm -O3
	gcc $(testPerformanceMain) $(testPerformanceMulticornCDst) $(testPerformanceMulticornSSrc) -no-pie -lm -o $(testPerformanceDstFile) -O3
runTestPerformance:
	$(testPerformanceDstFile)
testPerformance: buildTestPerformance runTestPerformance
clean:
	rm -f *.o $(testPerformanceDstFile)