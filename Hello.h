#pragma once

#include <stdio.h>
#include <iostream>
#include <string>

namespace Hello {
  class Speaker {
    
    public:
      Speaker();
      void sayHello();
      std::string test_string_inst_var;
      int test_int_inst_var;
      static double test_static_inst_var;

  };
}
