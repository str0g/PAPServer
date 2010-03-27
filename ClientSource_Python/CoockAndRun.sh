#! /bin/bash
#Author Łukasz Buśko
#Contact buskol.waw.pl@gmail.com
#GNU/General Public License
#
LOG=build.log
DATA=$(date +%F"_"%X)
./Makefile.py >$LOG
if [ $? -eq 1 ]; then
 touch "Compilation_Failed_"$DATA
 exit $?
fi
mv -f src/*.pyc bin/
./start.sh
