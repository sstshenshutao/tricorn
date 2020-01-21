	testCorrectionMain = ./test_correction.c
	testCorrectionMulticornCSrc = ./multicorn_c_test_double.c
	testCorrectionMulticornSSrc = ./packed_isnan_isinf.S
	testCorrectionMulticornCDst = ./multicorn_c_test_double.o
	testCorrectionDstFile = ./testCorrectionOutput
	testCorrectionDir = ./correction
.PHONY: all clean buildTestCorrection runTestCorrection
all: testCorrection
buildTestCorrection:
	gcc -c $(testCorrectionMulticornCSrc) -o $(testCorrectionMulticornCDst) -lm
	gcc $(testCorrectionMain) $(testCorrectionMulticornCDst) $(testCorrectionMulticornSSrc) -no-pie -lm -o $(testCorrectionDstFile)
runTestCorrection:
	if [ ! -d "$(testCorrectionDir)" ]; then mkdir -p "$(testCorrectionDir)"; fi	
	$(testCorrectionDstFile)
testCorrection: buildTestCorrection runTestCorrection
clean:
	rm -f *.o $(testCorrectionDstFile)