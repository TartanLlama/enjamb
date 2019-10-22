#pragma once

#include <variant>
#include <vector>
#include <iostream>
#include <unordered_map>

namespace enjamb {
   enum class opcode {
      //Stack operations
      push, dup, swap, pop,

      //Mathematical operations
      add, sub, mul, div, mod,

      //Heap operations
      store, load,

      //Control flow
      label, call, jump, jz, jn, ret, exit,

      //I/O
      putc, putn, readc, readn
   };

   struct instruction {
      opcode opcode;
      std::variant<
         std::monostate, //most instructions have no operands
         int32_t,        //push has an int operand
         std::string     //label instructions have a string operand
      > operand;
   };
   using code = std::vector<instruction>;

   // "User percieved" characters are the basic units of text which
   // a user sees, rather than ASCII characters or Unicode code points
   int32_t count_user_perceived_characters(std::string const& str);

   opcode to_opcode(int32_t count);
   bool has_label_operand(opcode op);
   instruction generate_instruction(opcode op, std::istream& is);
   code read_code(std::istream& is);
   void dump_code(code const& code, std::ostream& os);
   int32_t execute_code(code const& code, std::ostream& os);
   std::unordered_map<std::string, std::size_t> find_label_positions(code const& code);
}