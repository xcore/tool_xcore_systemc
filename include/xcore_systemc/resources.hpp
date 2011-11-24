#ifndef __xcore_resources_h__
#define __xcore_resources_h__


namespace xc {


  typedef enum {
    COND_PINSEQ,
    COND_PINSNEQ,
    COND_NONE,
  } pred;

  template<int W>
  bool check_condition(pred cond, sc_uint<32> data_reg, sc_uint<W> data) {
    if (cond == COND_NONE)
      return true;

    if (cond == COND_PINSEQ) {
      return (data_reg == data);
    }
    if (cond == COND_PINSNEQ)
      return (data_reg != data);
  }


  class clock_base : public sc_module
  {
  public:
    sc_in< bool > clk_in;
    sc_in_resolved clk_in_resolved;
    sc_out< bool > clk_out;
    sc_signal< bool > clk_out_sig;
    bool use_resolved;

    SC_HAS_PROCESS(clock_base);

    clock_base(sc_module_name name) :
      sc_module(name),
      clk_in("clk_in"),
      clk_in_resolved("clk_in_resolved"),
      clk_out("clk_out")
    {
      use_resolved = false;
      clk_out.bind(clk_out_sig);
      SC_THREAD(do_clock);
      sensitive << clk_in.value_changed();
      sensitive << clk_in_resolved.value_changed();
    }

    void before_end_of_elaboration() {
      if (!clk_in_resolved.get_interface()) {
        sc_signal_resolved *sig = new sc_signal_resolved();
        clk_in_resolved.bind(*sig);
      }
      if (!clk_in.get_interface()) {
        sc_signal<bool> *sig = new sc_signal<bool>();
        clk_in.bind(*sig);
      }
    }

  private:
    void do_clock() {
      while (true) {
        wait();
        if (use_resolved) {
          sc_logic bit = clk_in_resolved->read();
          if (bit != 'z' && bit != 'x') {
            clk_out->write(bit.to_bool());
          }
        }
        else {
          clk_out->write(clk_in->read());
        }
      }
    }
  };



  class base_port_if : public sc_interface {
  public:
    virtual void setcondition(const pred condition) = 0;
    virtual void setdata(sc_uint<32> data) = 0;
    virtual void setpsc(int width) = 0;
    virtual void setclock(clock_base &clk) = 0;
    virtual void _driveclock(clock_base &clk) = 0;
    virtual sc_event *get_port_event() = 0;
    virtual bool is_ready() = 0;
    virtual void enable_events() = 0;
  protected:
    virtual std::vector<sc_inout_resolved *> *get_io_map() = 0;
    friend class xcore;
  };

  template <int W>
  class in_port_if : public base_port_if {
  public:
    virtual sc_uint<32> in(void) = 0;
    virtual unsigned int endin(void) = 0;
  };

  template <int W1, int W2>
  class in_buffered_port_if : public base_port_if {
  public:
    virtual sc_uint<32> in(void) = 0;
    virtual unsigned int endin(void) = 0;
  };

  template <int W>
  class out_port_if : public base_port_if {
  public:
    virtual void out(sc_uint<W>) = 0;
  };

  template <int W1, int W2 = W1>
  class out_buffered_port_if : public base_port_if {
  public:
    virtual void out(sc_uint<W1>) = 0;
  };

  template <int W>
  class port_if : public in_port_if<W>, public out_port_if<W> {
  };


  template <int WIDTH>
  class port_block : public port_if<WIDTH>, public in_buffered_port_if<WIDTH, 32>, public in_buffered_port_if<WIDTH, 16>, public in_buffered_port_if<WIDTH, 8>, public in_buffered_port_if<WIDTH, 4>, public in_buffered_port_if<WIDTH, 2>, public in_buffered_port_if<WIDTH, 1>, public out_buffered_port_if<WIDTH, 32>, public out_buffered_port_if<WIDTH, 16>, public out_buffered_port_if<WIDTH, 8>, public out_buffered_port_if<WIDTH, 4>, public out_buffered_port_if<WIDTH, 2>, public out_buffered_port_if<WIDTH, 1>, public sc_module
  {
  public:
    sc_in<bool> clk_in;

    void setcondition(const pred c)  { cond = c; data_ready = false;}
    void setdata(sc_uint<32> d) { data_reg = d; data_ready = false;}

    sc_uint<32> in(void)  {
      if (!data_ready){
        events_enabled = true;
        wait(port_event);
      }
      data_ready = false;
      events_enabled = false;
      return transfer_reg;
    };
    void out(sc_uint<WIDTH>)  {};

