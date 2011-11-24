#ifndef __xcore_h__
#define __xcore_h__
#include "systemc.h"
#include "stdarg.h"
#include <map>
#include <string>
#include "xcore_systemc/connectors.hpp"
#include "xcore_systemc/resources.hpp"
#include "tinyxml.h"

namespace xc {

  typedef enum xsystem_type {
    XSYSTEM_XSIM,
    XSYSTEM_SYSC
  } xsystem_type;


  class xcore : public sc_module
  {
  private:
    std::map<std::string, std::vector<sc_inout_resolved *> *> ports;

  public:
    xcore(sc_module_name name, std::string id, xsystem_type systype);

    port_block<1> XS1_PORT_1A;
    port_block<1> XS1_PORT_1B;
    port_block<1> XS1_PORT_1C;
    port_block<1> XS1_PORT_1D;
    port_block<1> XS1_PORT_1E;
    port_block<1> XS1_PORT_1F;
    port_block<1> XS1_PORT_1G;
    port_block<1> XS1_PORT_1H;
    port_block<1> XS1_PORT_1I;
    port_block<1> XS1_PORT_1J;
    port_block<1> XS1_PORT_1K;
    port_block<1> XS1_PORT_1L;
    port_block<1> XS1_PORT_1M;
    port_block<1> XS1_PORT_1N;
    port_block<1> XS1_PORT_1O;
    port_block<1> XS1_PORT_1P;
    port_block<4> XS1_PORT_4A;
    port_block<4> XS1_PORT_4B;
    port_block<4> XS1_PORT_4C;
    port_block<4> XS1_PORT_4D;
    port_block<4> XS1_PORT_4E;
    port_block<4> XS1_PORT_4F;
    port_block<8> XS1_PORT_8A;
    port_block<8> XS1_PORT_8B;
    port_block<8> XS1_PORT_8C;
    port_block<8> XS1_PORT_8D;
    port_block<16> XS1_PORT_16A;
    port_block<16> XS1_PORT_16B;
    port_block<32> XS1_PORT_32A;

    clock XS1_CLKBLK_0;
    clock XS1_CLKBLK_1;
    clock XS1_CLKBLK_2;
    clock XS1_CLKBLK_3;
    clock XS1_CLKBLK_4;
    clock XS1_CLKBLK_5;


    template<int W, template <class> class SC_PORT>
    void implement_port(SC_PORT< sc_uint<W>  > &p, std::string name)
    {
      sc_module_name module_name((name + "_connector").c_str());
      connector<W, sc_uint<W> > *c =
        new connector<W, sc_uint<W> >(module_name, &p);
      std::vector<sc_inout_resolved *> *map = ports[name];
      for (int i=0;i<W;i++) {
        sc_inout_resolved *p = (*map)[i];
        (*p).bind(c->io_sig[i]);
      }
    }

    template<template <class> class SC_PORT>
    void implement_port(SC_PORT< bool  > &p, std::string name)
    {
      sc_module_name module_name((name + "_connector").c_str());
      connector<1, bool > *c =
        new connector<1, bool >(module_name, &p);
      std::vector<sc_inout_resolved *> *map = ports[name];
      for (int i=0;i<1;i++) {
        sc_inout_resolved *p = (*map)[i];
        (*p).bind(c->io_sig[i]);
      }
    }   



  private:
    void register_pin(sc_inout_resolved &p, std::string port_name, int offset);
    std::string id;
    friend class package;
    friend class xsystem;

  };

  class node : public sc_module
  {
  public:
    node(sc_module_name name, TiXmlElement *xml, xsystem_type systype);
  private:
    std::vector<xcore *> cores;
    friend class package;
    friend class xsystem;
  };


  class package : public sc_module
  {
  public:
    package(sc_module_name name, TiXmlElement *xml,
            std::string config_path, xsystem_type systype);
    void before_end_of_elaboration();
    std::map<std::string, sc_inout_resolved *> pins;
    std::string id;
  private:
    std::vector<node *> nodes;
    friend class xsystem;
  };



  class xsystem : public sc_module
  {
  public:
    xsystem(sc_module_name name, std::string xn);
    xsystem(sc_module_name name, std::string xn, xsystem_type systype);


    xcore *get_core(std::string expr);
    std::vector<xcore *> get_core_array(std::string name);
    std::vector<package *> packages;

  protected:
  private:
    void init(std::string xn, xsystem_type systype);

  };











}
#endif // __xcore_h__

