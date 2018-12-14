default: prog

prog:
	gcc -o ftserver ftserver.c
	echo "to run the program type in "ftserver" "
clean:
	rm ftserver

cleanall: 
	rm ftserver

