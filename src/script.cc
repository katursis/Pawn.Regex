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

cell Script::NewRegex(const std::string &pattern,
                      std::regex_constants::syntax_option_type option) {
  const auto regex = std::make_shared<std::regex>();

  regex->imbue(Plugin::Instance().GetLocale());

  regex->assign(pattern, option);

  regexes_.insert(regex);

  return reinterpret_cast<cell>(regex.get());
}

const RegexPtr &Script::GetRegex(cell ptr) {
  const auto iter =
      std::find_if(regexes_.begin(), regexes_.end(), [ptr](const RegexPtr &p) {
        return p.get() == reinterpret_cast<void *>(ptr);
      });

  if (iter == regexes_.end()) {
    throw std::runtime_error{"Invalid regex handle"};
  }

  return *iter;
}

void Script::DeleteRegex(cell regex) { regexes_.erase(GetRegex(regex)); }

cell Script::NewMatchResults(const std::smatch &match) {
  auto match_results = std::make_shared<MatchResults>();

  for (const auto &item : match) {
    match_results->push_back(item.str());
  }

  match_results_.insert(match_results);

  return reinterpret_cast<cell>(match_results.get());
}

const MatchResultsPtr &Script::GetMatchResults(cell ptr) {
  const auto iter =
      std::find_if(match_results_.begin(), match_results_.end(),
                   [ptr](const MatchResultsPtr &p) {
                     return p.get() == reinterpret_cast<void *>(ptr);
                   });

  if (iter == match_results_.end()) {
    throw std::runtime_error{"Invalid match_results handle"};
  }

  return *iter;
}

void Script::DeleteMatchResults(cell match_results) {
  match_results_.erase(GetMatchResults(match_results));
}

std::regex_constants::syntax_option_type Script::GetRegexFlag(
    E_REGEX_FLAG flags, E_REGEX_GRAMMAR grammar) {
  const static std::unordered_map<std::size_t,
                                  std::regex_constants::syntax_option_type>
      grammar_map{
          {REGEX_ECMASCRIPT, std::regex_constants::ECMAScript},
          {REGEX_BASIC, std::regex_constants::basic},
          {REGEX_EXTENDED, std::regex_constants::extended},
          {REGEX_AWK, std::regex_constants::awk},
          {REGEX_GREP, std::regex_constants::grep},
          {REGEX_EGREP, std::regex_constants::egrep},
      };

  const static std::unordered_map<std::size_t,
                                  std::regex_constants::syntax_option_type>
      syntax_option_map{
          {REGEX_ICASE, std::regex_constants::icase},
          {REGEX_NOSUBS, std::regex_constants::nosubs},
          {REGEX_OPTIMIZE, std::regex_constants::optimize},
          {REGEX_COLLATE, std::regex_constants::collate},
      };

  auto flag = grammar_map.at(grammar);

  for (auto item : syntax_option_map) {
    if (flags & item.first) {
      flag |= item.second;
    }
  }

  return flag;
}

std::regex_constants::match_flag_type Script::GetMatchFlag(E_MATCH_FLAG flags) {
  const static std::unordered_map<std::size_t,
                                  std::regex_constants::match_flag_type>
      match_flag_map{
          {MATCH_DEFAULT, std::regex_constants::match_default},
          {MATCH_NOT_BOL, std::regex_constants::match_not_bol},
          {MATCH_NOT_EOL, std::regex_constants::match_not_eol},
          {MATCH_NOT_BOW, std::regex_constants::match_not_bow},
          {MATCH_NOT_EOW, std::regex_constants::match_not_eow},
          {MATCH_ANY, std::regex_constants::match_any},
          {MATCH_NOT_NULL, std::regex_constants::match_not_null},
          {MATCH_CONTINUOUS, std::regex_constants::match_continuous},
          {MATCH_PREV_AVAIL, std::regex_constants::match_prev_avail},
          {MATCH_FORMAT_SED, std::regex_constants::format_sed},
          {MATCH_FORMAT_NO_COPY, std::regex_constants::format_no_copy},
          {MATCH_FORMAT_FIRST_ONLY, std::regex_constants::format_first_only},
      };

  std::regex_constants::match_flag_type flag{};

  for (auto item : match_flag_map) {
    if (flags & item.first) {
      flag |= item.second;
    }
  }

  return flag;
}

// native Regex:Regex_New(const pattern[], E_REGEX_FLAG:flags = REGEX_DEFAULT,
// E_REGEX_GRAMMAR:grammar = REGEX_ECMASCRIPT);
cell Script::Regex_New(std::string pattern, E_REGEX_FLAG flags,
                       E_REGEX_GRAMMAR grammar) {
  return NewRegex(pattern, GetRegexFlag(flags, grammar));
}

// native Regex_Delete(&Regex:r);
cell Script::Regex_Delete(cell *regex) {
  DeleteRegex(*regex);

  *regex = 0;

  return 1;
}

// native Regex_Check(const str[], Regex:r, E_MATCH_FLAG:flags = MATCH_DEFAULT);
cell Script::Regex_Check(std::string str, RegexPtr regex, E_MATCH_FLAG flags) {
  return std::regex_match(str, *regex, GetMatchFlag(flags)) ? 1 : 0;
}

// native Regex_Match(const str[], Regex:r, &RegexMatch:m, E_MATCH_FLAG:flags =
// MATCH_DEFAULT);
cell Script::Regex_Match(std::string str, RegexPtr regex, cell *match_results,
                         E_MATCH_FLAG flags) {
  std::smatch results;
  if (std::regex_match(str, results, *regex, GetMatchFlag(flags))) {
    *match_results = NewMatchResults(results);

    return 1;
  }

  return 0;
}

// native Regex_Search(const str[], Regex:r, &RegexMatch:m, &pos, startpos = 0,
// E_MATCH_FLAG:flags = MATCH_DEFAULT);
cell Script::Regex_Search(std::string str, RegexPtr regex, cell *match_results,
                          cell *pos, cell startpos, E_MATCH_FLAG flags) {
  str = str.substr(startpos);

  std::smatch results;
  if (std::regex_search(str, results, *regex, GetMatchFlag(flags))) {
    *match_results = NewMatchResults(results);

    *pos = results.position();

    return 1;
  }

  return 0;
}

// native Regex_Replace(const str[], Regex:r, const fmt[], dest[],
// E_MATCH_FLAG:flags = MATCH_DEFAULT, size = sizeof dest);
cell Script::Regex_Replace(std::string str, RegexPtr regex, std::string fmt,
                           cell *dest, E_MATCH_FLAG flags, cell size) {
  const auto result = std::regex_replace(str, *regex, fmt, GetMatchFlag(flags));

  SetString(dest, result, size);

  return 1;
}

// native Match_GetGroup(RegexMatch:m, index, dest[], &length, size = sizeof
// dest);
cell Script::Match_GetGroup(MatchResultsPtr match_results, cell index,
                            cell *dest, cell *length, cell size) {
  const auto str = match_results->at(index);

  SetString(dest, str, size);

  *length = str.length();

  return 1;
}

// native Match_Free(&RegexMatch:m);
cell Script::Match_Free(cell *match_results) {
  DeleteMatchResults(*match_results);

  *match_results = 0;

  return 1;
}
