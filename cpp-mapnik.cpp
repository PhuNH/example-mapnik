#include <mapnik/map.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/feature_type_style.hpp>
#include <mapnik/datasource.hpp>
#include <mapnik/layer.hpp>

#include <gdal.h>
#include <ogrsf_frmts.h>

using namespace mapnik;

color GetColor(double v, double vmin, double vmax) {
    color c = color(255, 255, 255); // white
    double dv;
    
    if (v < vmin) v = vmin;
    if (v > vmax) v = vmax;
    dv = vmax - vmin;
    
    if (v < (vmin + 0.25 * dv)) {
        c.set_red(0);
        c.set_green(static_cast<std::uint8_t> (4 * (v - vmin) / dv * 255));
    } else if (v < (vmin + 0.5 * dv)) {
        c.set_red(0);
        c.set_blue(static_cast<std::uint8_t> ((1 + 4 * (vmin + 0.25 * dv - v) / dv) * 255));
    } else if (v < (vmin + 0.75 * dv)) {
        c.set_red(static_cast<std::uint8_t> (4 * (v - vmin - 0.5 * dv) / dv * 255));
        c.set_blue(0);
    } else {
        c.set_green(static_cast<std::uint8_t> ((1 + 4 * (vmin + 0.75 * dv - v) / dv) * 255));
        c.set_blue(0);
    }
    
    return(c);
}

void save_png(Map m, std::string output_path) {
    image_rgba8 buf(m.width(), m.height());
    agg_renderer<image_rgba8> ren(m, buf);
    ren.apply();
    save_to_file(buf, output_path, "png");
}

void xml_filter() {
    std::string xml_path = "xml-filter.xml",
                output_path = "../output-mapnik/cpp-xml-filter.png";
    Map m(1300, 900);
    load_map(m, xml_path);
    m.zoom_all();
    save_png(m, output_path);
}

int main() {
    std::string plugins_dir = "/usr/lib64/mapnik/input";
    datasource_cache::instance().register_datasources(plugins_dir);
    
    //xml_filter();
    
    std::string xml_path = "cpp-shape-layers.xml",
                base_data_path = "../data/world_merc.shp",
                the_data_path = "../data/seis_cells.shp",
                output_path = "../output-mapnik/cpp-shape-layers.png";
    std::string srs_merc = "+init=epsg:3857";
    
    GDALAllRegister();
    
    GDALDataset * poDS = (GDALDataset *) GDALOpenEx(the_data_path.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
    OGRLayer * poLayer = poDS->GetLayer(0);
    std::vector<double> myData = std::vector<double>();
    for (auto& poFeature: poLayer)
        myData.push_back(poFeature->GetFieldAsDouble(0));
    
    int colorCount = 256;
    double  maxData = *std::max_element(std::begin(myData), std::end(myData)),
            minData = *std::min_element(std::begin(myData), std::end(myData));
    double rangeOneColor = (maxData - minData) / colorCount;
    std::cout << "max min " << maxData << " " << minData << " " << rangeOneColor << " data vector len " << myData.size() << std::endl;
    std::vector<double> stops = std::vector<double>(colorCount+1);
    std::vector<color> colors = std::vector<color>(colorCount);
    for (int iStop = 0; iStop < colorCount; iStop++) {
        stops[iStop] = minData + iStop * rangeOneColor;
        colors[iStop] = GetColor(stops[iStop], minData, maxData - rangeOneColor);
    }
    stops[0] -= 1;
    stops[colorCount] = maxData + 1;
    
    Map m(1300, 900);
    m.set_srs(srs_merc);
    m.set_background(color("white"));
    //load_map(m, xml_path);
    
    feature_type_style world_style;
    rule world_rule;
    polygon_symbolizer world_psym;
    world_psym.properties[keys::fill] = color("#c7e9b4");
    world_rule.append((std::move(world_psym)));
    world_style.add_rule(std::move(world_rule));
    m.insert_style("World Style", std::move(world_style));
    
    parameters wp;
    wp["type"] = "shape";
    wp["file"] = base_data_path;
    std::shared_ptr<datasource> world_ds = datasource_cache::instance().create(wp);
    layer wl("world_layer");
    wl.set_srs(srs_merc);
    wl.set_datasource(world_ds);
    wl.add_style("World Style");
    m.add_layer(wl);
    
    feature_type_style s;
    for (int iColor = 0; iColor < colorCount; iColor++) {
        rule r;
        std::ostringstream stringStream;
        stringStream << "[Data] >= " << stops[iColor] << " and [Data] < " << stops[iColor+1];
        expression_ptr f = parse_expression(stringStream.str());
        r.set_filter(f);
        polygon_symbolizer psym;
        psym.properties[keys::fill] = colors[iColor];
        //psym.properties[keys::fill_opacity] = static_cast<double>(iColor+1) / colorCount;
        r.append(std::move(psym));
        s.add_rule(std::move(r));
    }
    m.insert_style("data_style", std::move(s));
    
    parameters p;
    p["type"] = "shape";
    p["file"] = the_data_path;
    std::shared_ptr<datasource> ds = datasource_cache::instance().create(p);
    layer l("data_layer");
    l.set_srs(srs_merc);
    l.set_datasource(ds);
    l.add_style("data_style");
    m.add_layer(l);
    
    m.zoom_all();
    save_png(m, output_path);
}
