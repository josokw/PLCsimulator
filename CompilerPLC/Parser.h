#ifndef PARSER_H
#define PARSER_H

//#define BOOST_SPIRIT_DEBUG

#include "Compiler.h"
#include "FunctionConfig.h"
#include "Memory.h"
#include "MemoryConfig.h"
#include "Processor.h"
#include "ProcessorConfig.h"

#include <iostream>
#include <functional>
#include <queue>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_confix.hpp>
#include <boost/spirit/include/classic_config.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_if.hpp>

using namespace std::placeholders;
namespace bsp = boost::spirit::classic;

/// This structure contains all semantic actions for the PLC script parser.
struct VirtualPLCBuilder
{
  std::vector<Compiler::message_t>& messages;

  Memory memory;
  const Processor processor;
  const ProcessorConfig processorConfig;

  int tmpvarIndex;
  std::string qualifier;
  std::string type;
  std::string identifier;
  std::string lhs;
  std::string rhs;
  std::string value;
  bool isFunction;
  int timerIndex;
  int timerValue;
  int counterIndex;
  int counterValue;
  char XY;
  char index;
  std::vector<std::string> parList;
  std::queue<std::string> exprOutputRPN;
  std::stack<std::string> exprOperatorStack;
  std::queue<std::string> coils;

  using symbolData_t = struct symbolData
  {
    symbolData(const std::string& q, const std::string& t, int d)
      :  qualifier(q)
      ,  type(t)
      ,  data(d)
    {}
    std::string qualifier;
    std::string type;
    int data;
  };

  bsp::symbols<symbolData_t> symbolsData;
  std::function<void(const char*, const char*)> info;
  std::function<void(const char*, const char*)> declaration;
  std::function<void(const char*, const char*)> timerDeclaration;
  std::function<void(const char*, const char*)> counterDeclaration;
  std::function<void(const char*, const char*)> mapping;
  std::function<void(const char*, const char*)> factor;
  std::function<void(const char*, const char*)> expression;
  std::function<void(const char*, const char*)> exprAddToOutput;
  std::function<void(const char*, const char*)> exprAddToOperatorStack;
  std::function<void(const char*, const char*)> rparen;
  std::function<void(const char*, const char*)> compileRPN;
  std::function<void(const char*, const char*)> negationContact;
  std::function<void(const char*, const char*)> addCoilID;
  std::function<void(const char*, const char*)> normalCoil;
  std::function<void(const char*, const char*)> negationCoil;
  std::function<void(const char*, const char*)> setCoil;
  std::function<void(const char*, const char*)> resetCoil;
  std::function<void(const char*, const char*)> ladderDiagram;
  std::function<void(const char*, const char*)> terminate;

  VirtualPLCBuilder* self() { return this; }

  VirtualPLCBuilder(std::vector<Compiler::message_t>& messages)
    :  messages(messages)
    ,  memory()
    ,  processor(&memory, 0)
    ,  tmpvarIndex(0)
    ,  isFunction(false)
    ,  symbolsData()
    ,  info(std::bind(&VirtualPLCBuilder::do_info, self(), _1, _2))
    ,  declaration(std::bind(&VirtualPLCBuilder::do_declaration,
                             self(), _1, _2))
    ,  timerDeclaration(std::bind(&VirtualPLCBuilder::do_timerDeclaration,
                                  self(), _1, _2))
    ,  counterDeclaration(std::bind(&VirtualPLCBuilder::do_counterDeclaration,
                                    self(), _1, _2))
    ,  mapping(std::bind(&VirtualPLCBuilder::do_mapping, self(), _1, _2))
    ,  factor(std::bind(&VirtualPLCBuilder::do_factor, self(), _1, _2))
    ,  expression(std::bind(&VirtualPLCBuilder::do_expression, self(), _1, _2))
    ,  exprAddToOutput(std::bind(&VirtualPLCBuilder::do_exprAddToOutput,
                                 self(), _1, _2))
    ,  exprAddToOperatorStack(std::bind(
                                &VirtualPLCBuilder::do_exprAddToOperatorStack,
                                self(), _1, _2))
    ,  rparen(std::bind(&VirtualPLCBuilder::do_rparen, self(), _1, _2))
    ,  compileRPN(std::bind(&VirtualPLCBuilder::do_compileRPN, self(), _1, _2))
    ,  negationContact(std::bind(&VirtualPLCBuilder::do_negationContact,
                                 self(), _1, _2))
    ,  addCoilID(std::bind(&VirtualPLCBuilder::do_addCoilID, self(), _1, _2))
    ,  normalCoil(std::bind(&VirtualPLCBuilder::do_normalCoil, self(), _1, _2))
    ,  negationCoil(std::bind(&VirtualPLCBuilder::do_negationCoil,
                              self(), _1, _2))
    ,  setCoil(std::bind(&VirtualPLCBuilder::do_setCoil, self(), _1, _2))
    ,  resetCoil(std::bind(&VirtualPLCBuilder::do_resetCoil, self(), _1, _2))
    ,  ladderDiagram(std::bind(&VirtualPLCBuilder::do_ladderDiagram,
                               self(), _1, _2))
    ,  terminate(std::bind(&VirtualPLCBuilder::do_terminate, self(), _1, _2))
  {
    memory.addNextCODE(processorConfig.getIC("LATCH"));
    memory.addNextCODE(processorConfig.getIC("CCNTS"));
    // LOGICAL OPERATORS symbolData: qualifier, result type, operator priority
    bsp::add(symbolsData, "NOT", symbolData("UNARY_OPERATOR", "BOOL", 1));
    bsp::add(symbolsData, "AND", symbolData("BINARY_OPERATOR", "BOOL", 2));
    bsp::add(symbolsData, "OR",  symbolData("BINARY_OPERATOR", "BOOL", 3));
    bsp::add(symbolsData, "XOR", symbolData("BINARY_OPERATOR", "BOOL", 4));
    // LPAREN symbolData: qualifier, result type, operator priority
    bsp::add(symbolsData, "(", symbolData("LPAREN", "", 0));
    // COMPARATORS symbolData: qualifier, result type, number of arguments
    bsp::add(symbolsData, "GT", symbolData("COMPARATOR", "BOOL", 2));
    bsp::add(symbolsData, "GE", symbolData("COMPARATOR", "BOOL", 2));
    bsp::add(symbolsData, "EQ", symbolData("COMPARATOR", "BOOL", 2));
    bsp::add(symbolsData, "LE", symbolData("COMPARATOR", "BOOL", 2));
    bsp::add(symbolsData, "LT", symbolData("COMPARATOR", "BOOL", 2));
    bsp::add(symbolsData, "NE", symbolData("COMPARATOR", "BOOL", 2));
  }

