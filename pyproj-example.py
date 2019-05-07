import pyproj

lat1 = 0.0
long1 = 90.0
lat2 = 0.0
long2 = 180.0

geod = pyproj.Geod(ellps="WGS84")
angle1, angle2, distance = geod.inv(long1, lat1, long2, lat2)

print("{} {} {}".format(angle1, angle2, distance))