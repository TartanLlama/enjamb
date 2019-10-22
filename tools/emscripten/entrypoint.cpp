#include <enjamb.hpp>
#include <sstream>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE
char* run_code(char const* source) {
   std::stringstream in;
   in << source;
   auto code = enjamb::read_code(in);

   std::stringstream out;
   enjamb::execute_code(code, out);
   auto out_str = out.str();
   auto ret = new char[out_str.size() + 1];
   memcpy(ret, out_str.data(), out_str.size());
   ret[out_str.size()] = 0;
   return ret;
}
