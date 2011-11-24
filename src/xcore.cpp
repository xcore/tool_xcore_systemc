#include "xcore_systemc/xcore.hpp"
#include "tinyxml.h"

xc::xcore::xcore(sc_module_name name,
                 std::string id0,
                 xsystem_type systypex) :
  sc_module(name),
  XS1_CLKBLK_0("XS1_CLKBLK_0"),
  XS1_CLKBLK_1("XS1_CLKBLK_1"),
  XS1_CLKBLK_2("XS1_CLKBLK_2"),
  XS1_CLKBLK_3("XS1_CLKBLK_3"),
  XS1_CLKBLK_4("XS1_CLKBLK_4"),
  XS1_CLKBLK_5("XS1_CLKBLK_5"),
  XS1_PORT_1A("XS1_PORT_1A", ports, XS1_CLKBLK_0),
  XS1_PORT_1B("XS1_PORT_1B", ports, XS1_CLKBLK_0),
  XS1_PORT_1C("XS1_PORT_1C", ports, XS1_CLKBLK_0),
  XS1_PORT_1D("XS1_PORT_1D", ports, XS1_CLKBLK_0),
  XS1_PORT_1E("XS1_PORT_1E", ports, XS1_CLKBLK_0),
  XS1_PORT_1F("XS1_PORT_1F", ports, XS1_CLKBLK_0),
  XS1_PORT_1G("XS1_PORT_1G", ports, XS1_CLKBLK_0),
  XS1_PORT_1H("XS1_PORT_1H", ports, XS1_CLKBLK_0),
  XS1_PORT_1I("XS1_PORT_1I", ports, XS1_CLKBLK_0),
  XS1_PORT_1J("XS1_PORT_1J", ports, XS1_CLKBLK_0),
  XS1_PORT_1K("XS1_PORT_1K", ports, XS1_CLKBLK_0),
  XS1_PORT_1L("XS1_PORT_1L", ports, XS1_CLKBLK_0),
  XS1_PORT_1M("XS1_PORT_1M", ports, XS1_CLKBLK_0),
  XS1_PORT_1N("XS1_PORT_1N", ports, XS1_CLKBLK_0),
  XS1_PORT_1O("XS1_PORT_1O", ports, XS1_CLKBLK_0),
  XS1_PORT_1P("XS1_PORT_1P", ports, XS1_CLKBLK_0),
  XS1_PORT_4A("XS1_PORT_4A", ports, XS1_CLKBLK_0),
  XS1_PORT_4B("XS1_PORT_4B", ports, XS1_CLKBLK_0),
  XS1_PORT_4C("XS1_PORT_4C", ports, XS1_CLKBLK_0),
  XS1_PORT_4D("XS1_PORT_4D", ports, XS1_CLKBLK_0),
  XS1_PORT_4E("XS1_PORT_4E", ports, XS1_CLKBLK_0),
  XS1_PORT_4F("XS1_PORT_4F", ports, XS1_CLKBLK_0),
  XS1_PORT_8A("XS1_PORT_8A", ports, XS1_CLKBLK_0),
  XS1_PORT_8B("XS1_PORT_8B", ports, XS1_CLKBLK_0),
  XS1_PORT_8C("XS1_PORT_8C", ports, XS1_CLKBLK_0),
  XS1_PORT_8D("XS1_PORT_8D", ports, XS1_CLKBLK_0),
  XS1_PORT_16A("XS1_PORT_16A", ports, XS1_CLKBLK_0),
  XS1_PORT_16B("XS1_PORT_16B", ports, XS1_CLKBLK_0),
  XS1_PORT_32A("XS1_PORT_32A", ports, XS1_CLKBLK_0)
{
  id = id0;
}

void xc::xcore::register_pin(sc_inout_resolved &p,
                             std::string port_name,
                             int offset)
{
  std::vector<sc_inout_resolved *> *io_map = ports[port_name];
  (*io_map)[offset] = &p;
}

xc::node::node(sc_module_name name,
               TiXmlElement *xml,
               xsystem_type systype) : sc_module(name)
{
  // Create cores
  TiXmlElement *core_xml = xml->FirstChildElement("Core")->ToElement();
  while (core_xml) {
    std::string coreId(core_xml->Attribute("Number"));
    std::string reference(core_xml->Attribute("Reference"));
    std::string name = "xcore_"+coreId;
    xc::xcore *n = new xc::xcore(name.c_str(), reference, systype);
    cores.push_back(n);
    core_xml = core_xml->NextSiblingElement("Core");
  }

}

