/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 katursis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef PAWNREGEX_SCRIPT_H_
#define PAWNREGEX_SCRIPT_H_

using RegexPtr = std::shared_ptr<std::regex>;
using MatchResults = std::vector<std::string>;
using MatchResultsPtr = std::shared_ptr<MatchResults>;

class Script : public ptl::AbstractScript<Script> {
 public:
  const char *VarVersion() { return "_pawnregex_version"; }

  cell NewRegex(const std::string &pattern,
                std::regex_constants::syntax_option_type option);
  const RegexPtr &GetRegex(cell ptr);
  void DeleteRegex(cell regex);

  cell NewMatchResults(const std::smatch &match);
  const MatchResultsPtr &GetMatchResults(cell ptr);
  void DeleteMatchResults(cell match_results);

  std::regex_constants::syntax_option_type GetRegexFlag(
      E_REGEX_FLAG flags, E_REGEX_GRAMMAR grammar);
  std::regex_constants::match_flag_type GetMatchFlag(E_MATCH_FLAG flags);

 private:
  std::unordered_set<RegexPtr> regexes_;
  std::unordered_set<MatchResultsPtr> match_results_;
};

#endif  // PAWNREGEX_SCRIPT_H_
