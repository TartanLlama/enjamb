#include "enjamb.hpp"
#include <iostream>
#include <fstream>

int main(int argc, const char** argv) {
   if (argc == 1) {
      std::cerr << "Usage: enjamb <source file> [--debug]\n";
      return 0;
   }

   auto file = std::ifstream(argv[1]);
   auto code = enjamb::read_code(file);

   if (argc == 3 && argv[2] == std::string{ "--debug" }) enjamb::dump_code(code, std::cout);

   return enjamb::execute_code(code, std::cout);
}