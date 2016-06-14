# Pawn.Regex
Pawn.Regex - it is a plugin that allows you to work with regular expressions. 
Why he is better than others? Because it allows you to get results of a parsing.

## Natives
```pawn
native regex:regex_new(const pattern[], E_REGEX_FLAG:flags = REGEX_DEFAULT, E_REGEX_GRAMMAR:grammar = REGEX_ECMASCRIPT);
native regex_delete(&regex:r);

native regex_check(const str[], regex:r, E_MATCH_FLAG:flags = MATCH_DEFAULT);
native regex_match(const str[], regex:r, &match_results:m, E_MATCH_FLAG:flags = MATCH_DEFAULT);
native regex_search(const str[], regex:r, &match_results:m, &pos, startpos = 0, E_MATCH_FLAG:flags = MATCH_DEFAULT);
native regex_replace(const str[], regex:r, const fmt[], dest[], E_MATCH_FLAG:flags = MATCH_DEFAULT, size = sizeof dest);

native match_get_group(match_results:m, index, dest[], size = sizeof dest);
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
		regex:r_with_param = regex_new("\\/\\s*(\\w{1,15})\\s+(.+)"),
		match_results:m,
		cmd[16], params[256];

	if (regex_match(cmdtext, r_with_param, m))
	{
		match_get_group(m, 1, cmd);
		match_get_group(m, 2, params);

		match_free(m);
	}
	else
	{
		new regex:r = regex_new("\\/\\s*(\\w{1,15})\\s*");

		if (regex_match(cmdtext, r, m))
		{
			match_get_group(m, 1, cmd);

			match_free(m);
		}

		regex_delete(r);
	}

	printf("cmd '%s', params '%s'", cmd, params);

	regex_delete(r_with_param);

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
			new word[128];

			match_get_group(m, 0, word);

			printf("word: %s", word);

			startpos += pos + strlen(word);

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

	SplitAndPrint("1 2 3 	4 5 6	7 8 9");
    
	OnPlayerCommandText(-1, "/ban 42");
	OnPlayerCommandText(-1, "/kill");
    
	printf("%d %d",
					IsRpNickname("Your_Shadow"),
					IsRpNickname("urShadow"));
}
```
