#! /bin/bash
#Author Łukasz Buśko
#Contact buskol.waw.pl@gmail.com
#GNU/General Public License
#
OUTFILE1=MainFrame.py
OUTFILE2=AdminPanel.py
OUTDIR=src
UIFILE1=ui/MainFrame.ui
UIFILE2=ui/AdminPanel.ui
UIFILES="untitled.ui AdminPanel.ui"
log=build.log
#
DATA=$(date +%F"_"%X)
#
mv $OUTDIR/$OUTFILE1 $OUTDIR/$OUTFILE1'_'$DATA
mv $OUTDIR/$OUTFILE2 $OUTDIR/$OUTFILE2'_'$DATA
pyuic4 $UIFILE1 -o $OUTDIR/$OUTFILE1 > $log
pyuic4 $UIFILE2 -o $OUTDIR/$OUTFILE2 >>$log
