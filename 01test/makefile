server_snd_date.exe:server_snd_date.o
	gcc -lsctp server_snd_date.o -o server_snd_date.exe
client_snd_date.o: server_snd_date.c
	gcc -c server_snd_date.c
clean:
	rm -f *.o