  void showExprOutputRPN()
  {
    auto RPN(exprOutputRPN);
    std::clog << "*** [Parser] expression RPN: ";
    while (!RPN.empty()) {
      std::string s(RPN.front());
      std::clog << s << " ";
      RPN.pop();
    }
    std::clog << std::endl;
  }

  void do_info(const char* begin, const char* end)
  {
    std::clog << "~~~~~~~~~ Parser info: '"
              << std::string(begin, end) << std::endl;
  }

  void do_declaration(const char* begin, const char* /* end */)
  {
    if (!qualifier.empty())
    {
      if (qualifier != "CONST")
      {
        messages.push_back(Compiler::message_t('E', 0, begin,
                                               "Unknown qualifier '"
                                               + qualifier + "', CONST?"));
      }
    }

    if (type != "INT" && type != "BOOL")
    {
      messages.push_back(Compiler::message_t('E', 0, begin,
                                             "Unknown type '"
                                             + type + "', INT? BOOL?"));
    }

    int iValue = 0;
    auto pLhs = find(symbolsData, lhs.c_str());
    if (pLhs) {
      messages.push_back(Compiler::message_t('E', 0, begin,
                                             "'" + lhs + "' already exists!"));
    }
    else
    {
      // Initialisation by a const value
      if (rhs.empty())
      {
        if (value == "true" && type == "BOOL")
        {
          iValue = 1;
        }
        else
        {
          if (value == "false" && type == "BOOL")
          {
            iValue = 0;
          }
          else
          {
            if (type == "INT")
            {
              iValue = atoi(value.c_str());
            }
            else
            {
              messages.push_back(Compiler::message_t('E', 0, begin,
                                                     "Type '" + type
                                                     + "' does no match with '"
                                                     + value.c_str() + "'"));
            }
          }
        }
      }
      else
      {
        // Intialisation by a const var value (in rhs)
        auto pRhs = find(symbolsData, rhs.c_str());
        if (nullptr == pRhs) {
          messages.push_back(Compiler::message_t('E', 0, begin,
                                                 "Constant '" + rhs
                                                 + "' does not exists!"));
          iValue = 999999;
        }
        else
        {
          if (type != (pRhs->type))
          {
            messages.push_back(Compiler::message_t('E', 0, begin,
                                                   "Type mismatch '" + lhs
                                                   + "' and '" + rhs + "'"));
          }
          iValue = memory[pRhs->data].integer;
        }
      }

      // Compile
      if (qualifier.empty())
      {
        bsp::add(symbolsData, lhs.c_str(),
                                    symbolData(qualifier, type,
                                               memory.getNextVARaddress()));
        if (type == "INT") memory.addNextVAR(iValue);
        if (type == "BOOL") iValue == 1
            ? memory.addNextVAR(true) : memory.addNextVAR(false);
      }
      else
      {
        bsp::add(symbolsData, lhs.c_str(),
                                    symbolData(qualifier,
                                               type,
                                               memory.getNextCONSTaddress()));
        if (type == "INT") memory.addNextCONST(iValue);
        if (type == "BOOL") iValue == 1
            ? memory.addNextCONST(true) : memory.addNextCONST(false);
      }
    }
  }

