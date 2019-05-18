#include <mapnik/map.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/image_util.hpp>

using namespace mapnik;

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
    std::string srs = "+init=epsg:3857";
    
    datasource_cache::instance().register_datasources(plugins_dir);
    
    xml_filter();
}
