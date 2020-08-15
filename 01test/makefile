#link
server_snd_date.exe:server_snd_date.o
	gcc -o server_snd_date.exe server_snd_date.o -lsctp
#compare
server_snd_date.o:server_snd_date.c
	gcc -o server_snd_date.o -c server_snd_date.c
clean:
	rm -f *.o
