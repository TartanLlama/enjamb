#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <variant>
#include <stack>
#include <array>
#include <unordered_map>
#include <iostream>
#include <unicode/unistr.h>
#include <unicode/brkiter.h>

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
int32_t count_user_perceived_characters(std::string const& str) {
   //Apparently this is how you count grapheme clusters in ICU
   //This API makes me cry
   auto unistr = icu::UnicodeString::fromUTF8(str);
   auto error_code = U_ZERO_ERROR;
   auto iter = icu::BreakIterator::createCharacterInstance(icu::Locale{}, error_code);
   iter->setText(unistr);
   int32_t count = 0;

   int32_t start = iter->first();
   for (int32_t end = iter->next();
      end != icu::BreakIterator::DONE;
      start = end, end = iter->next())
   {
      ++count;
   }

   delete iter;
   return count;
}

// This mapping was so that super-common operations
// (like push) didn't require super-short lines.
opcode to_opcode(int32_t count) {
   switch (count) {
   case 19: return opcode::push;
   case 20: return opcode::dup;
   case 21: return opcode::swap;
   case 22: return opcode::pop;

   case 14: return opcode::add;
   case 15: return opcode::sub;
   case 16: return opcode::mul;
   case 17: return opcode::div;
   case 18: return opcode::mod;

   case 12: return opcode::store;
   case 13: return opcode::load;

   case 5: return opcode::label;
   case 6: return opcode::call;
   case 7: return opcode::jump;
   case 8: return opcode::jz;
   case 9: return opcode::jn;
   case 10: return opcode::ret;
   case 11: return opcode::exit;

   case 1: return opcode::putc;
   case 2: return opcode::putn;
   case 3: return opcode::readc;
   case 4: return opcode::readn;

   default: throw std::runtime_error("Invalid opcode");
   }
}

bool has_label_operand(opcode op) {
   switch (op) {
   case opcode::label:
   case opcode::call:
   case opcode::jump:
   case opcode::jz:
   case opcode::jn:
      return true;
   default:
      return false;
   }
}

instruction generate_instruction(opcode op, std::istream& is) {
   if (has_label_operand(op)) {
      std::string operand = "";
      std::getline(is, operand);
      return { op, operand };
   }

   if (op == opcode::push) {
      std::string operand = "";
      std::getline(is, operand);
      auto count = count_user_perceived_characters(operand);
      return { op, count };
   }

   return { op };
}

code read_code(std::istream& is) {
   code code;
   std::string line;
   while (std::getline(is, line)) {
      auto count = count_user_perceived_characters(line);
      //empty lines are a no-op
      if (count != 0) {
         auto opcode = to_opcode(count);
         code.push_back(generate_instruction(opcode, is));
      }
   }

   return code;
}

void dump_code(code const& code) {
   for (auto&& instr : code) {
      switch (instr.opcode) {
      case opcode::push: std::cout << "push"; break;
      case opcode::dup: std::cout << "dup"; break;
      case opcode::swap: std::cout << "swap"; break;
      case opcode::pop: std::cout << "pop"; break;
      case opcode::add: std::cout << "add"; break;
      case opcode::sub: std::cout << "sub"; break;
      case opcode::mul: std::cout << "mul"; break;
      case opcode::div: std::cout << "div"; break;
      case opcode::mod: std::cout << "mod"; break;
      case opcode::store: std::cout << "store"; break;
      case opcode::load: std::cout << "load"; break;
      case opcode::label: std::cout << "label"; break;
      case opcode::call: std::cout << "call"; break;
      case opcode::jump: std::cout << "jump"; break;
      case opcode::jz: std::cout << "jz"; break;
      case opcode::jn: std::cout << "jn"; break;
      case opcode::ret: std::cout << "ret"; break;
      case opcode::exit: std::cout << "exit"; break;
      case opcode::putc: std::cout << "putc"; break;
      case opcode::putn: std::cout << "putn"; break;
      case opcode::readc: std::cout << "readc"; break;
      case opcode::readn: std::cout << "readn"; break;
      }

      struct {
         void operator()(std::string const& op) {
            std::cout << " \"" << op << "\"\n";
         }
         void operator()(int32_t op) {
            std::cout << ' ' << op << '\n';
         }
         void operator()(std::monostate) {
            std::cout << '\n';
         }
      } visitor;

      std::visit(visitor, instr.operand);
   }
}

