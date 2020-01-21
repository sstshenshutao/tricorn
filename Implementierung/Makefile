	multicornS=./multicorn.S
	testIterationNumMain = ./iteration_num/iteration_number.c
	testIterationNumMulticornCSrc = ./iteration_num/multicorn_c_test_double_with_step.c
	testIterationNumMulticornCDst = ./iteration_num/multicorn_c_test_double.o
	testIterationNumDstFile = ./iteration_num/testIterationNumOutput
	testCorrectionMain = ./test_correction/test_correction.c
	testCorrectionMulticornCSrc = ./multicorn_c_test_double.c
	testCorrectionMulticornSSrc = $(multicornS)
	testCorrectionMulticornCDst = ./test_correction/multicorn_c_test_double.o
	testCorrectionDstFile = ./test_correction/testCorrectionOutput
	testCorrectionDir = ./test_correction/correction
	testPerformanceMain = ./test_performance/test_performance.c
	testPerformanceMulticornCSrc = ./multicorn_c_test_double.c
	testPerformanceMulticornSSrc = $(multicornS)
	testPerformanceMulticornCDst = ./test_performance/multicorn_c_test_double.o
	testPerformanceDstFile = ./test_performance/testPerformanceOutput
	entryMain=./tricorn.c
	multicornCImpSrc=./multicornCImp.c
	multicornCImpDst=./multicornCImp.o
	srcFile = $(entryMain) $(multicornS)
	dstFile = ./tricorn
.PHONY: all example testIterationNum testCorrection testPerformance build clean runMain runMainExample buildWithCImp buildTestIterationNum runTestIterationNum buildTestCorrection runTestCorrection buildTestPerformance runTestPerformance
all: build runMain
example: build runMainExample
testIterationNum: buildTestIterationNum runTestIterationNum
testCorrection: buildTestCorrection runTestCorrection
testPerformance: buildTestPerformance runTestPerformance
build:
	gcc $(srcFile) -o $(dstFile) -lm -O3
clean:
	rm -f $(multicornCImpDst) $(dstFile) $(testIterationNumDstFile) $(testIterationNumMulticornCDst) $(testCorrectionMulticornCDst) $(testCorrectionDstFile) $(testPerformanceMulticornCDst) $(testPerformanceDstFile)
	rm -r $(testCorrectionDir)
runMain:
	echo "this program called: $(dstFile), run $(dstFile) -h for help"
runMainExample:
	$(dstFile) --r_start -2 --r_end 2.0 --i_start -2 --i_end 2 --res 0.01 --output './outputExample.bmp'
buildWithCImp:
	gcc -c $(multicornCImpSrc) -o $(multicornCImpDst)
	gcc $(entryMain) $(multicornCImpDst) -o $(dstFile) -lm -O3
buildTestIterationNum:
	gcc -c $(testIterationNumMulticornCSrc) -o $(testIterationNumMulticornCDst) -lm
	gcc $(testIterationNumMain) $(testIterationNumMulticornCDst) -lm -o $(testIterationNumDstFile)
runTestIterationNum:
	$(testIterationNumDstFile)
buildTestCorrection:
	gcc -c $(testCorrectionMulticornCSrc) -o $(testCorrectionMulticornCDst) -lm
	gcc $(testCorrectionMain) $(testCorrectionMulticornCDst) $(testCorrectionMulticornSSrc) -no-pie -lm -o $(testCorrectionDstFile)
runTestCorrection:
	if [ ! -d "$(testCorrectionDir)" ]; then mkdir -p "$(testCorrectionDir)"; fi	
	$(testCorrectionDstFile)
buildTestPerformance:
	gcc -c $(testPerformanceMulticornCSrc) -o $(testPerformanceMulticornCDst) -lm -O3
	gcc $(testPerformanceMain) $(testPerformanceMulticornCDst) $(testPerformanceMulticornSSrc) -no-pie -lm -o $(testPerformanceDstFile) -O3
runTestPerformance:
	$(testPerformanceDstFile)


