# Pawn.Regex
🔎 Plugin that adds support for regular expressions in Pawn

Why is it better than others plugins? Because it gives you an opportunity to get match groups.

## Natives
```pawn
native Regex:Regex_New(const pattern[], E_REGEX_FLAG:flags = REGEX_DEFAULT, E_REGEX_GRAMMAR:grammar = REGEX_ECMASCRIPT);
native Regex_Delete(&Regex:r);

native Regex_Check(const str[], Regex:r, E_MATCH_FLAG:flags = MATCH_DEFAULT);
native Regex_Match(const str[], Regex:r, &RegexMatch:m, E_MATCH_FLAG:flags = MATCH_DEFAULT);
native Regex_Search(const str[], Regex:r, &RegexMatch:m, &pos, startpos = 0, E_MATCH_FLAG:flags = MATCH_DEFAULT);
native Regex_Replace(const str[], Regex:r, const fmt[], dest[], E_MATCH_FLAG:flags = MATCH_DEFAULT, size = sizeof dest);

native Match_GetGroup(RegexMatch:m, index, dest[], &length, size = sizeof dest);
native Match_Free(&RegexMatch:m);
```

## Examples
```pawn
#include <Pawn.Regex>

stock IsRpNickname(const nickname[])
{
  static Regex:regex;
  if (!regex) regex = Regex_New("[A-Z][a-z]+_[A-Z][a-z]+");

  return Regex_Check(nickname, regex);
}

stock IsValidEmail(const email[])
{
  static Regex:regex;
  if (!regex) regex = Regex_New("^[\\w-\\.]+@([\\w-]+\\.)+[\\w-]{2,4}$");

  return Regex_Check(email, regex);
}

public OnPlayerCommandText(playerid, cmdtext[])
{
  static Regex:regex;
  if (!regex) regex = Regex_New("^\\/([\\w]+)\\s*(.+?)?\\s*$");

  new RegexMatch:match;
  if (!Regex_Match(cmdtext, regex, match)) return 0;

  new cmd[16], cmd_length;
  Match_GetGroup(match, 1, cmd, cmd_length);

  new params[256], params_length;
  Match_GetGroup(match, 2, params, params_length);

  printf("cmd '%s' (len %d), params '%s' (len %d)", cmd, cmd_length, params, params_length);

  Match_Free(match);

  return 1;
}

stock SplitAndPrint(const str[])
{
  static Regex:regex;
  if (!regex) regex = Regex_New("[^\\s]+");

  new RegexMatch:match, pos, startpos;
  while (Regex_Search(str, regex, match, pos, startpos))
  {
    new word[128], length;
    Match_GetGroup(match, 0, word, length);

    printf("word: %s, len: %d", word, length);

    startpos += pos + length;

    Match_Free(match);
  }
}

stock ReplaceString(const str[], const regexp[], const fmt[], dest[], size = sizeof dest)
{
  new Regex:regex = Regex_New(regexp);
  if (!regex) return;

  Regex_Replace(str, regex, fmt, dest, MATCH_DEFAULT, size);

  Regex_Delete(regex);
}

main()
{
  new str[256];

  ReplaceString("Regex.Pawn", "(.+)\\.(.+)", "$1.$2 => $2.$1", str);

  printf("%s", str);

  SplitAndPrint("4 8 15 16 23 42");

  OnPlayerCommandText(-1, "/ban 42");
  OnPlayerCommandText(-1, "/kill");

  printf("%d %d %d %d", IsRpNickname("Firstname_Lastname"), IsRpNickname("katursis"), IsValidEmail("katursis@example.com"), IsValidEmail("email.example.com"));
}
```