  void do_timerDeclaration(const char* begin, const char* end)
  {
    int timerOffset = MemoryConfig::TIMERS;

    if (timerIndex > (MemoryConfig::nTIMERS - 1))
    {
      std::string tI(1, '0'+ timerIndex);
      messages.push_back(Compiler::message_t('E', 0, begin, "Timer" + tI
                                             + "C is not available"));
    }
    else
    {
      if (parList.size() == 1)
      {
        std::clog << "[Parser] " << std::string(begin, end) << " " << timerIndex
                  << " " << parList[0] << " " << timerValue << std::endl;
        auto p0 = find(symbolsData, parList[0].c_str());
        if (nullptr == p0) {
          messages.push_back(Compiler::message_t('E', 0, begin,
                                                 "Parameter '" + parList[0]
                             + "' does not exists!"));
        }
        else
        {
          memory[timerOffset + timerIndex * MemoryConfig::TIMER_SIZE
              + FunctionConfig::TEnable].actual = memory[p0->data].actual;
          memory[timerOffset + timerIndex * MemoryConfig::TIMER_SIZE
              + FunctionConfig::TValue].integer = timerValue;
        }
      }
      else
      {
        std::clog << "[Parser] " << std::string(begin, end) << "  " << timerIndex
                  << " " << parList[0] << " " << parList[1] << std::endl;
        auto p0 = find(symbolsData, parList[0].c_str());
        if (nullptr == p0)
        {
          messages.push_back(Compiler::message_t('E', 0, begin,
                                                 "Parameter '" + parList[0]
                             + "' does not exists!"));
        }
        auto p1 = find(symbolsData, parList[1].c_str());
        if (nullptr == p1) {
          messages.push_back(Compiler::message_t('E', 0, begin,
                                                 "Parameter '" + parList[1]
                             + "' does not exists!"));
        }
        // Compile
        if (p0 != nullptr && p1 != nullptr) {
          memory[timerOffset + timerIndex * MemoryConfig::TIMER_SIZE
              + FunctionConfig::TEnable] = memory[p0->data];
          memory[timerOffset + timerIndex * MemoryConfig::TIMER_SIZE
              + FunctionConfig::TValue] = memory[p1->data];
          memory[timerOffset + timerIndex * MemoryConfig::TIMER_SIZE
              + FunctionConfig::TC].actual = true;
          memory[timerOffset + timerIndex * MemoryConfig::TIMER_SIZE
              + FunctionConfig::TC].previous = true;
          memory.addNextCODE(processorConfig.getIC("CPY"));
          memory.addNextCODE(p0->data);
          memory.addNextCODE(timerOffset
                             + timerIndex * MemoryConfig::TIMER_SIZE
                             + FunctionConfig::TEnable);
          memory.dump(std::cerr);
        }
      }
      std::string timerName("TIMER" + std::to_string(timerIndex));
      {
        std::string timerNameSub(timerName + "N");
        bsp::add(symbolsData, timerNameSub.c_str(),
                                    symbolData("", "INT",
                                               timerOffset
                                               + timerIndex * MemoryConfig::TIMER_SIZE
                                               + FunctionConfig::TN));
      }
      {
        std::string timerNameSub(timerName + "S");
        bsp::add(symbolsData, timerNameSub.c_str(),
                                    symbolData("", "BOOL",
                                               timerOffset
                                               + timerIndex * MemoryConfig::TIMER_SIZE
                                               + FunctionConfig::TS));
      }
      {
        std::string timerNameSub(timerName + "C");
        bsp::add(symbolsData, timerNameSub.c_str(),
                                    symbolData("", "BOOL",
                                               timerOffset
                                               + timerIndex * MemoryConfig::TIMER_SIZE
                                               + FunctionConfig::TC));
      }
    }
    parList.clear();
  }

