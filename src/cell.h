/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2020 urShadow
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

#ifndef PAWNREGEX_CELL_H_
#define PAWNREGEX_CELL_H_

class Cell : public Script::Cell {
 public:
  operator E_REGEX_FLAG() { return static_cast<E_REGEX_FLAG>(amx_addr_); }

  operator E_REGEX_GRAMMAR() { return static_cast<E_REGEX_GRAMMAR>(amx_addr_); }

  operator E_MATCH_FLAG() { return static_cast<E_MATCH_FLAG>(amx_addr_); };

  operator RegexPtr() { return script_->GetRegex(amx_addr_); }

  operator MatchResultsPtr() { return script_->GetMatchResults(amx_addr_); };
};

#endif  // PAWNREGEX_CELL_H_
