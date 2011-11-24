SystemC/XCore integration repository
....................................

:Stable release:  unrelease

:Status:  draft

:Maintainer:  davelxmos

:Description: This repository contains code for simulating and
  modelling an XMOS device within a SystemC modelling environment.

  It is currently at a very early stage but allows you to plug the
  XMOS simulator (xsim) into a SystemC model and provide stimulus and
  measure output through SystemC ports.

Usage
=====

  This repository currently only works on Linux/Mac systems.

  You need to get the `SystemC 2.2.0 Library
  <http://www.systemc.org/members/download_files/check_file?agreement=systemc_2-2-0_07-03-14>`_,
  extract it into the same directory as the repo and build the
  library.

  You also need to obtain the `TinyXML library
  <http://sourceforge.net/projects/tinyxml/files/latest/download>`_
  and extract that into a ``tinyxml`` directory in the repo.

  Your workspace should end up looking like this::

     tool_xcore_systemc/
        build/
        src/
        include/
        examples/
        systemc-2.2.0/
        tinyxml/

  You can then do::

     cd examples/xsim/simple
     xcc hello.xc -o a.xe
     make all
     ./simple

   To run a simple example that runs a simulation within a SystemC simulation.

Support
=======

 This repository is not officially supported by XMOS Ltd.
