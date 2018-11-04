main.pdf: main.tex sp2-p-ttt.f.csv rps pacman
	pdflatex -shell-escape main.tex
	bibtex main
	#pdflatex main.tex
sp2-p-ttt.f.csv: ../logs/sp2-p-tictactoe.csv
	cat $< | grep 'cycle\|^[0-9]*0,' >$@
rps: sp1-rps.csv   sp1-p-rps.csv sp2-rps.csv   sp2-p-rps.csv sp3-rps.csv   sp4-rps.csv   sp5-rps.csv   sp7-rps.csv   sp6-rps.csv   sp8-rps.csv   sp9-rps.csv   spA-rps.csv   spB-rps.csv   spC-rps.csv   spD-rps.csv   spE-rps.csv   
sp1-rps.csv:   ../logs/sp1-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
sp1-p-rps.csv: ../logs/sp1-p-rps.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
sp2-rps.csv:   ../logs/sp2-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
sp2-p-rps.csv: ../logs/sp2-p-rps.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
sp3-rps.csv:   ../logs/sp3-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
sp4-rps.csv:   ../logs/sp4-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
sp5-rps.csv:   ../logs/sp5-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
sp7-rps.csv:   ../logs/sp7-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
sp6-rps.csv:   ../logs/sp6-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
sp8-rps.csv:   ../logs/sp8-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
sp9-rps.csv:   ../logs/sp9-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
spA-rps.csv:   ../logs/spA-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
spB-rps.csv:   ../logs/spB-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
spC-rps.csv:   ../logs/spC-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
spD-rps.csv:   ../logs/spD-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
spE-rps.csv:   ../logs/spE-rps.csv  
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@

pacman: pm-32-1.csv pm-32-1-d.csv pm-32-1-e.csv pm-32-1-o.csv pm-64-1.csv pm-96-1.csv pm-128-1.csv pm-32-2.csv pm-32-1-avg.csv pm-32-1-d-avg.csv pm-32-1-e-avg.csv pm-32-1-o-avg.csv pm-64-1-avg.csv pm-96-1-avg.csv pm-128-1-avg.csv pm-32-2-avg.csv pm-64-4-e-avg.csv pm-96-4-e-avg.csv pm-32-2.csv pm-64-4-e.csv pm-96-4-e.csv pm-96-4-d.csv pm-16-1-avg.csv pm-16-2-avg.csv pm-16-4-e-avg.csv pm-32-4-e-avg.csv pm-112-1.csv pm-48-1-e.csv pm-48-1-ee.csv pm-48-1.csv pm-80-1.csv pm-16-1.csv pm-16-2.csv pm-16-4-e.csv pm-32-4-e.csv pm-64-4-e.csv pm-96-4-d-avg.csv

pm-32-2-avg.csv: ../logs/pm-32-2.csv
	cat $< | runghc avg.hs > $@
pm-64-1-avg.csv: ../logs/pm-64-1.csv
	cat $< | runghc avg.hs > $@
pm-96-1-avg.csv: ../logs/pm-96-1.csv
	cat $< | runghc avg.hs > $@
pm-128-1-avg.csv: ../logs/pm-128-1.csv
	cat $< | runghc avg.hs > $@
pm-32-1-d-avg.csv:../logs/pm-32-1-d.csv
	cat $< | runghc avg.hs > $@
pm-32-1-e-avg.csv:../logs/pm-32-1-e.csv
	cat $< | runghc avg.hs > $@
pm-32-1-o-avg.csv:../logs/pm-32-1-o.csv
	cat $< | runghc avg.hs > $@
pm-32-1-avg.csv: ../logs/pm-32-1.csv
	cat $< | runghc avg.hs > $@
pm-64-4-e-avg.csv: 	../logs/pm-64-4-e.csv
	cat $< | runghc avg.hs > $@
pm-96-4-e-avg.csv: 	../logs/pm-96-4-e.csv
	cat $< | runghc avg.hs > $@
pm-96-4-d-avg.csv: 	../logs/pm-96-4-d.csv
	cat $< | runghc avg.hs > $@
pm-16-1-avg.csv:    ../logs/pm-16-1.csv 
	cat $< | runghc avg.hs > $@
pm-16-2-avg.csv:    ../logs/pm-16-2.csv 
	cat $< | runghc avg.hs > $@
pm-16-4-e-avg.csv:    ../logs/pm-16-4-e.csv
	cat $< | runghc avg.hs > $@
pm-32-4-e-avg.csv:    ../logs/pm-32-4-e.csv
	cat $< | runghc avg.hs > $@



pm-32-2.csv: ../logs/pm-32-2.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-64-1.csv: ../logs/pm-64-1.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-96-1.csv: ../logs/pm-96-1.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-128-1.csv: ../logs/pm-128-1.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-32-1-d.csv:../logs/pm-32-1-d.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-32-1-e.csv:../logs/pm-32-1-e.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-64-1-e.csv:../logs/pm-64-1-e.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-96-1-e.csv:../logs/pm-96-1-e.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-64-4-e.csv:../logs/pm-64-4-e.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-96-4-e.csv:../logs/pm-96-4-e.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-96-4-d.csv:../logs/pm-96-4-d.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-32-1-o.csv:../logs/pm-32-1-o.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-32-1.csv: ../logs/pm-32-1.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-16-1.csv: ../logs/pm-16-1.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-16-2.csv: ../logs/pm-16-2.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-16-4-e.csv: ../logs/pm-16-4-e.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-32-4-e.csv: ../logs/pm-32-4-e.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@

pm-112-1.csv: ../logs/pm-112-1.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-48-1-e.csv: ../logs/pm-48-1-e.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-48-1-ee.csv: ../logs/pm-48-1-ee.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-48-1.csv: ../logs/pm-48-1.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@
pm-80-1.csv: ../logs/pm-80-1.csv
	cat $< | egrep 'cycle|^[0-9]{1,3}0*,' >$@


