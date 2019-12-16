all: pdf html

pdf: pdf/scripting.pdf
html: html/scripting/index.html

pdf/scripting.pdf: scripting.lyx
	mkdir -p pdf
	lyx --export pdflatex scripting.lyx
	pdflatex scripting.tex
	pdflatex scripting.tex
	mv scripting.pdf pdf/

html/scripting/index.html: scripting.lyx
	mkdir -p html
	lyx --export pdflatex scripting.lyx
	latex2html scripting.tex
	mv scripting html/

clean:
	rm -rf *.aux
	rm -rf *.log
	rm -rf *.out
	rm -rf *.tex


