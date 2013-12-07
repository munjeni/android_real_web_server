@echo off
rem zip up stuff 
latex crypt
makeindex crypt
pdflatex crypt
make clean
zip -9 \web\zips\crypt.zip *