  void do_counterDeclaration(const char* begin, const char* end)
  {
    int counterOffset = MemoryConfig::COUNTERS;

    if (counterIndex > (MemoryConfig::nCOUNTERS - 1)) {
      std::string cI(1, '0'+ counterIndex);
      messages.push_back(Compiler::message_t('E', 0, begin,
                                             "Counter" + cI
                                             + "C is not available"));
    }
    else
    {
      if (parList.size() == 1) {
        std::clog << "[Parser] " << std::string(begin, end) << " "
                  << counterIndex << " " << parList[0] << " " << counterValue
                  << std::endl;
        auto p0 = find(symbolsData, parList[0].c_str());
        if (nullptr == p0) {
          messages.push_back(Compiler::message_t('E', 0, begin,
                                                 "Parameter '"
                                                 + parList[0]
                             + "' does not exists!"));
        }
        else
        {
          memory[counterOffset + counterIndex * MemoryConfig::COUNTER_SIZE
              + FunctionConfig::CEnable].actual = memory[p0->data].actual;
          memory[counterOffset + counterIndex * MemoryConfig::COUNTER_SIZE
              + FunctionConfig::CValue].integer = counterValue;
        }
      }
      else
      {
        std::clog << "[Parser] " << std::string(begin, end) << "  "
                  << counterIndex << " " << parList[0] << " " << parList[1]
                  << std::endl;
        auto p0 = find(symbolsData, parList[0].c_str());
        if (nullptr == p0) {
          messages.push_back(Compiler::message_t('E', 0, begin,
                                                 "Parameter '" + parList[0]
                             + "' does not exists!"));
        }
        auto p1 = find(symbolsData, parList[1].c_str());
        if (nullptr == p1) {
          messages.push_back(Compiler::message_t('E', 0, begin,
                                                 "Parameter '" + parList[1]
                             + "' does not exists!"));
        }
        // Compile
        if (p0 != nullptr && p1 != nullptr) {
          memory[counterOffset + counterIndex * MemoryConfig::COUNTER_SIZE
              + FunctionConfig::CEnable] = memory[p0->data];
          memory[counterOffset + counterIndex * MemoryConfig::COUNTER_SIZE
              + FunctionConfig::CValue] = memory[p1->data];
          memory[counterOffset + counterIndex * MemoryConfig::COUNTER_SIZE
              + FunctionConfig::CC].actual = true;
          memory[counterOffset + counterIndex * MemoryConfig::COUNTER_SIZE
              + FunctionConfig::CC].previous = true;
          memory.addNextCODE(processorConfig.getIC("CPY"));
          memory.addNextCODE(p0->data);
          memory.addNextCODE(counterOffset
                             + counterIndex * MemoryConfig::COUNTER_SIZE
                             + FunctionConfig::CEnable);
          memory.dump(std::cerr);
        }
      }
      std::string counterName("COUNTER" + std::to_string(counterIndex));
      {
        std::string counterNameSub(counterName + "N");
        bsp::add(symbolsData, counterNameSub.c_str(),
                                    symbolData("", "INT",
                                               counterOffset
                                               + counterIndex * MemoryConfig::COUNTER_SIZE
                                               + FunctionConfig::CN));
      }
      {
        std::string counterNameSub(counterName + "S");
        bsp::add(symbolsData, counterNameSub.c_str(),
                                    symbolData("", "BOOL",
                                               counterOffset
                                               + counterIndex * MemoryConfig::COUNTER_SIZE
                                               + FunctionConfig::CS));
      }
      {
        std::string counterNameSub(counterName + "C");
        bsp::add(symbolsData, counterNameSub.c_str(),
                                    symbolData("", "BOOL",
                                               counterOffset
                                               + counterIndex * MemoryConfig::COUNTER_SIZE
                                               + FunctionConfig::CC));
      }
    }
    parList.clear();
  }

  void do_mapping(const char* begin, const char* /* end */)
  {
    std::clog << "*** [Parser] mapping " << XY << index << " = " << identifier
              << std::endl;
    auto p = find(symbolsData, identifier.c_str());
    if (nullptr == p) {
      messages.push_back(Compiler::message_t('E', 0, begin,
                                             "'" + identifier
                                             + "' does not exists!"));
    }
    else
    {
      // index is a char!
      int indx = index - '0';
      switch (XY)
      {
        case 'X':
          memory.addNextCODE(processorConfig.getIC("READX"));
          if (indx > 0)
          {}
          break;
        case 'Y':
          memory.addNextCODE(processorConfig.getIC("SETY"));
          if (indx > 0)
          {}
          break;
        default:
          messages.push_back(Compiler::message_t('E', 0, begin,
                                                 "Mapping IO, cannot find X or Y"));
      }
      memory.addNextCODE(index - '0');
      memory.addNextCODE(p->data);
    }
  }

  void do_factor(const char* begin, const char* end)
  {
    std::clog << "*** [Parser] factor: " << std::string(begin, end) <<
                 " " << parList.size() << " " << isFunction << std::endl;
    if (!isFunction) {
      exprOutputRPN.push(identifier);
      std::clog << "*** [Parser] factor: is var/constant" << std::endl;
    }
    else
    {
      //exprOperatorStack.push(identifier);
      auto p0 = find(symbolsData, identifier.c_str());
      if (nullptr == p0 || p0->qualifier != "COMPARATOR") {
        messages.push_back(Compiler::message_t('E', 0, begin,
                                               "Unknown function block '"
                                               + identifier + "'"));
      }
      else
      {
        if (parList.size() != size_t(p0->data))
        {
          std::stringstream message;
          message << "Number of parameters of '" << identifier
                  << "' != " << p0->data;
          messages.push_back(Compiler::message_t('E', 0, begin, message.str()));
        }
        else
        {
          auto it = parList.cbegin();
          while (it != parList.end())
          {
            exprOutputRPN.push(*it);
            ++it;
          }
          exprOutputRPN.push(identifier);
        }
      }
      std::clog << "*** [Parser] factor: is function" << std::endl;
    }
    showExprOutputRPN();
    identifier.clear();
    parList.clear();
    isFunction = false;
  }

