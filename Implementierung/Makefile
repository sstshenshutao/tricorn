	srcFile = ./tricorn.c
	dstFile = ./debug.out
.PHONY: all build clean
all: build testDebug
build:
	gcc $(srcFile) -o $(dstFile)
clean:
	rm -f *.aux *.bbl *.blg *.fls *.fdb_latexmk *.log *.pdf *~ Ausarbeitung.out
testDebug:
	$(dstFile) --r_start 3.5 --r_end 6.5 --i_start 1.7 --i_end 2.3 --res 0.7 --output './opop'