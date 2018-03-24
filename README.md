# Pawn.Regex
Pawn.Regex - a plugin that allows you to work with regular expressions.

Why is it better than others? Because it gives you an opportunity to get results of parsing, not just true/false if there is a match.

## Natives
```pawn
native regex:regex_new(const pattern[], E_REGEX_FLAG:flags = REGEX_DEFAULT, E_REGEX_GRAMMAR:grammar = REGEX_ECMASCRIPT);
native regex_delete(&regex:r);

native regex_check(const str[], regex:r, E_MATCH_FLAG:flags = MATCH_DEFAULT);
native regex_match(const str[], regex:r, &match_results:m, E_MATCH_FLAG:flags = MATCH_DEFAULT);
native regex_search(const str[], regex:r, &match_results:m, &pos, startpos = 0, E_MATCH_FLAG:flags = MATCH_DEFAULT);
native regex_replace(const str[], regex:r, const fmt[], dest[], E_MATCH_FLAG:flags = MATCH_DEFAULT, size = sizeof dest);

native match_get_group(match_results:m, index, dest[], &length, size = sizeof dest);
native match_free(&match_results:m);
```

## Examples
```pawn
#include <Pawn.Regex>

stock IsRpNickname(nickname[])
{
    new regex:r = regex_new("[A-Z][a-z]+_[A-Z][a-z]+");

    new check = regex_check(nickname, r);

    regex_delete(r);

    return check;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    new
        regex:r = regex_new("^\\/([\\w]+)\\s*(.+?)?\\s*$"),
        match_results:m,
        cmd[16], params[256],
        cmd_length, params_length;

    if (regex_match(cmdtext, r, m))
    {
        match_get_group(m, 1, cmd, cmd_length);
        match_get_group(m, 2, params, params_length);

        match_free(m);
    }

    printf("cmd '%s' (%d), params '%s' (%d)", cmd, cmd_length, params, params_length);

    regex_delete(r);

    return 1;
}

stock SplitAndPrint(str[])
{
    new regex:r = regex_new("[^\\s]+");

    if (r)
    {
        new match_results:m;
        new startpos, pos;

        while (regex_search(str, r, m, pos, startpos))
        {
            new word[128], length;

            if (!match_get_group(m, 0, word, length))
            {
                break;
            }

            printf("word: %s", word);

            startpos += pos + length;

            match_free(m);
        }

        regex_delete(r);
    }
}

stock ReplaceString(const str[], const regexp[], const fmt[], dest[], size = sizeof dest)
{
    new regex:r = regex_new(regexp);

    if (r)
    {
        regex_replace(str, r, fmt, dest, MATCH_DEFAULT, size);

        regex_delete(r);
    }
}

main()
{
    new str[128];

    ReplaceString("Pawn.CMD", "CMD", "Regex", str);

    printf("%s", str);

    SplitAndPrint("4 8 15 16 23 42");

    OnPlayerCommandText(-1, "/ban 42");
    OnPlayerCommandText(-1, "/kill");

    printf("%d %d", IsRpNickname("Your_Shadow"), IsRpNickname("urShadow"));
}
```
