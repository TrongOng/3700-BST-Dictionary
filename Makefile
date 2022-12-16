output: main.o sampler.o
	g++ -o output main.o sampler.o

main.o: main.cpp bstDictionary.h sampler.h
	g++ -c main.cpp

sampler.o: sampler.cpp sampler.h
	g++ -c sampler.cpp
