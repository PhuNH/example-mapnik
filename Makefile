all:
	g++ cpp-mapnik.cpp -I/usr/include/mapnik -std=c++11 -Wall -g -o cpp-mapnik -lmapnik -L/usr/lib64 -L/usr/lib -licuuc
clean:
	rm cpp-mapnik