    unsigned int endin(void)  {
      unsigned int res = count;
      transfer_reg = shift_reg;
      shift_reg = 0;
      count = 0;
      data_ready = true;
      cond = COND_NONE;
      return res;
    };

    void setpsc(int x) {
      psc = x;
    }

    void setclock(clock_base &clk) {
      clk_in.bind(clk.clk_out_sig);
    }

    void _driveclock(clock_base &clk) {
      sc_signal_resolved *clk_in_sig = new sc_signal_resolved();
      io_map[0]->bind(*clk_in_sig);
      clk.clk_in_resolved.bind(*clk_in_sig);
      clk.use_resolved = true;
    }

    bool is_ready(void) {
      return data_ready;
    }

    void enable_events() {
      events_enabled = true;
    }

    SC_HAS_PROCESS(port_block);

    port_block(sc_module_name port_name,
               std::map <std::string,
               std::vector<sc_inout_resolved *> *> &ports,
               clock_base &default_clk0) :
      sc_module(port_name),
      clk_in("clk_in"),
      io_map(WIDTH)
    {
      default_clk = &default_clk0;
      count = 0;
      psc = WIDTH;
      cond = COND_NONE;
      data_reg = 0;
      events_enabled = false;
      SC_THREAD(main);
      sensitive << clk_in.pos();
      std::string pn((const char *) port_name);
      ports[pn] = &io_map;
    }


    void before_end_of_elaboration() {
      if (!clk_in.get_interface()) {
        setclock(*default_clk);
      }
    }

    sc_event *get_port_event() {
      return &port_event;
    }
  protected:
    int psc;
    std::vector<sc_inout_resolved *> * get_io_map(void) {
      return &io_map;
    }


  private:
    clock_base *default_clk;
    sc_uint<32> shift_reg;
    sc_uint<32> transfer_reg;
    sc_uint<32> data_reg;
    bool data_ready;
    bool events_enabled;
    int count;
    std::vector<sc_inout_resolved *> io_map;
    pred cond;
    sc_event port_event;


    sc_uint<WIDTH> read_ios() {
      sc_logic bit;
      sc_uint<WIDTH> data = 0;
      for (int i=WIDTH-1;i>=0;i--) {
        data <<= 1;
        bit = io_map[i]->read();
        if (bit != 'z' && bit != 'x') {
          data |= (int) bit.to_bool();
        }
      }
      return data;
    }


    void main(void) {
      while (true) {
        sc_uint<WIDTH> data;
        bool satisfies_condition;
        wait();
        data = read_ios();
        shift_reg = shift_reg >> WIDTH;
        shift_reg = shift_reg | (((sc_uint<32>) data) << (psc-WIDTH));
        count += WIDTH;
        satisfies_condition = check_condition(cond, data_reg, data);
        if (count == psc || (satisfies_condition && cond != COND_NONE)) {
          transfer_reg = shift_reg;
          shift_reg = 0;
          count = 0;
          if (satisfies_condition) {
            data_ready = true;
            cond = COND_NONE;
          }
          if (events_enabled && satisfies_condition) {
            port_event.notify();
          }
        }
      }
    }

  };



  template<int W>
  class port : public sc_port< port_if<W> >
  {
  };

  template<int W>
  class in_port : public sc_port< in_port_if<W> >
  {
  };

  template<int W>
  class out_port : public sc_port< out_port_if<W> >
  {
  };

  template<int W1, int W2>
  class in_buffered_port : public sc_port< in_buffered_port_if<W1, W2> >
  {
  public:
    void bind(in_buffered_port_if<W1, W2> &i) {
      sc_port< in_buffered_port_if<W1,W2> >::bind(i);
      (*this)->setpsc(W2);
    }
  };

  template<int W1, int W2>
  class out_buffered_port : public sc_port< out_buffered_port_if<W1, W2> >
  {
  public:
    void bind(out_buffered_port_if<W1, W2> &i) {
      sc_port< out_buffered_port_if<W1,W2> >::bind(i);
      (*this)->setpsc(W2);
    }
  };

  class clock : public clock_base
  {
  public:
    clock(sc_module_name name) : clock_base(name)
    {
    }

    void setsrc(port_block<1> &p) {
      p._driveclock(*this);
    }

    void setsrc(in_port<1> &p) {
      p->_driveclock(*this);
    }

  private:
  };

}

#endif // __xcore_resources_h__

