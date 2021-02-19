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
    new Regex:r = Regex_New("[A-Z][a-z]+_[A-Z][a-z]+");

    new check = Regex_Check(nickname, r);

    Regex_Delete(r);

    return check;
}

stock IsValidEmail(const email[])
{
    new Regex:r = Regex_New("^[\\w-\\.]+@([\\w-]+\\.)+[\\w-]{2,4}$");
    
    new check = Regex_Check(email, r);
    
    Regex_Delete(r);
    
    return check;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    new
        Regex:r = Regex_New("^\\/([\\w]+)\\s*(.+?)?\\s*$"),
        RegexMatch:m,
        cmd[16], params[256],
        cmd_length, params_length;

    if (Regex_Match(cmdtext, r, m))
    {
        Match_GetGroup(m, 1, cmd, cmd_length);
        Match_GetGroup(m, 2, params, params_length);

        Match_Free(m);
    }

    printf("cmd '%s' (%d), params '%s' (%d)", cmd, cmd_length, params, params_length);

    Regex_Delete(r);

    return 1;
}

stock SplitAndPrint(str[])
{
    new Regex:r = Regex_New("[^\\s]+");

    if (r)
    {
        new RegexMatch:m;
        new startpos, pos;

        while (Regex_Search(str, r, m, pos, startpos))
        {
            new word[128], length;

            if (!Match_GetGroup(m, 0, word, length))
            {
                break;
            }

            printf("word: %s, length: %d", word, length);

            startpos += pos + length;

            Match_Free(m);
        }

        Regex_Delete(r);
    }
}

stock ReplaceString(const str[], const regexp[], const fmt[], dest[], size = sizeof dest)
{
    new Regex:r = Regex_New(regexp);

    if (r)
    {
        Regex_Replace(str, r, fmt, dest, MATCH_DEFAULT, size);

        Regex_Delete(r);
    }
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
