#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
#include "Pawn.Regex.inc"

#include <unordered_set>
#include <vector>
#include <memory>
#include <regex>

using logprintf_t = void(*)(char* format, ...);

logprintf_t logprintf;
extern void *pAMXFunctions;

class Plugin
{
public:

	static constexpr char
		*Name = "Pawn.Regex",
		*Version = "1.0",
		*PublicVarName = "_pawnregex_version";

	static bool Load(void)
	{
		return logprintf("%s plugin v%s by urShadow loaded", Name, Version), true;
	}

	static void Unload(void)
	{
		_regex_set.clear();
		_match_results_set.clear();

		logprintf("%s plugin v%s by urShadow unloaded", Name, Version);
	}

	static void AmxLoad(AMX *amx)
	{
		static std::vector<AMX_NATIVE_INFO> native_vec =
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
		if (!amx_FindPubVar(amx, PublicVarName, &addr) &&
			!amx_GetAddr(amx, addr, &phys_addr))
		{
			if (*phys_addr != PAWNREGEX_INCLUDE_VERSION)
				return logprintf("[%s] %s: .inc-file version does not equal a plugin version. Recompile a script.", Name, __FUNCTION__);
		}

		amx_Register(amx, native_vec.data(), native_vec.size());
	}

private:

	using syntax_option = std::regex_constants::syntax_option_type;
	using match_flag = std::regex_constants::match_flag_type;
	using regex_ptr = std::shared_ptr<std::regex>;
	using regex_set = std::unordered_set<regex_ptr>;
	using match_results_ptr = std::shared_ptr<std::vector<std::string>>;
	using match_results_set = std::unordered_set<match_results_ptr>;

