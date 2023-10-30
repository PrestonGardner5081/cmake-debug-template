#include "Hello.h"

using namespace Hello;
using namespace std;

namespace Hello {
  Speaker::Speaker(){
    test_int_inst_var = 321;
    test_string_inst_var = "I am a string";
  }

  void Speaker::sayHello() {
    cout << "Hello, world!\n";
  }
}
