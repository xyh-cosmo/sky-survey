#!/bin/bash
TEX=xelatex
TEXFILE='CSS-OS'
BIBFILE='CSS-OS'
${TEX} -synctex=1 ${TEXFILE}.tex
bibtex ${BIBFILE}.aux


mv ${BIBFILE}.bbl ${BIBFILE}.bbl.bak
m4 -D\em=\it ${BIBFILE}.bbl.bak > ${BIBFILE}.bbl
rm ${BIBFILE}.bbl.bak
${TEX}  -synctex=1 ${BIBFILE}.tex
${TEX}  -synctex=1 ${BIBFILE}.tex

PDFviewer=""
SystemName=`uname`
if [[ $SystemName == "Linux" ]]; then
	PDFviewer="xdg-open"
elif [[ $SystemName == "Darwin" ]]; then
	PDFviewer="open"
fi

${PDFviewer} "${TEXFILE}.pdf"
