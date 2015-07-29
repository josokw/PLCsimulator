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

#include <boost/bind.hpp>

using namespace std;
using namespace boost::spirit::classic;

/// This structure contains all semantic actions for the PLCscript parser.
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

  typedef struct symbolData
  {
    symbolData(const std::string& q, const std::string& t, int d)
      :  qualifier(q)
      ,  type(t)
      ,  data(d)
    {}

    std::string qualifier;
    std::string type;
    int data;
  } symbolData_t;

  symbols<symbolData_t> symbolsData;

  function<void(const char*, const char*)> info;
  function<void(const char*, const char*)> declaration;
  function<void(const char*, const char*)> timerDeclaration;
  function<void(const char*, const char*)> counterDeclaration;
  function<void(const char*, const char*)> mapping;
  function<void(const char*, const char*)> factor;
  function<void(const char*, const char*)> expression;
  function<void(const char*, const char*)> exprAddToOutput;
  function<void(const char*, const char*)> exprAddToOperatorStack;
  function<void(const char*, const char*)> rparen;
  function<void(const char*, const char*)> compileRPN;
  function<void(const char*, const char*)> negationContact;
  function<void(const char*, const char*)> addCoilID;
  function<void(const char*, const char*)> normalCoil;
  function<void(const char*, const char*)> negationCoil;
  function<void(const char*, const char*)> setCoil;
  function<void(const char*, const char*)> resetCoil;
  function<void(const char*, const char*)> ladderDiagram;
  function<void(const char*, const char*)> terminate;

  VirtualPLCBuilder* self() { return this; }

  VirtualPLCBuilder(std::vector<Compiler::message_t>& messages)
    :  messages(messages)
    ,  memory()
    ,  processor(&memory, 0)
    ,  tmpvarIndex(0)
    ,  isFunction(false)
    ,  info(boost::bind(&VirtualPLCBuilder::do_info, self(), _1, _2))
    ,  declaration(boost::bind(&VirtualPLCBuilder::do_declaration, self(), _1, _2))
    ,  timerDeclaration(boost::bind(&VirtualPLCBuilder::do_timerDeclaration, self(), _1, _2))
    ,  counterDeclaration(boost::bind(&VirtualPLCBuilder::do_counterDeclaration, self(), _1, _2))
    ,  mapping(boost::bind(&VirtualPLCBuilder::do_mapping, self(), _1, _2))
    ,  factor(boost::bind(&VirtualPLCBuilder::do_factor, self(), _1, _2))
    ,  expression(boost::bind(&VirtualPLCBuilder::do_expression, self(), _1, _2))
    ,  exprAddToOutput(boost::bind(&VirtualPLCBuilder::do_exprAddToOutput, self(), _1, _2))
    ,  exprAddToOperatorStack(boost::bind(&VirtualPLCBuilder::do_exprAddToOperatorStack, self(), _1, _2))
    ,  rparen(boost::bind(&VirtualPLCBuilder::do_rparen, self(), _1, _2))
    ,  compileRPN(boost::bind(&VirtualPLCBuilder::do_compileRPN, self(), _1, _2))
    ,  negationContact(boost::bind(&VirtualPLCBuilder::do_negationContact, self(), _1, _2))
    ,  addCoilID(boost::bind(&VirtualPLCBuilder::do_addCoilID, self(), _1, _2))
    ,  normalCoil(boost::bind(&VirtualPLCBuilder::do_normalCoil, self(), _1, _2))
    ,  negationCoil(boost::bind(&VirtualPLCBuilder::do_negationCoil, self(), _1, _2))
    ,  setCoil(boost::bind(&VirtualPLCBuilder::do_setCoil, self(), _1, _2))
    ,  resetCoil(boost::bind(&VirtualPLCBuilder::do_resetCoil, self(), _1, _2))
    ,  ladderDiagram(boost::bind(&VirtualPLCBuilder::do_ladderDiagram, self(), _1, _2))
    ,  terminate(boost::bind(&VirtualPLCBuilder::do_terminate, self(), _1, _2))
  {
    memory.addNextCODE(processorConfig.getIC("LATCH"));
    memory.addNextCODE(processorConfig.getIC("CCNTS"));
    // LOGICAL OPERATORS symbolData: qualifier, result type, operator priority
    boost::spirit::classic::add(symbolsData, "NOT",
                                symbolData("UNARY_OPERATOR", "BOOL", 1));
    boost::spirit::classic::add(symbolsData, "AND",
                                symbolData("BINARY_OPERATOR", "BOOL", 2));
    boost::spirit::classic::add(symbolsData, "OR",
                                symbolData("BINARY_OPERATOR", "BOOL", 3));
    boost::spirit::classic::add(symbolsData, "XOR",
                                symbolData("BINARY_OPERATOR", "BOOL", 4));
    // LPAREN symbolData: qualifier, result type, operator priority
    boost::spirit::classic::add(symbolsData, "(",
                                symbolData("LPAREN", "", 0));
    // COMPARATORS symbolData: qualifier, result type, number of arguments
    boost::spirit::classic::add(symbolsData, "GT",
                                symbolData("COMPARATOR", "BOOL", 2));
    boost::spirit::classic::add(symbolsData, "GE",
                                symbolData("COMPARATOR", "BOOL", 2));
    boost::spirit::classic::add(symbolsData, "EQ",
                                symbolData("COMPARATOR", "BOOL", 2));
    boost::spirit::classic::add(symbolsData, "LE",
                                symbolData("COMPARATOR", "BOOL", 2));
    boost::spirit::classic::add(symbolsData, "LT",
                                symbolData("COMPARATOR", "BOOL", 2));
    boost::spirit::classic::add(symbolsData, "NE",
                                symbolData("COMPARATOR", "BOOL", 2));
  }

  void showExprOutputRPN()
  {
    std::queue<string> RPN(exprOutputRPN);
    std::cout << "*** [Parser] expression RPN: ";
    while (!RPN.empty()) {
      string s(RPN.front());
      cout << s << " ";
      RPN.pop();
    }
    std::cout << std::endl;
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
        boost::spirit::classic::add(symbolsData, lhs.c_str(),
                                    symbolData(qualifier, type,
                                               memory.getNextVARaddress()));
        if (type == "INT") memory.addNextVAR(iValue);
        if (type == "BOOL") iValue == 1
            ? memory.addNextVAR(true) : memory.addNextVAR(false);
      }
      else
      {
        boost::spirit::classic::add(symbolsData, lhs.c_str(),
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
      string tI(1, '0'+ timerIndex);
      messages.push_back(Compiler::message_t('E', 0, begin, "Timer" + tI
                                             + "C is not available"));
    }
    else
    {
      if (parList.size() == 1)
      {
        cout << "[Parser] " << string(begin, end) << " " << timerIndex << " "
             << parList[0] << " " << timerValue << endl;
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
        cout << "[Parser] " << string(begin, end) << "  " << timerIndex << " "
             << parList[0] << " " << parList[1] << endl;
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
          memory.addNextCODE(processorConfig.getIC(string("CPY")));
          memory.addNextCODE(p0->data);
          memory.addNextCODE(timerOffset + timerIndex * MemoryConfig::TIMER_SIZE
                             + FunctionConfig::TEnable);
          memory.dump(cerr);
        }
      }
      std::string timerName("TIMER" + to_string(timerIndex));
      {
        std::string timerNameSub(timerName + "N");
        boost::spirit::classic::add(symbolsData, timerNameSub.c_str(),
                                    symbolData(std::string(""), std::string("INT"),
                                               timerOffset
                                               + timerIndex * MemoryConfig::TIMER_SIZE
                                               + FunctionConfig::TN));
      }
      {
        std::string timerNameSub(timerName + "S");
        boost::spirit::classic::add(symbolsData, timerNameSub.c_str(),
                                    symbolData(std::string(""), std::string("BOOL"),
                                               timerOffset
                                               + timerIndex * MemoryConfig::TIMER_SIZE
                                               + FunctionConfig::TS));
      }
      {
        std::string timerNameSub(timerName + "C");
        boost::spirit::classic::add(symbolsData, timerNameSub.c_str(),
                                    symbolData(std::string(""), std::string("BOOL"),
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
      string cI(1, '0'+ counterIndex);
      messages.push_back(Compiler::message_t('E', 0, begin,
                                             "Counter" + cI
                                             + "C is not available"));
    }
    else
    {
      if (parList.size() == 1) {
        cout << "[Parser] " << string(begin, end) << " " << counterIndex << " "
             << parList[0] << " " << counterValue << endl;
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
        cout << "[Parser] " << string(begin, end) << "  " << counterIndex << " "
             << parList[0] << " " << parList[1] << endl;
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
          memory.addNextCODE(processorConfig.getIC(string("CPY")));
          memory.addNextCODE(p0->data);
          memory.addNextCODE(counterOffset
                             + counterIndex * MemoryConfig::COUNTER_SIZE
                             + FunctionConfig::CEnable);
          memory.dump(cerr);
        }
      }
      std::string counterName("COUNTER" + to_string(counterIndex));
      {
        std::string counterNameSub(counterName + "N");
        boost::spirit::classic::add(symbolsData, counterNameSub.c_str(),
                                    symbolData(std::string(""), std::string("INT"),
                                               counterOffset
                                               + counterIndex * MemoryConfig::COUNTER_SIZE
                                               + FunctionConfig::CN));
      }
      {
        std::string counterNameSub(counterName + "S");
        boost::spirit::classic::add(symbolsData, counterNameSub.c_str(),
                                    symbolData(std::string(""), std::string("BOOL"),
                                               counterOffset
                                               + counterIndex * MemoryConfig::COUNTER_SIZE
                                               + FunctionConfig::CS));
      }
      {
        std::string counterNameSub(counterName + "C");
        boost::spirit::classic::add(symbolsData, counterNameSub.c_str(),
                                    symbolData(std::string(""), std::string("BOOL"),
                                               counterOffset
                                               + counterIndex * MemoryConfig::COUNTER_SIZE
                                               + FunctionConfig::CC));
      }
    }
    parList.clear();
  }

  void do_mapping(const char* begin, const char* /* end */)
  {
    std::cout << "*** [Parser] mapping " << XY << index << " = " << identifier << std::endl;
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
          memory.addNextCODE(processorConfig.getIC(string("READX")));
          if (indx > 0)
          {}
          break;
        case 'Y':
          memory.addNextCODE(processorConfig.getIC(string("SETY")));
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
    std::cout << "*** [Parser] factor: " << std::string(begin, end) <<
                 " " << parList.size() << " " << isFunction << std::endl;
    if (!isFunction) {
      exprOutputRPN.push(identifier);
      std::cout << "*** [Parser] factor: is var/constant" << std::endl;
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
      std::cout << "*** [Parser] factor: is function" << std::endl;
    }
    showExprOutputRPN();
    identifier.clear();
    parList.clear();
    isFunction = false;
  }

  void do_expression(const char* begin, const char* end)
  {
    std::cout << "*** [Parser] expression: " << std::string(begin, end)
              << std::endl;
  }

  void do_exprAddToOutput(const char* begin, const char* end)
  {
    std::cout << "*** [Parser] to output '" << std::string(begin, end) << "'"
              << std::endl;
    exprOutputRPN.push(std::string(begin, end));
  }

  void do_exprAddToOperatorStack(const char* begin, const char* end)
  {
    string Operator(begin, end);
    std::cout << "*** [Parser] to opStack '" << Operator  << "'" << std::endl;
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
    std::cout << "*** [Parser] RPAREN" << std::endl;
    showExprOutputRPN();
    while (!exprOperatorStack.empty() && exprOperatorStack.top() != "(")
    {
      std::string OperatorOnStack(exprOperatorStack.top());
      std::cout << "*** [Parser] ) operator on top stack '" << OperatorOnStack
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
    std::cout << "*** [Parser] compileRPN" << std::endl;
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
          std::cout << "*** [Parser] compile a var/const "
                    << exprOutputRPN.front() << std::endl;
          memory.addNextCODE(processorConfig.getIC("SPUSH"));
          memory.addNextCODE(p0->data);
        }
        else
        {
          std::cout << "*** [Parser] compile a function "
                    << exprOutputRPN.front() << std::endl;
          memory.addNextCODE(processorConfig.getIC(string("S")
                                                   + exprOutputRPN.front()));
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
    std::cout << "*** [Parser] to coils queue '" << id << "'" << std::endl;
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
      std::cout << "*** [Parser] Normal coil '" << coils.front() << "'"
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
      std::cout << "*** [Parser] Negation coil '" << coils.front() << "'"
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
      std::cout << "*** [Parser] Set coil '" << coils.front() << "'"
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
      std::cout << "*** [Parser] Reset coil '" << coils.front() << "'"
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
    copy(&memory[MemoryConfig::VARS],
        &memory[MemoryConfig::VARS + MemoryConfig::VARS_SIZE],
        &memory[MemoryConfig::VARS_INIT]);
    copy(&memory[MemoryConfig::COUNTERS],
        &memory[MemoryConfig::COUNTERS + MemoryConfig::nCOUNTERS * MemoryConfig::COUNTER_SIZE],
        &memory[MemoryConfig::COUNTERS_INIT]);
    copy(&memory[MemoryConfig::TIMERS],
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

struct skipParser: public grammar<skipParser>
{
  template <typename ScannerT>
  struct definition
  {
    rule<ScannerT> skip;

    definition(const skipParser& /* self */)
    {
      skip = comment_p("//") | space_p | ch_p('\n') | blank_p;

#ifdef BOOST_SPIRIT_DEBUG
      BOOST_SPIRIT_DEBUG_RULE(skip);
#endif

    }

    const rule<ScannerT>& start() { return skip; }
  };
};

struct PLCscriptParser: public grammar<PLCscriptParser>
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

      strlit<> ASSIGN("=");
      strlit<> LPAREN("(");
      strlit<> RPAREN(")");
      strlit<> SEMI(";");
      strlit<> TRUE("true");
      strlit<> FALSE("false");
      strlit<> CONST("CONST");
      strlit<> NOT("NOT");
      strlit<> OR("OR");
      strlit<> AND("AND");
      strlit<> XOR("XOR");
      strlit<> TIMER_M("TIMER_M");
      strlit<> COUNTER_M("COUNTER_M");
      strlit<> GT("GT");
      strlit<> GE("GE");
      strlit<> EQ("EQ");
      strlit<> LE("LE");
      strlit<> LT("LT");
      strlit<> NE("NE");

      script =
          *(declaration)
          >> *(functionBlock)
          >> *(mappingIO)
          >> *(ladderDiagram)
          >> end_p[vpb.terminate]
             ;

      declaration =
          ((qualifier[assign_a(vpb.qualifier)]
           >> type[assign_a(vpb.type)] >> identifier[assign_a(vpb.lhs)]
          >> ASSIGN
          >> (value[assign_a(vpb.rhs, "")] | identifier[assign_a(vpb.value, "")][assign_a(vpb.rhs)])
          >> SEMI)[vpb.declaration]) |
          ((type[assign_a(vpb.type)][assign_a(vpb.qualifier, "")]
          >> identifier[assign_a(vpb.lhs)]
          >> ASSIGN
          >> (value[assign_a(vpb.rhs, "")] | identifier[assign_a(vpb.value, "")][assign_a(vpb.rhs)])
          >> SEMI)[vpb.declaration])
          ;

      mappingIO =
          (lexeme_d[(ch_p('X')[assign_a(vpb.XY)] | ch_p('Y')[assign_a(vpb.XY)])
          >> digit_p[assign_a(vpb.index)]]
          >> ASSIGN >> identifier[assign_a(vpb.identifier)] >> SEMI)[vpb.mapping]
          ;

      empty =
          epsilon_p
          ;

      term =
          factor[vpb.factor] |
          (NOT[vpb.exprAddToOperatorStack] >> factor[vpb.factor]) |
          (LPAREN[vpb.exprAddToOperatorStack] >> expression >> RPAREN[vpb.rparen])
          ;

      // assign_a(vpb.isFunction, true) does not work (??) replaced by assign_a(vpb.isFunction, 1)
      factor =
          identifier[assign_a(vpb.identifier)]
          >> if_p(LPAREN)[!(identifier[push_back_a(vpb.parList)]
          >>
             *("," >> identifier[push_back_a(vpb.parList)]) >>
          RPAREN)[assign_a(vpb.isFunction, 1)]]
          ;

      expression =
          ((!NOT[vpb.exprAddToOperatorStack]) >> term
                                              >> *(binaryOperator[vpb.exprAddToOperatorStack]
                                              >> (!NOT[vpb.exprAddToOperatorStack]) >> term))[vpb.expression]
          ;

      // assign_a(vpb.isFunction, true) replaced by assign_a(vpb.isFunction, 1)
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
          (lexeme_d[((alpha_p >> *(alnum_p | ch_p('_')))
                     - (keywords >> (anychar_p - (alnum_p | '_'))))])
          ;

      coil =
          // normal
          (ch_p('(') >> ch_p(')'))[vpb.normalCoil] |
          // negation
          (ch_p('(') >> ch_p('/') >> ch_p(')'))[vpb.negationCoil] |
          // set
          (ch_p('(') >> ch_p('S') >> ch_p(')'))[vpb.setCoil] |
          // reset
          (ch_p('(') >> ch_p('R') >> ch_p(')'))[vpb.resetCoil]
          ;

      contact =
          // normal
          (ch_p('[') >> ch_p(']')) |
          // negation
          (ch_p('[') >> ch_p('/') >> ch_p(']'))[vpb.negationContact]
          ;

      ladderDiagram =
          (expression >> identifier[vpb.compileRPN][vpb.addCoilID]
          >> *(identifier)[vpb.addCoilID]
          >> lexeme_d[+ch_p('-')
          >> contact >> +ch_p('-')
          >> +(coil >> *ch_p('-'))])[vpb.ladderDiagram]
          ;

      type =
          lexeme_d[alpha_p >> *(alnum_p | ch_p('_'))][assign_a(vpb.type)]
          ;

      qualifier =
          lexeme_d[alpha_p >> *(alnum_p | ch_p('_'))][assign_a(vpb.qualifier)]
          ;

      value =
          (TRUE | FALSE | int_p)[assign_a(vpb.value)]
          ;

      functionBlock =
          ((TIMER_M >> lexeme_d[strlit<>("TIMER") >> int_p[assign_a(vpb.timerIndex)]
          >> strlit<>("C")]
          >> LPAREN >> identifier[push_back_a(vpb.parList)] >> ch_p(',')
          >> (identifier[push_back_a(vpb.parList)] | int_p[assign_a(vpb.timerValue)])
          >> RPAREN >> SEMI)[vpb.timerDeclaration]) |
          ((COUNTER_M >> lexeme_d[strlit<>("COUNTER") >> int_p[assign_a(vpb.counterIndex)]
          >> strlit<>("C")]
          >> LPAREN >> identifier[push_back_a(vpb.parList)] >> ch_p(',')
          >> (identifier[push_back_a(vpb.parList)] | int_p[assign_a(vpb.counterValue)])
          >> RPAREN >> SEMI)[vpb.counterDeclaration])
          ;
    }

    symbols<> keywords;
    typedef rule<ScannerT> rule_t;

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

    rule<ScannerT> const& start() const
    {
      return script;
    }

  };
};

#endif // PARSER_H
