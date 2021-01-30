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

#include "main.h"

// native Regex:Regex_New(const pattern[], E_REGEX_FLAG:flags = REGEX_DEFAULT,
// E_REGEX_GRAMMAR:grammar = REGEX_ECMASCRIPT);
cell Natives::Regex_New(Script *script, std::string pattern, E_REGEX_FLAG flags,
                        E_REGEX_GRAMMAR grammar) {
  return script->NewRegex(pattern, script->GetRegexFlag(flags, grammar));
}

// native Regex_Delete(&Regex:r);
cell Natives::Regex_Delete(Script *script, cell *regex) {
  script->DeleteRegex(*regex);

  *regex = 0;

  return 1;
}

// native Regex_Check(const str[], Regex:r, E_MATCH_FLAG:flags = MATCH_DEFAULT);
cell Natives::Regex_Check(Script *script, std::string str, RegexPtr regex,
                          E_MATCH_FLAG flags) {
  return std::regex_match(str, *regex, script->GetMatchFlag(flags)) ? 1 : 0;
}

// native Regex_Match(const str[], Regex:r, &RegexMatch:m, E_MATCH_FLAG:flags =
// MATCH_DEFAULT);
cell Natives::Regex_Match(Script *script, std::string str, RegexPtr regex,
                          cell *match_results, E_MATCH_FLAG flags) {
  std::smatch results;
  if (std::regex_match(str, results, *regex, script->GetMatchFlag(flags))) {
    *match_results = script->NewMatchResults(results);

    return 1;
  }

  return 0;
}

// native Regex_Search(const str[], Regex:r, &RegexMatch:m, &pos, startpos = 0,
// E_MATCH_FLAG:flags = MATCH_DEFAULT);
cell Natives::Regex_Search(Script *script, std::string str, RegexPtr regex,
                           cell *match_results, cell *pos, cell startpos,
                           E_MATCH_FLAG flags) {
  str = str.substr(startpos);

  std::smatch results;
  if (std::regex_search(str, results, *regex, script->GetMatchFlag(flags))) {
    *match_results = script->NewMatchResults(results);

    *pos = results.position();

    return 1;
  }

  return 0;
}

// native Regex_Replace(const str[], Regex:r, const fmt[], dest[],
// E_MATCH_FLAG:flags = MATCH_DEFAULT, size = sizeof dest);
cell Natives::Regex_Replace(Script *script, std::string str, RegexPtr regex,
                            std::string fmt, cell *dest, E_MATCH_FLAG flags,
                            cell size) {
  const auto result =
      std::regex_replace(str, *regex, fmt, script->GetMatchFlag(flags));

  script->SetString(dest, result, size);

  return 1;
}

// native Match_GetGroup(RegexMatch:m, index, dest[], &length, size = sizeof
// dest);
cell Natives::Match_GetGroup(Script *script, MatchResultsPtr match_results,
                             cell index, cell *dest, cell *length, cell size) {
  const auto str = match_results->at(index);

  script->SetString(dest, str, size);

  *length = str.length();

  return 1;
}

// native Match_Free(&RegexMatch:m);
cell Natives::Match_Free(Script *script, cell *match_results) {
  script->DeleteMatchResults(*match_results);

  *match_results = 0;

  return 1;
}