  void do_expression(const char* begin, const char* end)
  {
    std::clog << "*** [Parser] expression: " << std::string(begin, end)
              << std::endl;
  }

  void do_exprAddToOutput(const char* begin, const char* end)
  {
    std::clog << "*** [Parser] to output '" << std::string(begin, end) << "'"
              << std::endl;
    exprOutputRPN.push(std::string(begin, end));
  }

  void do_exprAddToOperatorStack(const char* begin, const char* end)
  {
    std::string Operator(begin, end);
    std::clog << "*** [Parser] to opStack '" << Operator  << "'" << std::endl;
    if (Operator == "NOT" || Operator == "(") {
      exprOperatorStack.push(Operator);
    }
    else
    {
      symbolData_t* p1 = find(symbolsData, Operator.c_str());
      if (p1->qualifier == "COMPARATOR") {
        exprOutputRPN.push(Operator);
      }
      else
      {
        bool ready = false;
        while (!exprOperatorStack.empty()
               && exprOperatorStack.top() != "("
               && !ready)
        {
          std::string OperatorOnStack(exprOperatorStack.top());
          symbolData_t* p2 = find(symbolsData, OperatorOnStack.c_str());
          // compare operator priority
          if (p1->data > p2->data)
          {
            exprOutputRPN.push(OperatorOnStack);
            exprOperatorStack.pop();
          }
          else
          {
            ready = true;
          }
        }
      }
      exprOperatorStack.push(Operator);
    }
  }

  void do_rparen(const char* /* begin */, const char* /* end */)
  {
    std::clog << "*** [Parser] RPAREN" << std::endl;
    showExprOutputRPN();
    while (!exprOperatorStack.empty() && exprOperatorStack.top() != "(")
    {
      std::string OperatorOnStack(exprOperatorStack.top());
      std::clog << "*** [Parser] ) operator on top stack '" << OperatorOnStack
                << "'" << std::endl;
      exprOutputRPN.push(OperatorOnStack);
      exprOperatorStack.pop();
      showExprOutputRPN();
    }
    // Pop "("
    exprOperatorStack.pop();
    showExprOutputRPN();
  }

  void do_compileRPN(const char* begin, const char* /* end */)
  {
    std::clog << "*** [Parser] compileRPN" << std::endl;
    // Put entire exprOperatorStack to exprOutputRPN
    while (!exprOperatorStack.empty())
    {
      exprOutputRPN.push(exprOperatorStack.top());
      exprOperatorStack.pop();
    }
    showExprOutputRPN();

    // Compile RPN representation of expression
    while (!exprOutputRPN.empty())
    {
      auto p0 = find(symbolsData, exprOutputRPN.front().c_str());
      if (p0)
      {
        if (p0->qualifier == "" || p0->qualifier == "CONST")
        {
          std::clog << "*** [Parser] compile a var/const "
                    << exprOutputRPN.front() << std::endl;
          memory.addNextCODE(processorConfig.getIC("SPUSH"));
          memory.addNextCODE(p0->data);
        }
        else
        {
          std::clog << "*** [Parser] compile a function "
                    << exprOutputRPN.front() << std::endl;
          memory.addNextCODE(processorConfig.getIC("S" + exprOutputRPN.front()));
        }
      }
      else
      {
        messages.push_back(Compiler::message_t('E', 0, begin,
                                               "'" + exprOutputRPN.front()
                                               + "' does not exists!"));
      }
      exprOutputRPN.pop();
    }
    if (tmpvarIndex >= MemoryConfig::TMPVARS_SIZE)
    {
      messages.push_back(Compiler::message_t('E', 0, begin, "Out of memory!"));
    }
    else
    {
      memory.addNextCODE(processorConfig.getIC("SPOPB"));
      memory.addNextCODE(MemoryConfig::TMPVARS + tmpvarIndex);
      memory.addNextCODE(processorConfig.getIC("SPUSH"));
      memory.addNextCODE(MemoryConfig::TMPVARS + tmpvarIndex);
      ++tmpvarIndex;
    }
  }

  void do_negationContact(const char* /* begin */, const char* /* end */)
  {
    memory.addNextCODE(processorConfig.getIC("SNOT"));
  }

  void do_addCoilID(const char* begin, const char* end)
  {
    std::string id(begin, end);
    std::clog << "*** [Parser] to coils queue '" << id << "'" << std::endl;
    auto p = find(symbolsData, id.c_str());
    if (nullptr == p) {
      messages.push_back(Compiler::message_t('E', 0, begin,
                                             "Variable '" + id
                                             + "' does not exists!"));
    }
    coils.push(id);
  }

