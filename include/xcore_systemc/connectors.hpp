#ifndef __xc_connectors_h__
#define __xc_connectors_h__

namespace xc {


  template <int W, class D>
  class connector : public sc_module
  {
  public:
    sc_inout_resolved to_io[W];
    sc_signal_resolved io_sig[W];
    sc_in< D > * p_in;
    sc_out< D > * p_out;
    SC_HAS_PROCESS(connector);

    connector(sc_module_name name, sc_in< D > *p) : sc_module(name)
    {
      for (int i=0;i<W;i++)
        to_io[i].bind(io_sig[i]);
      p_in = p;
      SC_THREAD(p_to_io);
      sensitive << (*p_in).value_changed();
    }

    connector(sc_module_name name, sc_out< D > *p) : sc_module(name)
    {
      for (int i=0;i<W;i++)
        to_io[i].bind(io_sig[i]);
      p_out = p;
      SC_THREAD(io_to_p);
      for (int i=0;i<W;i++)
        sensitive << to_io[i].value_changed();
    }

    connector(sc_module_name name, sc_inout< D > *p) : sc_module(name)
    {
      for (int i=0;i<W;i++)
        to_io[i].bind(io_sig[i]);

      p_out = p;
      p_in = p;
      SC_THREAD(p_to_io);
      sensitive << (*p_in)->value_changed();
      SC_THREAD(io_to_p);
      for (int i=0;i<W;i++)
        sensitive << to_io[i].value_changed();
    }

  private:
    void p_to_io(void) {
      while (true) {
        sc_uint<32> data;
        wait();
        data = (*p_in)->read();
        for (int i=0;i<W;i++) {
          if (data & 1) {
            to_io[i]->write(sc_logic(1));
          }
          else {
            to_io[i]->write(sc_logic(0));
          }
          data >>= 1;
        }
      }
    }

    void io_to_p(void) {
      while (true) {
        sc_uint<32> data = 0;
        wait();
        bool do_drive = false;
        for (int i=W-1;i>=0;i--) {
          sc_logic bit = (to_io[i]->read());
          data <<= 1;
          if (bit != 'z' && bit != 'x') {
            do_drive = true;
            data |= (int) bit.to_bool();
          }
        }
        if (do_drive) {
          (*p_out)->write(data);
        }
      }
    }

  };


}

#endif // __xc_connectors_h__
