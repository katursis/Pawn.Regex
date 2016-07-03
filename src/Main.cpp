#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

#include <unordered_set>
#include <vector>
#include <memory>
#include <regex>

#include "Pawn.Regex.inc"

using logprintf_t = void(*)(char* format, ...);

logprintf_t logprintf;

extern void *pAMXFunctions;

class Plugin
{
public:

	static constexpr char
		*kName = "Pawn.Regex",
		*kVersion = "1.1",
		*kPublicVarName = "_pawnregex_version";

	static bool Load(void)
	{
		return logprintf("%s plugin v%s by urShadow loaded", kName, kVersion), true;
	}

	static void Unload(void)
	{
		_regex_set.clear();

		_match_results_set.clear();

		logprintf("%s plugin v%s by urShadow unloaded", kName, kVersion);
	}

	static void AmxLoad(AMX *amx)
	{
		const static std::vector<AMX_NATIVE_INFO> native_vec =
		{
			{ "regex_new", &n_regex_new },
			{ "regex_delete", &n_regex_delete },

			{ "regex_check", &n_regex_check },
			{ "regex_match", &n_regex_match },
			{ "regex_search", &n_regex_search },
			{ "regex_replace", &n_regex_replace },

			{ "match_get_group", &n_match_get_group },
			{ "match_free", &n_match_free }
		};

		cell addr{}, *phys_addr{};
		if (!amx_FindPubVar(amx, kPublicVarName, &addr) &&
			!amx_GetAddr(amx, addr, &phys_addr))
		{
			if (*phys_addr != PAWNREGEX_INCLUDE_VERSION)
				return logprintf("[%s] %s: .inc-file version does not equal the plugin's version", kName, __FUNCTION__);

			amx_Register(amx, native_vec.data(), native_vec.size());
		}
	}

private:

	using Regex = std::shared_ptr<std::regex>;
	using RegexSet = std::unordered_set<Regex>;
	using MatchResults = std::shared_ptr<std::vector<std::string>>;
	using MatchResultsSet = std::unordered_set<MatchResults>;

