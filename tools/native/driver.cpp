#include "enjamb.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> file_to_lines(std::ifstream& file) {
   std::vector<std::string> lines;
   file.clear();
   file.seekg(0);
   std::string line;
   while (std::getline(file, line)) {
      lines.push_back(line);
   }
}

int main(int argc, const char** argv) {
   if (argc == 1) {
      std::cerr << "Usage: enjamb <source file> [--debug]\n";
      return 0;
   }

   auto file = std::ifstream(argv[1]);
   auto code = enjamb::read_code(file);

   if (argc == 3 && argv[2] == std::string{ "--debug" }) enjamb::dump_code(code, std::cout);

   try {
      return enjamb::execute_code(code, std::cout);
   }
   catch (enjamb::error const& err) {
      auto lines = file_to_lines(file);
      std::cerr << err.what() << " at line " << err.line_number << ":\n";
      std::cerr << lines[err.line_number - 1];
      return -1;
   }
}