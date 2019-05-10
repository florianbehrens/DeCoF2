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

#ifndef DECOF_CLI_SCANNER_H
#define DECOF_CLI_SCANNER_H

// $insert baseclass_h
#include "scannerbase.h"

// $insert namespace-open
namespace decof {

namespace cli {

// $insert classHead
class scanner : public scannerBase
{
  public:
    explicit scanner(std::istream& in = std::cin, std::ostream& out = std::cout);

    scanner(std::string const& infile, std::string const& outfile);

    // $insert lexFunctionDecl
    int lex();

  private:
    int lex__();
    int executeAction__(size_t ruleNr);

    void print();
    void preCode(); // re-implement this function for code that must
                    // be exec'ed before the patternmatching starts

    void postCode(PostEnum__);
    // re-implement this function for code that must
    // be exec'ed after the rules's actions.
};

// $insert scannerConstructors
inline scanner::scanner(std::istream& in, std::ostream& out) : scannerBase(in, out)
{
}

inline scanner::scanner(std::string const& infile, std::string const& outfile) : scannerBase(infile, outfile)
{
}

// $insert inlineLexFunction
inline int scanner::lex()
{
    return lex__();
}

inline void scanner::preCode()
{
    // optionally replace by your own code
}

inline void scanner::postCode(PostEnum__)
{
    // optionally replace by your own code
}

inline void scanner::print()
{
    print__();
}

// $insert namespace-close
} // namespace cli

} // namespace decof

#endif // DECOF_CLI_SCANNER_H
