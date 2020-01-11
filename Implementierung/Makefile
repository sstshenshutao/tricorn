	srcFile = ./tricorn.c
	dstFile = ./debug.out
.PHONY: all build clean
all: build testDebug
build:
	gcc $(srcFile) -o $(dstFile) -lm
clean:
	rm -f *.aux *.bbl *.blg *.fls *.fdb_latexmk *.log *.pdf *~ Ausarbeitung.out
testDebug:
	$(dstFile) --r_start 3.5 --r_end 6.5 --i_start 1.7 --i_end 2.3 --res 0.1 --output './opop'
testOutput:
	gcc -c ./multicornTestDouble.c -o ./multicornTestDouble.o
	gcc ./tricorn.c ./multicornTestDouble.o -o $(dstFile)
	$(dstFile) --r_start 3.5 --r_end 6.5 --i_start 1.7 --i_end 2.3 --res 0.1 --output './opop'
testWithCImp:
	gcc -c ./multicornCImp.c -o ./multicornCImp.o
	gcc ./tricorn.c ./multicornCImp.o -o $(dstFile) -lm
	$(dstFile) --r_start "-5" --r_end 5.0 --i_start "-5.0" --i_end 5 --res 0.1 --output './opop'