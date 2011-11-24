#include "systemc.h"
#include "xcore_systemc.h"

class counter: public sc_module
{
public:
  sc_out< sc_uint<4> > count_out;

  SC_HAS_PROCESS(counter);

  counter(sc_module_name name) : sc_module(name)
  {
    SC_THREAD(main);
  }

private:
  void main() {
    int i = 0;
    while (1) {
      wait(sc_time(5000,SC_NS));
      count_out->write(i);
      i++;
    }
  }

};

class test_xcore_application : public sc_module
{
public:
  sc_clock xcore_clk;
  sc_out< sc_uint<1> > p_out;
  sc_in< sc_uint<4> > count_in;

  test_xcore_application(sc_module_name name, std::string xsim_args) :
    sc_module(name),
    p_out("p_out"),
    count_in("count_in"),
    xsi("xsim", "XC-1A", xsim_args),
    xcore_clk("xcore_clk", sc_time(2.5, SC_NS))
  {
    xsi.clk_in.bind(xcore_clk);
    xsi.get_core("stdcore[0]")->implement_port(p_out, "XS1_PORT_1A");
    xsi.get_core("stdcore[0]")->implement_port(count_in, "XS1_PORT_4A");
  }

private:
  xc::xsim xsi;
};

class testbench: public sc_module
{
public:
  test_xcore_application test_app;
  counter test_counter;
  sc_signal < sc_uint<4> > count_wire;

  SC_HAS_PROCESS(testbench);

  testbench(sc_module_name name, std::string xsim_args):
    sc_module(name),
    test_counter("test_counter"),
    test_app("test_app", xsim_args)
  {
    test_app.count_in.bind(count_wire);
    test_counter.count_out.bind(count_wire);
  }

  void start() {
    test_app.xcore_clk.start(sc_time(0,SC_NS));
  }
private:

};


int sc_main(int argc, char *argv[])
{
  testbench bench("testbench","a.xe");
  sc_trace_file *wf = sc_create_vcd_trace_file("mii");
  sc_signal< sc_uint<1> > p_out_sig;
  bench.test_app.p_out.bind(p_out_sig);
  sc_trace(wf, bench.test_app.p_out, "p_out");

  bench.start();

  sc_start(sc_time(50,SC_US));
  sc_close_vcd_trace_file(wf);
}