  void do_normalCoil(const char* begin, const char* /* end */)
  {
    if (!coils.empty())
    {
      std::clog << "*** [Parser] Normal coil '" << coils.front() << "'"
                << std::endl;
      auto p = find(symbolsData, coils.front().c_str());
      if (p)
      {
        memory.addNextCODE(processorConfig.getIC("SCNM"));
        memory.addNextCODE(p->data);
      }
      else
      {
        messages.push_back(Compiler::message_t('E', 0, begin,
                                               "Coil reference '"
                                               + coils.front()
                                               + "' does not exists!"));
      }
      coils.pop();
    }
    else
    {
      messages.push_back(Compiler::message_t('E', 0, begin,
                                             "Missing coil reference!"));
    }
  }

  void do_negationCoil(const char* begin, const char* /* end */)
  {
    if (!coils.empty())
    {
      std::clog << "*** [Parser] Negation coil '" << coils.front() << "'"
                << std::endl;
      auto p = find(symbolsData, coils.front().c_str());
      if (p) {
        memory.addNextCODE(processorConfig.getIC("SCNG"));
        memory.addNextCODE(p->data);
      }
      else
      {
        messages.push_back(Compiler::message_t('E', 0, begin,
                                               "Coil reference '"
                                               + coils.front()
                                               + "' does not exists!"));
      }
      coils.pop();
    }
    else
    {
      messages.push_back(Compiler::message_t('E', 0, begin,
                                             "Missing coil reference!"));
    }
  }

  void do_setCoil(const char* begin, const char* /* end */)
  {
    if (!coils.empty())
    {
      std::clog << "*** [Parser] Set coil '" << coils.front() << "'"
                << std::endl;
      auto p = find(symbolsData, coils.front().c_str());
      if (p) {
        memory.addNextCODE(processorConfig.getIC("SCST"));
        memory.addNextCODE(p->data);
      }
      else
      {
        messages.push_back(Compiler::message_t('E', 0, begin,
                                               "Coil reference '"
                                               + coils.front()
                                               + "' does not exists!"));
      }
      coils.pop();
    }
    else
    {
      messages.push_back(Compiler::message_t('E', 0, begin,
                                             "Missing coil reference!"));
    }
  }

  void do_resetCoil(const char* begin, const char* /* end */)
  {
    if (!coils.empty())
    {
      std::clog << "*** [Parser] Reset coil '" << coils.front() << "'"
                << std::endl;
      auto p = find(symbolsData, coils.front().c_str());
      if (p) {
        memory.addNextCODE(processorConfig.getIC("SCRS"));
        memory.addNextCODE(p->data);
      }
      else
      {
        messages.push_back(Compiler::message_t('E', 0, begin,
                                               "Coil reference '"
                                               + coils.front()
                                               + "' does not exists!"));
      }
      coils.pop();
    }
    else
    {
      messages.push_back(Compiler::message_t('E', 0, begin,
                                             "Missing coil reference!"));
    }
  }

  void do_ladderDiagram(const char* begin, const char* /* end */)
  {
    if (coils.empty()) {
      memory.addNextCODE(processorConfig.getIC("SDROP"));
    }
    else
    {
      messages.push_back(Compiler::message_t('E', 0, begin,
                                             "Number of references > number of coils."));
    }
  }

  void do_terminate(const char* /* begin */, const char* /* end */)
  {
    memory.addNextCODE(processorConfig.getIC("RESTORE"));
    memory.addNextCODE(processorConfig.getIC("SHX"));
    memory.addNextCODE(processorConfig.getIC("SHY"));
    memory.addNextCODE(processorConfig.getIC("EOPL"));
    // Store initial values for clearing PLC
    std::copy(&memory[MemoryConfig::VARS],
        &memory[MemoryConfig::VARS + MemoryConfig::VARS_SIZE],
        &memory[MemoryConfig::VARS_INIT]);
    std::copy(&memory[MemoryConfig::COUNTERS],
        &memory[MemoryConfig::COUNTERS + MemoryConfig::nCOUNTERS * MemoryConfig::COUNTER_SIZE],
        &memory[MemoryConfig::COUNTERS_INIT]);
    std::copy(&memory[MemoryConfig::TIMERS],
        &memory[MemoryConfig::TIMERS + MemoryConfig::nTIMERS * MemoryConfig::TIMER_SIZE],
        &memory[MemoryConfig::TIMERS_INIT]);
    // Clear parser status vars
    tmpvarIndex = 0;
    //memory.dump(cerr);
  }

  const Memory& getResult()
  {
    return memory;
  }

};

struct skipParser: public bsp::grammar<skipParser>
{
  template <typename ScannerT>
  struct definition
  {
    bsp::rule<ScannerT> skip;

    definition(const skipParser& /* self */)
    {
      skip = bsp::comment_p("//") | bsp::space_p | bsp::ch_p('\n') | bsp::blank_p;

#ifdef BOOST_SPIRIT_DEBUG
      BOOST_SPIRIT_DEBUG_RULE(skip);
#endif

    }

    const bsp::rule<ScannerT>& start() { return skip; }
  };
};

struct PLCscriptParser: public bsp::grammar<PLCscriptParser>
{
  VirtualPLCBuilder& vpb_;

