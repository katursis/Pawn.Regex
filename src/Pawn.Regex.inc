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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if !defined PAWNREGEX_INC_
    #define PAWNREGEX_INC_
    #define _pawnregex_included // backward compatibility

    #if !defined PACK_PLUGIN_VERSION
        #define PACK_PLUGIN_VERSION(%0,%1,%2) (((%0) << 16) | ((%1) << 8) | (%2))
    #endif

    #define PAWNREGEX_VERSION PACK_PLUGIN_VERSION(1, 2, 3)
    #define PAWNREGEX_INCLUDE_VERSION PAWNREGEX_VERSION // backward compatibility

    enum E_REGEX_GRAMMAR
    {
        REGEX_ECMASCRIPT, // ECMAScript grammar
        REGEX_BASIC, // Basic POSIX grammar
        REGEX_EXTENDED, // Extended POSIX grammar
        REGEX_AWK, // Awk POSIX grammar
        REGEX_GREP, // Grep POSIX grammar
        REGEX_EGREP // Egrep POSIX grammar
    };

    enum E_REGEX_FLAG
    {
        REGEX_DEFAULT = 1 << 0, // Default flag
        REGEX_ICASE = 1 << 1, // Regular expressions match without regard to case
        REGEX_NOSUBS = 1 << 2, // The match_results structure will not contain sub-expression matches.
        REGEX_OPTIMIZE = 1 << 3, // Matching efficiency is preferred over efficiency constructing regex objects.
        REGEX_COLLATE = 1 << 4, // Character ranges, like "[a-b]", are affected by locale.
    };

    enum E_MATCH_FLAG
    {
        MATCH_DEFAULT = 1 << 0, // Default matching behavior.
        MATCH_NOT_BOL = 1 << 1, // The first character is not considered a beginning of line ("^" does not match).
        MATCH_NOT_EOL = 1 << 2, // The last character is not considered an end of line ("$" does not match).
        MATCH_NOT_BOW = 1 << 3, // The escape sequence "\b" does not match as a beginning-of-word.
        MATCH_NOT_EOW = 1 << 4, // The escape sequence "\b" does not match as an end-of-word.
        MATCH_ANY = 1 << 5, // Any match is acceptable if more than one match is possible.
        MATCH_NOT_NULL = 1 << 6, // Empty sequences do not match.
        MATCH_CONTINUOUS = 1 << 7, // The expression must match a sub-sequence that begins at the first character.
                                // Sub-sequences must begin at the first character to match.
        MATCH_PREV_AVAIL = 1 << 8, // One or more characters exist before the first one. (MATCH_NOT_BOL and MATCH_NOT_BOW are ignored)
        MATCH_FORMAT_SED = 1 << 9, // Uses the same rules as the sed utility in POSIX to replace matches.
        MATCH_FORMAT_NO_COPY = 1 << 10, // The sections in the target sequence that do not match the regular expression
                                        // are not copied when replacing matches.
        MATCH_FORMAT_FIRST_ONLY = 1 << 11, // Only the first occurrence of a regular expression is replaced.
    };

    #if !defined __cplusplus
        public _pawnregex_version = PAWNREGEX_VERSION;
        #pragma unused _pawnregex_version

        native Regex:Regex_New(const pattern[], E_REGEX_FLAG:flags = REGEX_DEFAULT, E_REGEX_GRAMMAR:grammar = REGEX_ECMASCRIPT);
        native Regex_Delete(&Regex:r);

        native Regex_Check(const str[], Regex:r, E_MATCH_FLAG:flags = MATCH_DEFAULT);
        native Regex_Match(const str[], Regex:r, &RegexMatch:m, E_MATCH_FLAG:flags = MATCH_DEFAULT);
        native Regex_Search(const str[], Regex:r, &RegexMatch:m, &pos, startpos = 0, E_MATCH_FLAG:flags = MATCH_DEFAULT);
        native Regex_Replace(const str[], Regex:r, const fmt[], dest[], E_MATCH_FLAG:flags = MATCH_DEFAULT, size = sizeof dest);

        native Match_GetGroup(RegexMatch:m, index, dest[], &length, size = sizeof dest);
        native Match_Free(&RegexMatch:m);

        #pragma deprecated Use Regex_New instead
        native regex:regex_new(const pattern[], E_REGEX_FLAG:flags = REGEX_DEFAULT, E_REGEX_GRAMMAR:grammar = REGEX_ECMASCRIPT) = Regex_New;
        #pragma deprecated Use Regex_Delete instead
        native regex_delete(&regex:r) = Regex_Delete;

        #pragma deprecated Use Regex_Check instead
        native regex_check(const str[], regex:r, E_MATCH_FLAG:flags = MATCH_DEFAULT) = Regex_Check;
        #pragma deprecated Use Regex_Match instead
        native regex_match(const str[], regex:r, &match_results:m, E_MATCH_FLAG:flags = MATCH_DEFAULT) = Regex_Match;
        #pragma deprecated Use Regex_Search instead
        native regex_search(const str[], regex:r, &match_results:m, &pos, startpos = 0, E_MATCH_FLAG:flags = MATCH_DEFAULT) = Regex_Search;
        #pragma deprecated Use Regex_Replace instead
        native regex_replace(const str[], regex:r, const fmt[], dest[], E_MATCH_FLAG:flags = MATCH_DEFAULT, size = sizeof dest) = Regex_Replace;

        #pragma deprecated Use Match_GetGroup instead
        native match_get_group(match_results:m, index, dest[], &length, size = sizeof dest) = Match_GetGroup;
        #pragma deprecated Use Match_Free instead
        native match_free(&match_results:m) = Match_Free;
    #endif // !defined __cplusplus
#endif // !defined PAWNREGEX_INC_
