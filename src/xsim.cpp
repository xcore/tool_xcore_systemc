#include "xcore_systemc/xsim.hpp"
#include "xsidevice.h"

static void report_status(XsiStatus status)
{
  switch (status)
    {
    case XSI_STATUS_OK:
    case XSI_STATUS_DONE:
      // OK, Any other error will exit the program
      return;
    case XSI_STATUS_TIMEOUT:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_TIMEOUT" << std::endl;
      break;
    case XSI_STATUS_INVALID_FILE:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_INVALID_FILE" << std::endl;
      break;
    case XSI_STATUS_INVALID_INSTANCE:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_INVALiD_INSTANCE" << std::endl;
      break;
    case XSI_STATUS_INVALID_CORE:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_INVALID_CORE" << std::endl;
      break;
    case XSI_STATUS_INVALID_PACKAGE:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_INVALID_PACKAGE" << std::endl;
      break;
    case XSI_STATUS_INVALID_PIN:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_INVALID_PIN" << std::endl;
      break;
    case XSI_STATUS_INVALID_PORT:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_INVALID_PORT" << std::endl;
      break;
    case XSI_STATUS_MEMORY_ERROR:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_MEMORY_ERROR" << std::endl;
      break;
    case XSI_STATUS_PSWITCH_ERROR:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_PSWITCH_ERROR" << std::endl;
      break;
    case XSI_STATUS_INVALID_ARGS:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_INVALD_ARGS" << std::endl;
      break;
    case XSI_STATUS_NULL_ARG:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_NULL_ARG" << std::endl;
      break;
    case XSI_STATUS_INCOMPATIBLE_VERSION:
      std::cerr << "Xcore Simulator Error: XSI_STATUS_INCOMPATIBLE_VERSION" << std::endl;
      break;
    }
  exit(1);
}

xc::xsim::xsim(sc_module_name name,
               std::string xn,
               std::string args) : xsystem(name, xn)
{
  // Create Simulator Instance
  XsiStatus status = xsi_create(&instance, args.c_str());
  report_status(status);

  // SystemC elaboration
  SC_THREAD(main);
  sensitive << clk_in.pos();




}


void xc::xsim::main()
{
  while (true) {
    XsiStatus status = xsi_clock(instance);
    report_status(status);
    wait();

    std::vector<package *>::iterator pkg;


    for (pkg = packages.begin();pkg != packages.end();pkg++) {
      std::map<std::string, sc_inout_resolved *>::iterator it;
      const char *pkg_name = ((*pkg)->id).c_str();
      for (it = (*pkg)->pins.begin(); it != (*pkg)->pins.end(); it++) {
        sc_inout_resolved *p = (it->second);

        const char *pin_name = (it->first).c_str();
        unsigned int is_driving;
        report_status(xsi_is_pin_driving(instance,
                                         pkg_name,
                                         pin_name,
                                         &is_driving));
        if (is_driving) {
          unsigned int value;
          report_status(xsi_sample_pin(instance,
                                       pkg_name,
                                       pin_name,
                                       &value));
          (*p)->write(sc_logic((int) value));
        }
        else {
          sc_logic bit = (*p)->read();
          if (bit != 'z' && bit != 'x') {
            report_status(xsi_drive_pin(instance,
                                        pkg_name,
                                        pin_name,
                                        bit.to_bool()));
          }
        }
      }

    }

  }
}
