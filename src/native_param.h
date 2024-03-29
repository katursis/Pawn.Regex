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

#ifndef PAWNREGEX_NATIVE_PARAM_H_
#define PAWNREGEX_NATIVE_PARAM_H_

struct NativeParam : Script::NativeParam {
  operator E_REGEX_FLAG() { return static_cast<E_REGEX_FLAG>(raw_value); }

  operator E_REGEX_GRAMMAR() { return static_cast<E_REGEX_GRAMMAR>(raw_value); }

  operator E_MATCH_FLAG() { return static_cast<E_MATCH_FLAG>(raw_value); };

  operator RegexPtr() { return script.GetRegex(raw_value); }

  operator MatchResultsPtr() { return script.GetMatchResults(raw_value); };
};

#endif  // PAWNREGEX_NATIVE_PARAM_H_
