all: text

text:
	latex text
#	bibtex text
#	latex text
	latex text
	dvips text.dvi -o text.ps
	dvipdf text.dvi text.pdf

clean:
	rm -f *.aux
	rm -f *.dvi
	rm -f *.log
	rm -f *.toc
	rm -f *.blg
	rm -f *.bbl
	rm -f text.ps
	rm -f text.pdf