  PLCscriptParser(VirtualPLCBuilder& vpb): vpb_(vpb) { }

  template <typename ScannerT>
  struct definition
  {
    definition(PLCscriptParser const& self)
    {
      VirtualPLCBuilder& vpb = self.vpb_;

#define BOOST_SPIRIT_DEBUG
#ifdef BOOST_SPIRIT_DEBUG
      debug();
#endif

      keywords =
          "true", "false",
          "CONST", "INT", "BOOL", "TIMER_M", "COUNTER_M",
          "AND", "OR", "XOR", "NOT",
          "GT", "GE", "EQ", "LE", "LT", "NE";

      bsp::strlit<> ASSIGN("=");
      bsp::strlit<> LPAREN("(");
      bsp::strlit<> RPAREN(")");
      bsp::strlit<> SEMI(";");
      bsp::strlit<> TRUE("true");
      bsp::strlit<> FALSE("false");
      bsp::strlit<> CONST("CONST");
      bsp::strlit<> NOT("NOT");
      bsp::strlit<> OR("OR");
      bsp::strlit<> AND("AND");
      bsp::strlit<> XOR("XOR");
      bsp::strlit<> TIMER_M("TIMER_M");
      bsp::strlit<> COUNTER_M("COUNTER_M");
      bsp::strlit<> GT("GT");
      bsp::strlit<> GE("GE");
      bsp::strlit<> EQ("EQ");
      bsp::strlit<> LE("LE");
      bsp::strlit<> LT("LT");
      bsp::strlit<> NE("NE");

      script =
          *(declaration)
          >> *(functionBlock)
          >> *(mappingIO)
          >> *(ladderDiagram)
          >> bsp::end_p[vpb.terminate]
             ;

      declaration =
          ((qualifier[bsp::assign_a(vpb.qualifier)]
           >> type[bsp::assign_a(vpb.type)] >> identifier[bsp::assign_a(vpb.lhs)]
          >> ASSIGN
          >> (value[bsp::assign_a(vpb.rhs, "")] | identifier[bsp::assign_a(vpb.value, "")][bsp::assign_a(vpb.rhs)])
          >> SEMI)[vpb.declaration]) |
          ((type[bsp::assign_a(vpb.type)][bsp::assign_a(vpb.qualifier, "")]
          >> identifier[bsp::assign_a(vpb.lhs)]
          >> ASSIGN
          >> (value[bsp::assign_a(vpb.rhs, "")] | identifier[bsp::assign_a(vpb.value, "")][bsp::assign_a(vpb.rhs)])
          >> SEMI)[vpb.declaration])
          ;

      mappingIO =
          (bsp::lexeme_d[(bsp::ch_p('X')[bsp::assign_a(vpb.XY)] | bsp::ch_p('Y')[bsp::assign_a(vpb.XY)])
          >> bsp::digit_p[bsp::assign_a(vpb.index)]]
          >> ASSIGN >> identifier[bsp::assign_a(vpb.identifier)] >> SEMI)[vpb.mapping]
          ;

      empty =
          bsp::epsilon_p
          ;

      term =
          factor[vpb.factor] |
          (NOT[vpb.exprAddToOperatorStack] >> factor[vpb.factor]) |
          (LPAREN[vpb.exprAddToOperatorStack] >> expression >> RPAREN[vpb.rparen])
          ;

      // bsp::assign_a(vpb.isFunction, true) does not work (??) replaced by bsp::assign_a(vpb.isFunction, 1)
      factor =
          identifier[bsp::assign_a(vpb.identifier)]
          >> if_p(LPAREN)[!(identifier[bsp::push_back_a(vpb.parList)]
          >>
             *("," >> identifier[bsp::push_back_a(vpb.parList)]) >>
          RPAREN)[bsp::assign_a(vpb.isFunction, 1)]]
          ;

      expression =
          ((!NOT[vpb.exprAddToOperatorStack]) >> term
                                              >> *(binaryOperator[vpb.exprAddToOperatorStack]
                                              >> (!NOT[vpb.exprAddToOperatorStack]) >> term))[vpb.expression]
          ;

      // bsp::assign_a(vpb.isFunction, true) replaced by bsp::assign_a(vpb.isFunction, 1)
      //         term =
      //             (identifier[assign_a(vpb.identifier)]
      //             >> if_p(LPAREN)[!(identifier[push_back_a(vpb.parList)] >>
      //                               *("," >> identifier[push_back_a(vpb.parList)]) >>
      //                               RPAREN)[assign_a(vpb.isFunction, 1)]])
      //             |
      //             (LPAREN[vpb.exprAddToOperatorStack] >> expression >> RPAREN[vpb.rparen])
      //             ;

      //         factor =
      //             !NOT[vpb.exprAddToOperatorStack] >> term[vpb.factor]
      //             ;

      //         expression =
      //              factor % binaryOperator[vpb.exprAddToOperatorStack]
      //              ;

      binaryOperator =
          AND | OR | XOR
          ;

      unaryOperator =
          NOT
          ;

      identifier =
          (bsp::lexeme_d[((bsp::alpha_p >> *(bsp::alnum_p | bsp::ch_p('_')))
                     - (keywords >> (bsp::anychar_p - (bsp::alnum_p | '_'))))])
          ;

      coil =
          // normal
          (bsp::ch_p('(') >> bsp::ch_p(')'))[vpb.normalCoil] |
          // negation
          (bsp::ch_p('(') >> bsp::ch_p('/') >> bsp::ch_p(')'))[vpb.negationCoil] |
          // set
          (bsp::ch_p('(') >> bsp::ch_p('S') >> bsp::ch_p(')'))[vpb.setCoil] |
          // reset
          (bsp::ch_p('(') >> bsp::ch_p('R') >> bsp::ch_p(')'))[vpb.resetCoil]
          ;

      contact =
          // normal
          (bsp::ch_p('[') >> bsp::ch_p(']')) |
          // negation
          (bsp::ch_p('[') >> bsp::ch_p('/') >> bsp::ch_p(']'))[vpb.negationContact]
          ;

      ladderDiagram =
          (expression >> identifier[vpb.compileRPN][vpb.addCoilID]
          >> *(identifier)[vpb.addCoilID]
          >> bsp::lexeme_d[+bsp::ch_p('-')
          >> contact >> +bsp::ch_p('-')
          >> +(coil >> *bsp::ch_p('-'))])[vpb.ladderDiagram]
          ;

      type =
          bsp::lexeme_d[bsp::alpha_p >> *(bsp::alnum_p | bsp::ch_p('_'))][bsp::assign_a(vpb.type)]
          ;

      qualifier =
          bsp::lexeme_d[bsp::alpha_p >> *(bsp::alnum_p | bsp::ch_p('_'))][bsp::assign_a(vpb.qualifier)]
          ;

      value =
          (TRUE | FALSE | bsp::int_p)[bsp::assign_a(vpb.value)]
          ;

      functionBlock =
          ((TIMER_M >> bsp::lexeme_d[bsp::strlit<>("TIMER") >> bsp::int_p[bsp::assign_a(vpb.timerIndex)]
          >> bsp::strlit<>("C")]
          >> LPAREN >> identifier[bsp::push_back_a(vpb.parList)] >> bsp::ch_p(',')
          >> (identifier[bsp::push_back_a(vpb.parList)] | bsp::int_p[bsp::assign_a(vpb.timerValue)])
          >> RPAREN >> SEMI)[vpb.timerDeclaration]) |
          ((COUNTER_M >> bsp::lexeme_d[bsp::strlit<>("COUNTER") >> bsp::int_p[bsp::assign_a(vpb.counterIndex)]
          >> bsp::strlit<>("C")]
          >> LPAREN >> identifier[bsp::push_back_a(vpb.parList)] >> bsp::ch_p(',')
          >> (identifier[bsp::push_back_a(vpb.parList)] | bsp::int_p[bsp::assign_a(vpb.counterValue)])
          >> RPAREN >> SEMI)[vpb.counterDeclaration])
          ;
    }

