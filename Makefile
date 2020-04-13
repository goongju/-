main : main.o copy.o mygets.o
	gcc -o main main.o copy.o mygets.o
main.o : main.c copy.h
	gcc -c main.c
copy.o : copy.c
	gcc -c copy.c
mygets.o: mygets.c
	gcc -c mygets.c

