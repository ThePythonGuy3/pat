## PAT
Project Administration Tool (PAT) is an experimental Version Control software.

_**Use under your own risk, project very early in development.**_

---
**1**. Creating a static library from `zlib` (Once):

	cd zlib
	gcc -c *.c
	ar -rc libz.a *.o

**2**. Compiling:

    g++ -o pat.exe main.cpp -L./zlib -lz -O3 -std=c++20

