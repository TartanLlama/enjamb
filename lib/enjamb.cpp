#include <string>
#include <fstream>
#include <stack>
#include <array>
#include <unicode/unistr.h>
#include <unicode/brkiter.h>
#include "enjamb.hpp"

namespace enjamb {
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

   void dump_code(code const& code, std::ostream& os) {
      for (auto&& instr : code) {
         switch (instr.opcode) {
         case opcode::push: os << "push"; break;
         case opcode::dup: os << "dup"; break;
         case opcode::swap: os << "swap"; break;
         case opcode::pop: os << "pop"; break;
         case opcode::add: os << "add"; break;
         case opcode::sub: os << "sub"; break;
         case opcode::mul: os << "mul"; break;
         case opcode::div: os << "div"; break;
         case opcode::mod: os << "mod"; break;
         case opcode::store: os << "store"; break;
         case opcode::load: os << "load"; break;
         case opcode::label: os << "label"; break;
         case opcode::call: os << "call"; break;
         case opcode::jump: os << "jump"; break;
         case opcode::jz: os << "jz"; break;
         case opcode::jn: os << "jn"; break;
         case opcode::ret: os << "ret"; break;
         case opcode::exit: os << "exit"; break;
         case opcode::putc: os << "putc"; break;
         case opcode::putn: os << "putn"; break;
         case opcode::readc: os << "readc"; break;
         case opcode::readn: os << "readn"; break;
         }

         struct {
            void operator()(std::string const& op) {
               os << " \"" << op << "\"\n";
            }
            void operator()(int32_t op) {
               os << ' ' << op << '\n';
            }
            void operator()(std::monostate) {
               os << '\n';
            }
            std::ostream& os;
         } visitor{ os };

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

   int32_t execute_code(code const& code, std::ostream& os) {
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
            stack.pop();
            break;
         }

         case opcode::jn: {
            if (stack.top() < 0) {
               pc = labels[std::get<std::string>(instr.operand)];
            }
            stack.pop();
            break;
         }

         case opcode::ret: {
            pc = ret_stack.top();
            ret_stack.pop();
            break;
         }

         case opcode::exit: {
            if (stack.empty()) return 0;
            return stack.top();
         }

         case opcode::putc: {
            os << static_cast<char>(stack.top());
            stack.pop();
            break;
         }

         case opcode::putn: {
            os << stack.top();
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
}