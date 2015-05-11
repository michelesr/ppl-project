# Makefile Progetto Programmazione Procedurale e Logica        
# Sessione Autunnale 2011/2012 
# Michele Sorcinelli, Matricola n° 248412     

# Programma per la trasformazione di formule di logica dei predicati
#  in forma normale prenessa in formule in forma normale di Skolem  

# Compilazione del progetto
progetto: progetto.c Makefile
	gcc -ansi -Wall -O progetto.c -o progetto

# Compilazione con simboli di debug
db_progetto: progetto.c Makefile
	gcc -ansi -Wall -g progetto.c -o db_progetto

# Pulizia degli eseguibili
pulisci:
	rm -f progetto db_progetto
