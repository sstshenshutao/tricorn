# Implementation

- the main program is ./tricorn, already compiled. You can use make to compile it again.
- other test programs, such as `testCorrection`, `testIterationNum`, and `testPerformance`, are just provided by source code, use Makefile to run them!!!
- please always use makefile to run `testCorrection`, because it is folder dependence. The makefile will check if the folder is existed.
- the autotest program is not integrated into make::all, because it takes long time to run the test cases. So make just compile the main program.
- Makefile supported instructions:

* all: build runMain
-Die Hauptdatei von Compiler generieren und dann ausführen.

* example: build runMainExample
-Die Hauptdatei von Compiler generieren und dann mit vordefinierte Eingaben
ausführen.

* testIterationNum: buildTestIterationNum runTestIterationNum
–Datei,die die rationale Nummer für Iteration in Hauptprogram testet, von Compiler
generieren und dann ausführen.

* testCorrection: buildTestCorrection runTestCorrection
–Datei, die die Korrektheit dieses Programs per Vergleich mit dem Standardscode
testet, von Compiler generieren und dann ausführen.

* testPerformance: buildTestPerformance runTestPerformance
– Datei,die die Performance dieses Programs per Vergleich mit dem Standardscode
testet, von Compiler generieren und dann ausführen.

* clean
-Löschen alle frühzeitige Operationen.

* buildWithCImp
-Kompilieren und führen das ganzen Projekt mit einer C-Datei als Ersetzung des
Assemble-Anteils aus.
