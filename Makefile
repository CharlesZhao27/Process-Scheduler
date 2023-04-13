EXE=allocate

$(EXE): main.c linkedList.c linkedList.h memory.c memory.h
	cc -Wall -o $(EXE) $< linkedList.c linkedList.h memory.c memory.h -g

clean:
	rm allocate *.o

format:
	clang-format -style=file -i *.c