std::unordered_map<std::string, std::size_t> find_label_positions(code const& code) {
   std::unordered_map<std::string, std::size_t> labels;
   std::size_t pc = 0;

   while (pc < code.size()) {
      if (code[pc].opcode == opcode::label) {
         labels[std::get<std::string>(code[pc].operand)] = pc;
      }
      ++pc;
   }

   return labels;
}

int32_t execute_code(code const& code) {
   std::stack<int32_t> stack;
   std::stack<std::size_t> ret_stack;
   std::array<int32_t, 4000> heap;
   std::unordered_map<std::string, std::size_t> labels = find_label_positions(code);
   std::size_t pc = 0;

   while (pc < code.size()) {
      auto const& instr = code[pc];
      switch (instr.opcode) {
      case opcode::push: stack.push(std::get<int>(instr.operand)); break;

      case opcode::dup: stack.push(stack.top()); break;

      case opcode::swap: {
         auto old_top = stack.top();
         stack.pop();
         std::swap(old_top, stack.top());
         stack.push(old_top);
         break;
      }

      case opcode::pop: stack.pop(); break;

      case opcode::add: {
         auto old_top = stack.top();
         stack.pop();
         stack.top() += old_top;
         break;
      }

      case opcode::sub: {
         auto old_top = stack.top();
         stack.pop();
         stack.top() -= old_top;
         break;
      }

      case opcode::mul: {
         auto old_top = stack.top();
         stack.pop();
         stack.top() *= old_top;
         break;
      }

      case opcode::div: {
         auto old_top = stack.top();
         stack.pop();
         stack.top() /= old_top;
         break;
      }

      case opcode::mod: {
         auto old_top = stack.top();
         stack.pop();
         stack.top() %= old_top;
         break;
      }

      case opcode::store: {
         auto loc = stack.top();
         stack.pop();
         heap[loc] = stack.top();
         break;
      }

      case opcode::load: {
         auto loc = stack.top();
         stack.pop();
         stack.push(heap[loc]);
         break;
      }

      case opcode::call: {
         ret_stack.push(pc);
         pc = labels[std::get<std::string>(instr.operand)];
         break;
      }

      case opcode::jump: {
         pc = labels[std::get<std::string>(instr.operand)];
         break;
      }

      case opcode::jz: {
         if (stack.top() == 0) {
            pc = labels[std::get<std::string>(instr.operand)];
         }
         break;
      }

      case opcode::jn: {
         if (stack.top() < 0) {
            pc = labels[std::get<std::string>(instr.operand)];
         }
         break;
      }

      case opcode::ret: {
         pc = ret_stack.top();
         ret_stack.pop();
         break;
      }

      case opcode::exit: return stack.top();

      case opcode::putc: {
         std::cout << static_cast<char>(stack.top());
         stack.pop();
         break;
      }

      case opcode::putn: {
         std::cout << stack.top();
         stack.pop();
         break;
      }

      case opcode::readc: {
         char c;
         std::cin >> c;
         stack.push(c);
         break;
      }

      case opcode::readn: {
         int32_t i;
         std::cin >> i;
         stack.push(i);
         break;
      }
      }

      ++pc;
   }

   return 0;
}

int main(int argc, const char** argv) {
   if (argc == 1) {
      std::cerr << "Usage: enjamb <source file> [--debug]\n";
      return 0;
   }

   auto file = std::ifstream(argv[1]);
   auto code = read_code(file);

   if (argc == 3 && argv[2] == std::string{ "--debug" }) dump_code(code);

   return execute_code(code);
}