xc::package::package(sc_module_name name,
                     TiXmlElement *package_xml,
                     std::string config_path,
                     xsystem_type systype) : sc_module(name)
{

  // Load the package file
  std::string pkg_name(package_xml->Attribute("Type"));
  std::string pkg_path = config_path + "/" + pkg_name + ".pkg";
  TiXmlDocument pkg_file(pkg_path.c_str());
  bool loadOkay = pkg_file.LoadFile();
  if (!loadOkay) {
    std::cerr << "Cannot load package file `" << pkg_path << "'" << std::endl;
    std::cerr << pkg_file.ErrorDesc() << std::endl;
    exit(1);
  }
  id = std::string(package_xml->Attribute("id"));


  // Create nodes
  TiXmlNode *nodes_xml =
    package_xml->FirstChild("Nodes")->ToElement();

  TiXmlElement *node_xml = nodes_xml->FirstChildElement("Node")->ToElement();
  while (node_xml) {
  std::string nodeId(node_xml->Attribute("Id"));


    std::string name = "node_"+nodeId;
    xc::node *n = new xc::node(name.c_str(), node_xml, systype);
    nodes.push_back(n);
    node_xml = node_xml->NextSiblingElement("Node");
  }

  // Create pins
  TiXmlNode *pins_xml = pkg_file.FirstChild("XPins")->FirstChild("Pins");

  assert(pins_xml);

  TiXmlElement *pin_xml = pins_xml->FirstChildElement("Pin")->ToElement();

  while (pin_xml) {
    std::string name(pin_xml->Attribute("name"));
    sc_inout_resolved *pin = new sc_inout_resolved(name.c_str());
    pins[name] = pin;

    TiXmlElement *port_xml = pin_xml->FirstChildElement("Port")->ToElement();

    while (port_xml) {
      int node = atoi(port_xml->Attribute("node"));
      int core = atoi(port_xml->Attribute("core"));
      xcore *xcore = nodes[node]->cores[core];
      xcore->register_pin(*pin,
                          std::string(port_xml->Attribute("name")),
                          atoi(port_xml->Attribute("bitNum")));
      assert(xcore);
      port_xml = port_xml->NextSiblingElement("Port");
    }

    pin_xml = pin_xml->NextSiblingElement("Pin");
  }

}

void xc::package::before_end_of_elaboration()
{
  std::map<std::string, sc_inout_resolved *>::iterator it;

  for (it = pins.begin(); it != pins.end(); it++) {
    sc_inout_resolved *p = (it->second);
    if (!p->get_interface()) {
      sc_signal_resolved *sig = new sc_signal_resolved();
      p->bind(*sig);
    }
  }
}


static void create_packages(std::vector<xc::package *> &vec,
                            TiXmlNode *xml,
                            std::string config_path,
                            xc::xsystem_type systype)
{
  TiXmlElement *package_xml;
  assert(xml);
  package_xml = xml->FirstChildElement()->ToElement();
  while (package_xml) {
    std::string pkgId(package_xml->Attribute("id"));
    std::string name = "pkg_"+pkgId;
    xc::package *pkg = new xc::package(name.c_str(),
                                       package_xml,
                                       config_path,
                                       systype);
    vec.push_back(pkg);
    package_xml = package_xml->NextSiblingElement();
  }
}


std::vector<xc::xcore *> xc::xsystem::get_core_array(std::string expr) {
  std::vector<xc::xcore *> vec;

  std::vector<xc::package *>::iterator pkg;
  std::vector<xc::node *>::iterator n;
  std::vector<xc::xcore *>::iterator c;

  for (pkg = packages.begin();pkg != packages.end();pkg++)
    for (n = (*pkg)->nodes.begin();n != (*pkg)->nodes.end();n++)
      for (c = (*n)->cores.begin();c != (*n)->cores.end();n++)
        {
          vec.push_back(*c);
        }
  return vec;
}

xc::xcore * xc::xsystem::get_core(std::string expr)
{
  std::vector<xc::package *>::iterator pkg;
  std::vector<xc::node *>::iterator n;
  std::vector<xc::xcore *>::iterator c;

  for (pkg = packages.begin();pkg != packages.end();pkg++)
    for (n = (*pkg)->nodes.begin();n != (*pkg)->nodes.end();n++)
      for (c = (*n)->cores.begin();c != (*n)->cores.end();n++)
        {
          if ( (*c)->id == expr )
            return *c;
        }


  return NULL;
}

void xc::xsystem::init(std::string xn, xsystem_type systype)
{
  char *XCC_DEVICE_PATH = getenv("XCC_DEVICE_PATH");
  assert(XCC_DEVICE_PATH);
  std::string config_path(XCC_DEVICE_PATH);
  xn = xn + ".xn";
  std::string xn_path = config_path + "/" + xn;
  TiXmlDocument xn_file(xn_path.c_str());
  bool loadOkay = xn_file.LoadFile();
  if (!loadOkay) {
    std::cerr << "Cannot load XN file `" << xn_path << "'" << std::endl;
    std::cerr << xn_file.ErrorDesc() << std::endl;
    exit(1);
  }

  TiXmlNode *packages_xml =
    xn_file.FirstChild("Network")->ToElement()->FirstChild("Packages");

  create_packages(packages, packages_xml, config_path, systype);
}

xc::xsystem::xsystem(sc_module_name name, std::string xn, xsystem_type systype) : sc_module(name)
{
  init(xn, systype);
}

xc::xsystem::xsystem(sc_module_name name, std::string xn) : sc_module(name)
{
  init(xn, XSYSTEM_SYSC);
}
