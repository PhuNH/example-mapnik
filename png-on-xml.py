import os
import mapnik
m = mapnik.Map(1200,800)
#m.srs = '+init=epsg:3395'
style = 'png-on-xml.xml'
mapnik.load_map(m,style)

s=mapnik.Style()
rule=mapnik.Rule()
sym = mapnik.RasterSymbolizer()
#sym.scaling = 'mapnik.scaling_method.GAUSSIAN'
sym.opacity = 1
#sym.comp_op = "screen"
rule.symbols.append(sym)
s.rules.append(rule)
m.append_style('Raster Style',s)

lyr = mapnik.Layer('GDAL Layer')
#lyr.srs = '+proj=merc +lon_0=0 +k=1 +x_0=0 +y_0=0 +datum=WGS84 +units=m +no_defs'
lyr.datasource = mapnik.Gdal(file='../data/png-on-xml-data.png',bbox=(0,0,1000,1000))
lyr.styles.append('Raster Style')
m.layers.append(lyr)

m.zoom_all()
mapnik.render_to_file(m,'../output-mapnik/png-on-xml.png')
os.system('xdg-open ../output-mapnik/png-on-xml.png')