	// native regex:regex_new(const pattern[], E_REGEX_FLAG:flags = REGEX_DEFAULT, E_REGEX_GRAMMAR:grammar = REGEX_ECMASCRIPT);
	static cell AMX_NATIVE_CALL n_regex_new(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 3, params))
			return 0;

		const std::unique_ptr<char[]> pattern(get_string(amx, params[1]));

		if (!pattern)
			return logprintf("[%s] %s: invalid pattern", kName, __FUNCTION__), 0;

		const auto option = get_regex_flag(
			static_cast<E_REGEX_FLAG>(params[2]),
			static_cast<E_REGEX_GRAMMAR>(params[3]));

		cell result{};

		try
		{
			const auto regex = std::make_shared<std::regex>(pattern.get(), option);

			_regex_set.insert(regex);

			result = reinterpret_cast<cell>(regex.get());
		}
		catch (const std::exception &e)
		{
			logprintf("[%s] %s: %s", kName, __FUNCTION__, e.what());
		}

		return result;
	}

	// native regex_delete(&regex:r);
	static cell AMX_NATIVE_CALL n_regex_delete(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 1, params))
			return 0;

		cell *cptr{};

		if (amx_GetAddr(amx, params[1], &cptr) != AMX_ERR_NONE)
			return logprintf("[%s] %s: invalid param reference", kName, __FUNCTION__), 0;

		const auto regex = get_regex(*cptr);

		if (!regex)
			return logprintf("[%s] %s: invalid regex handle", kName, __FUNCTION__), 0;

		_regex_set.erase(regex);

		*cptr = 0;

		return 1;
	}

	// native regex_check(const str[], regex:r, E_MATCH_FLAG:flags = MATCH_DEFAULT);
	static cell AMX_NATIVE_CALL n_regex_check(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 3, params))
			return 0;

		const std::unique_ptr<char[]> str(get_string(amx, params[1]));

		if (!str)
			return logprintf("[%s] %s: invalid str", kName, __FUNCTION__), 0;

		const auto regex = get_regex(params[2]);

		if (!regex)
			return logprintf("[%s] %s: invalid regex handle", kName, __FUNCTION__), 0;

		const auto flag = get_match_flag(static_cast<E_MATCH_FLAG>(params[3]));

		cell result{};

		try
		{
			result = static_cast<cell>(std::regex_match(str.get(), *regex, flag));
		}
		catch (const std::exception &e)
		{
			logprintf("[%s] %s: %s", kName, __FUNCTION__, e.what());
		}

		return result;
	}

	// native regex_match(const str[], regex:r, &match_results:m, E_MATCH_FLAG:flags = MATCH_DEFAULT);
	static cell AMX_NATIVE_CALL n_regex_match(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 4, params))
			return 0;

		const std::unique_ptr<char[]> str(get_string(amx, params[1]));

		if (!str)
			return logprintf("[%s] %s: invalid str", kName, __FUNCTION__), 0;

		const auto regex = get_regex(params[2]);

		if (!regex)
			return logprintf("[%s] %s: invalid regex handle", kName, __FUNCTION__), 0;

		cell result{};

		try
		{
			const auto m = std::make_shared<std::vector<std::string>>();

			const auto flag = get_match_flag(static_cast<E_MATCH_FLAG>(params[4]));

			std::cmatch results;

			if (std::regex_match(str.get(), results, *regex, flag))
			{
				for (auto &result : results)
					m->push_back(result.str());

				_match_results_set.insert(m);

				cell *addr{};

				if (amx_GetAddr(amx, params[3], &addr) == AMX_ERR_NONE)
					*addr = reinterpret_cast<cell>(m.get());

				result = 1;
			}
		}
		catch (const std::exception &e)
		{
			logprintf("[%s] %s: %s", kName, __FUNCTION__, e.what());
		}

		return result;
	}

	// native regex_search(const str[], regex:r, &match_results:m, &pos, startpos = 0, E_MATCH_FLAG:flags = MATCH_DEFAULT);
	static cell AMX_NATIVE_CALL n_regex_search(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 6, params))
			return 0;

		const std::unique_ptr<char[]> str(get_string(amx, params[1]));

		if (!str)
			return logprintf("[%s] %s: invalid str", kName, __FUNCTION__), 0;

		const std::string s(str.get());

		const auto regex = get_regex(params[2]);

		if (!regex)
			return logprintf("[%s] %s: invalid regex handle", kName, __FUNCTION__), 0;

		cell result{};

		try
		{
			const auto m = std::make_shared<std::vector<std::string>>();

			const auto startpos = static_cast<size_t>(params[5]);

			const auto flag = get_match_flag(static_cast<E_MATCH_FLAG>(params[6]));

			std::cmatch results;

			if (std::regex_search(s.substr(startpos).c_str(), results, *regex, flag))
			{
				for (auto &result : results)
					m->push_back(result.str());

				_match_results_set.insert(m);

				cell *addr{};

				if (amx_GetAddr(amx, params[4], &addr) == AMX_ERR_NONE)
					*addr = results.position();

				if (amx_GetAddr(amx, params[3], &addr) == AMX_ERR_NONE)
					*addr = reinterpret_cast<cell>(m.get());

				result = 1;
			}
		}
		catch (const std::exception &e)
		{
			logprintf("[%s] %s: %s", kName, __FUNCTION__, e.what());
		}

		return result;
	}

	// native regex_replace(const str[], regex:r, const fmt[], dest[], E_MATCH_FLAG:flags = MATCH_DEFAULT, size = sizeof dest);
	static cell AMX_NATIVE_CALL n_regex_replace(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 6, params))
			return 0;

		const std::unique_ptr<char[]>
			str(get_string(amx, params[1])),
			fmt(get_string(amx, params[3]));

		if ((!str) || (!fmt))
			return logprintf("[%s] %s: invalid str or fmt", kName, __FUNCTION__), 0;

		const auto regex = get_regex(params[2]);

		if (!regex)
			return logprintf("[%s] %s: invalid regex handle", kName, __FUNCTION__), 0;

		cell result{};

		try
		{
			const auto flag = get_match_flag(static_cast<E_MATCH_FLAG>(params[5]));

			const auto size = static_cast<size_t>(params[6]);

			const auto dest = std::regex_replace(str.get(), *regex, fmt.get(), flag);

			set_amxstring(amx, params[4], dest.c_str(), size);

			result = 1;
		}
		catch (const std::exception &e)
		{
			logprintf("[%s] %s: %s", kName, __FUNCTION__, e.what());
		}

		return result;
	}

	// native match_get_group(match_results:m, index, dest[], &length, size = sizeof dest);
	static cell AMX_NATIVE_CALL n_match_get_group(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 5, params))
			return 0;

		const auto match_results = get_match_results(params[1]);

		if (!match_results)
			return logprintf("[%s] %s: invalid match_results handle", kName, __FUNCTION__), 0;

		const auto
			index = static_cast<size_t>(params[2]),
			size = static_cast<size_t>(params[5]);

		cell result{};

		try
		{
			const auto str = match_results->at(index);

			set_amxstring(amx, params[3], str.c_str(), size);

			cell *addr{};

			if (amx_GetAddr(amx, params[4], &addr) == AMX_ERR_NONE)
				*addr = str.length();

			result = 1;
		}
		catch (const std::exception &e)
		{
			logprintf("[%s] %s: %s", kName, __FUNCTION__, e.what());
		}

		return result;
	}

	// native match_free(&match_results:m);
	static cell AMX_NATIVE_CALL n_match_free(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 1, params))
			return 0;

		cell *cptr{};

		if (amx_GetAddr(amx, params[1], &cptr) != AMX_ERR_NONE)
			return logprintf("[%s] %s: invalid param reference", kName, __FUNCTION__), 0;

		const auto match_results = get_match_results(*cptr);

		if (!match_results)
			return logprintf("[%s] %s: invalid match_results handle", kName, __FUNCTION__), 0;

		_match_results_set.erase(match_results);

		*cptr = 0;

		return 1;
	}

	static inline Regex get_regex(cell ptr)
	{
		auto iter = std::find_if(_regex_set.begin(), _regex_set.end(), [ptr](const Regex& p)
		{
			return p.get() == reinterpret_cast<void *>(ptr);
		});

		if (iter != _regex_set.end())
			return *iter;

		return nullptr;
	}

	static inline MatchResults get_match_results(cell ptr)
	{
		auto iter = std::find_if(_match_results_set.begin(), _match_results_set.end(), [ptr](const MatchResults& p)
		{
			return p.get() == reinterpret_cast<void *>(ptr);
		});

		if (iter != _match_results_set.end())
			return *iter;

		return nullptr;
	}

	static inline std::regex_constants::syntax_option_type get_regex_flag(E_REGEX_FLAG flags, E_REGEX_GRAMMAR grammar)
	{
		std::regex_constants::syntax_option_type flag{};

		switch (grammar)
		{
			case REGEX_ECMASCRIPT:
			default:
				flag = std::regex_constants::ECMAScript;
				break;
			case REGEX_BASIC:
				flag = std::regex_constants::basic;
				break;
			case REGEX_EXTENDED:
				flag = std::regex_constants::extended;
				break;
			case REGEX_AWK:
				flag = std::regex_constants::awk;
				break;
			case REGEX_GREP:
				flag = std::regex_constants::grep;
				break;
			case REGEX_EGREP:
				flag = std::regex_constants::egrep;
				break;
		}

		if (flags & REGEX_ICASE)
			flag |= std::regex_constants::icase;
		if (flags & REGEX_NOSUBS)
			flag |= std::regex_constants::nosubs;
		if (flags & REGEX_OPTIMIZE)
			flag |= std::regex_constants::optimize;
		if (flags & REGEX_COLLATE)
			flag |= std::regex_constants::collate;

		return flag;
	}

	static inline std::regex_constants::match_flag_type get_match_flag(E_MATCH_FLAG flags)
	{
		std::regex_constants::match_flag_type flag{};

		if (flags & MATCH_DEFAULT)
			flag |= std::regex_constants::match_default;
		if (flags & MATCH_NOT_BOL)
			flag |= std::regex_constants::match_not_bol;
		if (flags & MATCH_NOT_EOL)
			flag |= std::regex_constants::match_not_eol;
		if (flags & MATCH_NOT_BOW)
			flag |= std::regex_constants::match_not_bow;
		if (flags & MATCH_NOT_EOW)
			flag |= std::regex_constants::match_not_eow;
		if (flags & MATCH_ANY)
			flag |= std::regex_constants::match_any;
		if (flags & MATCH_NOT_NULL)
			flag |= std::regex_constants::match_not_null;
		if (flags & MATCH_CONTINUOUS)
			flag |= std::regex_constants::match_continuous;
		if (flags & MATCH_PREV_AVAIL)
			flag |= std::regex_constants::match_prev_avail;
		if (flags & MATCH_FORMAT_SED)
			flag |= std::regex_constants::format_sed;
		if (flags & MATCH_FORMAT_NO_COPY)
			flag |= std::regex_constants::format_no_copy;
		if (flags & MATCH_FORMAT_FIRST_ONLY)
			flag |= std::regex_constants::format_first_only;

		return flag;
	}

	static inline char *get_string(AMX *amx, cell amx_addr)
	{
		int	len{};
		cell *addr{};

		if (!amx_GetAddr(amx, amx_addr, &addr) &&
			!amx_StrLen(addr, &len) &&
			len)
		{
			len++;

			char *str = new (std::nothrow) char[len] {};

			if (str &&
				!amx_GetString(str, addr, 0, len))
				return str;
		}

		return nullptr;
	}

	static inline bool check_params(const char *native, int count, cell *params)
	{
		if (params[0] != (count * sizeof(cell)))
			return logprintf("[%s] %s: invalid number of parameters. Should be %d", kName, native, count), false;

		return true;
	}

	static inline int set_amxstring(AMX *amx, cell amx_addr, const char *source, int max)
	{
		cell *dest = reinterpret_cast<cell *>(amx->base +
			static_cast<int>(reinterpret_cast<AMX_HEADER *>(amx->base)->dat + amx_addr));

		cell *start = dest;

		while (max--&&*source)
			*dest++ = static_cast<cell>(*source++);

		*dest = 0;

		return dest - start;
	}

	static RegexSet _regex_set;

	static MatchResultsSet _match_results_set;
};

Plugin::RegexSet Plugin::_regex_set;

Plugin::MatchResultsSet Plugin::_match_results_set;

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];

	logprintf = reinterpret_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);

	return Plugin::Load();
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	Plugin::Unload();
}

PLUGIN_EXPORT void PLUGIN_CALL AmxLoad(AMX *amx)
{
	Plugin::AmxLoad(amx);
}