	// native regex:regex_new(const pattern[], E_REGEX_FLAG:flags = REGEX_DEFAULT, E_REGEX_GRAMMAR:grammar = REGEX_ECMASCRIPT);
	static cell AMX_NATIVE_CALL n_regex_new(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 3, params))
			return 0;

		std::unique_ptr<char[]> pattern(get_string(amx, params[1]));

		if (pattern)
		{
			try
			{
				syntax_option option{};

				auto flags = static_cast<E_REGEX_FLAG>(params[2]);
				auto grammar = static_cast<E_REGEX_GRAMMAR>(params[3]);

				switch (grammar)
				{
					case REGEX_ECMASCRIPT:
					default:
						option = syntax_option::ECMAScript;
						break;
					case REGEX_BASIC:
						option = syntax_option::basic;
						break;
					case REGEX_EXTENDED:
						option = syntax_option::extended;
						break;
					case REGEX_AWK:
						option = syntax_option::awk;
						break;
					case REGEX_GREP:
						option = syntax_option::grep;
						break;
					case REGEX_EGREP:
						option = syntax_option::egrep;
						break;
				}

				if (flags & REGEX_ICASE)
					option |= syntax_option::icase;
				if (flags & REGEX_NOSUBS)
					option |= syntax_option::nosubs;
				if (flags & REGEX_OPTIMIZE)
					option |= syntax_option::optimize;
				if (flags & REGEX_COLLATE)
					option |= syntax_option::collate;

				auto regex = std::make_shared<std::regex>(pattern.get(), option);

				_regex_set.insert(regex);

				return reinterpret_cast<cell>(regex.get());
			}
			catch (const std::exception &e)
			{
				logprintf("[%s] %s: %s", Name, __FUNCTION__, e.what());
			}
		}
		else
		{
			logprintf("[%s] %s: invalid pattern", Name, __FUNCTION__);
		}

		return 0;
	}

	// native regex_delete(&regex:r);
	static cell AMX_NATIVE_CALL n_regex_delete(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 1, params))
			return 0;

		cell *cptr{};

		if (!amx_GetAddr(amx, params[1], &cptr))
		{
			if (auto regex = get_regex(*cptr))
			{
				_regex_set.erase(regex);

				*cptr = 0;

				return 1;
			}
			else
			{
				logprintf("[%s] %s: invalid regex handle", Name, __FUNCTION__);
			}
		}
		else
		{
			logprintf("[%s] %s: invalid param reference", Name, __FUNCTION__);
		}

		return 0;
	}

	// native regex_check(const str[], regex:r, E_MATCH_FLAG:flags = MATCH_DEFAULT);
	static cell AMX_NATIVE_CALL n_regex_check(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 3, params))
			return 0;

		std::unique_ptr<char[]> str(get_string(amx, params[1]));

		if (str)
		{
			if (auto regex = get_regex(params[2]))
			{
				try
				{
					match_flag flag = get_match_flag(static_cast<E_MATCH_FLAG>(params[3]));

					return static_cast<cell>(std::regex_match(str.get(), *regex, flag));
				}
				catch (const std::exception &e)
				{
					logprintf("[%s] %s: %s", Name, __FUNCTION__, e.what());
				}
			}
			else
			{
				logprintf("[%s] %s: invalid regex handle", Name, __FUNCTION__);
			}
		}
		else
		{
			logprintf("[%s] %s: invalid str", Name, __FUNCTION__);
		}

		return 0;
	}

	// native regex_match(const str[], regex:r, &match_results:m, E_MATCH_FLAG:flags = MATCH_DEFAULT);
	static cell AMX_NATIVE_CALL n_regex_match(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 4, params))
			return 0;

		std::unique_ptr<char[]> str(get_string(amx, params[1]));

		if (str)
		{
			if (auto regex = get_regex(params[2]))
			{
				try
				{
					auto m = std::make_shared<std::vector<std::string>>();

					match_flag flag = get_match_flag(static_cast<E_MATCH_FLAG>(params[4]));

					std::cmatch results;

					bool match = std::regex_match(str.get(), results, *regex, flag);

					if (match)
					{
						for (auto &result : results)
							m->push_back(result.str());

						_match_results_set.insert(m);

						cell *addr{};

						if (!amx_GetAddr(amx, params[3], &addr))
							*addr = reinterpret_cast<cell>(m.get());

						return 1;
					}
				}
				catch (const std::exception &e)
				{
					logprintf("[%s] %s: %s", Name, __FUNCTION__, e.what());
				}
			}
			else
			{
				logprintf("[%s] %s: invalid regex handle", Name, __FUNCTION__);
			}
		}
		else
		{
			logprintf("[%s] %s: invalid str", Name, __FUNCTION__);
		}

		return 0;
	}

	// native regex_search(const str[], regex:r, &match_results:m, &pos, startpos = 0, E_MATCH_FLAG:flags = MATCH_DEFAULT);
	static cell AMX_NATIVE_CALL n_regex_search(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 6, params))
			return 0;

		std::unique_ptr<char[]> str(get_string(amx, params[1]));

		if (str)
		{
			if (auto regex = get_regex(params[2]))
			{
				try
				{
					auto m = std::make_shared<std::vector<std::string>>();

					auto startpos = static_cast<size_t>(params[5]);

					match_flag flag = get_match_flag(static_cast<E_MATCH_FLAG>(params[6]));

					std::cmatch results;

					bool match = std::regex_search(&str.get()[startpos], results, *regex, flag);

					if (match)
					{
						for (auto &result : results)
							m->push_back(result.str());

						_match_results_set.insert(m);

						cell *addr{};

						if (!amx_GetAddr(amx, params[4], &addr))
							*addr = results.position();

						if (!amx_GetAddr(amx, params[3], &addr))
							*addr = reinterpret_cast<cell>(m.get());

						return 1;
					}
				}
				catch (const std::exception &e)
				{
					logprintf("[%s] %s: %s", Name, __FUNCTION__, e.what());
				}
			}
			else
			{
				logprintf("[%s] %s: invalid regex handle", Name, __FUNCTION__);
			}
		}
		else
		{
			logprintf("[%s] %s: invalid str", Name, __FUNCTION__);
		}

		return 0;
	}

	// native regex_replace(const str[], regex:r, const fmt[], dest[], E_MATCH_FLAG:flags = MATCH_DEFAULT, size = sizeof dest);
	static cell AMX_NATIVE_CALL n_regex_replace(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 6, params))
			return 0;

		std::unique_ptr<char[]>
			str(get_string(amx, params[1])),
			fmt(get_string(amx, params[3]));

		if (str && fmt)
		{
			if (auto regex = get_regex(params[2]))
			{
				try
				{
					match_flag flag = get_match_flag(static_cast<E_MATCH_FLAG>(params[5]));

					auto size = static_cast<size_t>(params[6]);

					auto result = std::regex_replace(str.get(), *regex, fmt.get(), flag);

					set_amxstring(amx, params[4], result.c_str(), size);

					return 1;
				}
				catch (const std::exception &e)
				{
					logprintf("[%s] %s: %s", Name, __FUNCTION__, e.what());
				}
			}
			else
			{
				logprintf("[%s] %s: invalid regex handle", Name, __FUNCTION__);
			}
		}
		else
		{
			logprintf("[%s] %s: invalid str or fmt", Name, __FUNCTION__);
		}

		return 0;
	}

	// native match_get_group(match_results:m, index, dest[], size = sizeof dest);
	static cell AMX_NATIVE_CALL n_match_get_group(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 4, params))
			return 0;

		if (auto match_results = get_match_results(params[1]))
		{
			auto
				index = static_cast<size_t>(params[2]),
				size = static_cast<size_t>(params[4]);

			try
			{
				set_amxstring(amx, params[3], match_results->at(index).c_str(), size);
			}
			catch (const std::exception &e)
			{
				logprintf("[%s] %s: %s", Name, __FUNCTION__, e.what());
			}
		}
		else
		{
			logprintf("[%s] %s: invalid match_results handle", Name, __FUNCTION__);
		}

		return 0;
	}

	// native match_free(&match_results:m);
	static cell AMX_NATIVE_CALL n_match_free(AMX *amx, cell *params)
	{
		if (!check_params(__FUNCTION__, 1, params))
			return 0;

		cell *cptr{};

		if (!amx_GetAddr(amx, params[1], &cptr))
		{
			if (auto match_results = get_match_results(*cptr))
			{
				_match_results_set.erase(match_results);

				*cptr = 0;

				return 1;
			}
			else
			{
				logprintf("[%s] %s: invalid match_results handle", Name, __FUNCTION__);
			}
		}
		else
		{
			logprintf("[%s] %s: invalid param reference", Name, __FUNCTION__);
		}

		return 0;
	}

	static inline regex_ptr get_regex(cell ptr)
	{
		auto iter = std::find_if(_regex_set.begin(), _regex_set.end(), [ptr](const regex_ptr& r)
		{
			return r.get() == reinterpret_cast<void *>(ptr);
		});

		if (iter != _regex_set.end())
			return *iter;

		return nullptr;
	}

	static inline match_results_ptr get_match_results(cell ptr)
	{
		auto iter = std::find_if(_match_results_set.begin(), _match_results_set.end(), [ptr](const match_results_ptr& r)
		{
			return r.get() == reinterpret_cast<void *>(ptr);
		});

		if (iter != _match_results_set.end())
			return *iter;

		return nullptr;
	}

	static inline match_flag get_match_flag(E_MATCH_FLAG flags)
	{
		match_flag flag{};

		if (flags & MATCH_DEFAULT)
			flag |= match_flag::match_default;
		if (flags & MATCH_NOT_BOL)
			flag |= match_flag::match_not_bol;
		if (flags & MATCH_NOT_EOL)
			flag |= match_flag::match_not_eol;
		if (flags & MATCH_NOT_BOW)
			flag |= match_flag::match_not_bow;
		if (flags & MATCH_NOT_EOW)
			flag |= match_flag::match_not_eow;
		if (flags & MATCH_ANY)
			flag |= match_flag::match_any;
		if (flags & MATCH_NOT_NULL)
			flag |= match_flag::match_not_null;
		if (flags & MATCH_CONTINUOUS)
			flag |= match_flag::match_continuous;
		if (flags & MATCH_PREV_AVAIL)
			flag |= match_flag::match_prev_avail;
		if (flags & MATCH_FORMAT_SED)
			flag |= match_flag::format_sed;
		if (flags & MATCH_FORMAT_NO_COPY)
			flag |= match_flag::format_no_copy;
		if (flags & MATCH_FORMAT_FIRST_ONLY)
			flag |= match_flag::format_first_only;

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
			return logprintf("[%s] %s: invalid number of parameters. Should be %d", Name, native, count), false;

		return true;
	}

	static inline int set_amxstring(AMX *amx, cell amx_addr, const char *source, int max)
	{
		cell *dest = reinterpret_cast<cell *>(amx->base + static_cast<int>(reinterpret_cast<AMX_HEADER *>(amx->base)->dat + amx_addr));

		cell *start = dest;

		while (max--&&*source)
			*dest++ = static_cast<cell>(*source++);

		*dest = 0;

		return dest - start;
	}

	static regex_set _regex_set;

	static match_results_set _match_results_set;
};

Plugin::regex_set Plugin::_regex_set;

Plugin::match_results_set Plugin::_match_results_set;

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