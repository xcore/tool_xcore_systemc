#ifndef _xcore_systemc_xsim_h_
#define _xcore_systemc_xsim_h_
#include <string>
#include "systemc.h"
#include "xcore_systemc/xcore.hpp"

namespace xc {

  class xsim : public xsystem
  {
  public:
    sc_in<bool> clk_in;
    xsim(sc_module_name name, std::string xn, std::string args);

    SC_HAS_PROCESS(xsim);
  private:
    void *instance;
    void main();
  };
}

#endif// _xcore_systemc_h_
