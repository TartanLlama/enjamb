#pragma once

#include <variant>
#include <vector>
#include <iostream>
#include <exception>
#include <unordered_map>

namespace enjamb {
   constexpr std::size_t heap_size = 4096;

   struct error : std::runtime_error {
      error(std::string const& what, std::size_t line_number) : 
         std::runtime_error(what), line_number(line_number) {

      }
      std::size_t line_number;
   };

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
      opcode op;
      std::variant<
         std::monostate, //most instructions have no operands
         int32_t,        //push has an int operand
         std::string     //label instructions have a string operand
      > operand;
      std::size_t line_number;
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