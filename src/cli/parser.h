/*
 * Copyright (c) 2014 Florian Behrens
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DECOF_CLI_PARSER_H
#define DECOF_CLI_PARSER_H

#include "scanner.h"

#ifdef _MSC_VER
// Undefine the preprocessor define ERROR defined within Windows header files
#ifdef ERROR
#undef ERROR
#endif
#endif

#include "parserbase.h"

// $insert namespace-open
namespace decof
{

namespace cli
{

#undef parser
class parser: public parserBase
{        
public:
    parser(std::istream &in = std::cin);

    int parse();
    boost::any result() const;

private:
    void error(char const *msg);    // called on (syntax) errors
    int lex();                      // returns the next token from the
                                    // lexical scanner.
    void print();                   // use, e.g., d_token, d_loc

    // Support functions for parse():
    void executeAction(int ruleNr);
    void errorRecovery();
    int lookup(bool recovery);
    void nextToken();
    void print__();
    void exceptionHandler__(std::exception const &);

    scanner d_scanner;
    boost::any result_;
};

// $insert namespace-close
}

}

#endif // DECOF_CLI_PARSER_H
