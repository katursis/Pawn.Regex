// Pawn.Regex plugin by urShadow
 
#if !defined __cplusplus
	#if defined _pawnregex_included
	  #endinput
	#endif
	#define _pawnregex_included	
#endif

#define PAWNREGEX_INCLUDE_VERSION 112

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
	public _pawnregex_version = PAWNREGEX_INCLUDE_VERSION;
	#pragma unused _pawnregex_version

	native Regex:Regex_New(const pattern[], E_REGEX_FLAG:flags = REGEX_DEFAULT, E_REGEX_GRAMMAR:grammar = REGEX_ECMASCRIPT) = regex_new;
	native Regex_Delete(&Regex:r) = regex_delete;

	native Regex_Check(const str[], Regex:r, E_MATCH_FLAG:flags = MATCH_DEFAULT) = regex_check;
	native Regex_Match(const str[], Regex:r, &RegexMatch:m, E_MATCH_FLAG:flags = MATCH_DEFAULT) = regex_match;
	native Regex_Search(const str[], Regex:r, &RegexMatch:m, &pos, startpos = 0, E_MATCH_FLAG:flags = MATCH_DEFAULT) = regex_search;
	native Regex_Replace(const str[], Regex:r, const fmt[], dest[], E_MATCH_FLAG:flags = MATCH_DEFAULT, size = sizeof dest) = regex_replace;

	native Match_GetGroup(RegexMatch:m, index, dest[], &length, size = sizeof dest) = match_get_group;
	native Match_Free(&RegexMatch:m) = match_free;

	#pragma deprecated
	native regex:regex_new(const pattern[], E_REGEX_FLAG:flags = REGEX_DEFAULT, E_REGEX_GRAMMAR:grammar = REGEX_ECMASCRIPT);
	#pragma deprecated
	native regex_delete(&regex:r);

	#pragma deprecated
	native regex_check(const str[], regex:r, E_MATCH_FLAG:flags = MATCH_DEFAULT);
	#pragma deprecated
	native regex_match(const str[], regex:r, &match_results:m, E_MATCH_FLAG:flags = MATCH_DEFAULT);
	#pragma deprecated
	native regex_search(const str[], regex:r, &match_results:m, &pos, startpos = 0, E_MATCH_FLAG:flags = MATCH_DEFAULT);
	#pragma deprecated
	native regex_replace(const str[], regex:r, const fmt[], dest[], E_MATCH_FLAG:flags = MATCH_DEFAULT, size = sizeof dest);

	#pragma deprecated
	native match_get_group(match_results:m, index, dest[], &length, size = sizeof dest);
	#pragma deprecated
	native match_free(&match_results:m);
#endif