    bsp::symbols<> keywords;
    using rule_t = bsp::rule<ScannerT>;

    rule_t
    script,
    declaration,
    mappingIO,
    group,
    empty,
    factor,
    term,
    expression,
    binaryOperator,
    unaryOperator,
    identifier,
    coil,
    contact,
    ladderDiagram,
    type,
    qualifier,
    value,
    functionBlock;


#ifdef BOOST_SPIRIT_DEBUG
    void debug()
    {
      BOOST_SPIRIT_DEBUG_RULE(script);
      BOOST_SPIRIT_DEBUG_RULE(declaration);
      BOOST_SPIRIT_DEBUG_RULE(mappingIO);
      BOOST_SPIRIT_DEBUG_RULE(group);
      BOOST_SPIRIT_DEBUG_RULE(empty);
      BOOST_SPIRIT_DEBUG_RULE(factor);
      BOOST_SPIRIT_DEBUG_RULE(term);
      BOOST_SPIRIT_DEBUG_RULE(expression);
      BOOST_SPIRIT_DEBUG_RULE(binaryOperator);
      BOOST_SPIRIT_DEBUG_RULE(unaryOperator);
      BOOST_SPIRIT_DEBUG_RULE(identifier);
      BOOST_SPIRIT_DEBUG_RULE(coil);
      BOOST_SPIRIT_DEBUG_RULE(contact);
      BOOST_SPIRIT_DEBUG_RULE(ladderDiagram);
      BOOST_SPIRIT_DEBUG_RULE(type);
      BOOST_SPIRIT_DEBUG_RULE(qualifier);
      BOOST_SPIRIT_DEBUG_RULE(value);
      BOOST_SPIRIT_DEBUG_RULE(functionBlock);
    }
#endif

    bsp::rule<ScannerT> const& start() const
    {
      return script;
    }

  };
};

#endif // PARSER_H
