all:
	g++ cpp-mapnik.cpp -I/usr/include/mapnik -I/usr/include/gdal -DBIGINT -std=c++11 -Wall -O2 -g -o cpp-mapnik -lmapnik -lgdal -L/usr/lib64 -L/usr/lib -licuuc
clean:
	rm cpp-mapnik
