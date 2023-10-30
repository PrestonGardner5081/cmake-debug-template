#include "Hello.h"

double Hello::Speaker::test_static_inst_var = 1.4;

int main() {
    Hello::Speaker s;
    s.test_static_inst_var = 1.3;
    s.sayHello();
}