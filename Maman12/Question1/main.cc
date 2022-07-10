#include <iostream>
#include <string>
#include "credit.h"

int main(int argc, char const *argv[]) {
    std::string entitled_str = is_entitled_for_promotional_gift(0) ?  "yes" : "no";
    std::cout << entitled_str << std::endl;

    entitled_str = is_entitled_for_promotional_gift_fixed(0) ?  "yes" : "no";
    std::cout << entitled_str << std::endl;
}
