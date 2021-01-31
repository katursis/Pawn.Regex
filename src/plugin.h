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

#ifndef PAWNREGEX_PLUGIN_H_
#define PAWNREGEX_PLUGIN_H_

class Plugin : public ptl::AbstractPlugin<Plugin, Script, Cell> {
 public:
  const char *Name() { return "Pawn.Regex"; }

  int Version() { return PAWNREGEX_VERSION; }

  bool OnLoad() {
    ReadConfig();

    RegisterNative<natives::Regex_New>("Regex_New");
    RegisterNative<natives::Regex_Delete>("Regex_Delete");

    RegisterNative<natives::Regex_Check>("Regex_Check");
    RegisterNative<natives::Regex_Match>("Regex_Match");
    RegisterNative<natives::Regex_Search>("Regex_Search");
    RegisterNative<natives::Regex_Replace>("Regex_Replace");

    RegisterNative<natives::Match_GetGroup>("Match_GetGroup");
    RegisterNative<natives::Match_Free>("Match_Free");

    Log("plugin v%s by katursis loaded", VersionAsString().c_str());

    return true;
  }

  void OnUnload() {
    SaveConfig();

    Log("plugin v%s by katursis unloaded", VersionAsString().c_str());
  }

  void ReadConfig() {
    std::fstream{config_path_, std::fstream::out | std::fstream::app};

    const auto config = cpptoml::parse_file(config_path_);

    locale_ =
        std::locale{config->get_as<std::string>("LocaleName").value_or("C")};
  }

  void SaveConfig() {
    auto config = cpptoml::make_table();

    config->insert("LocaleName", locale_.name());

    std::fstream{config_path_, std::fstream::out | std::fstream::trunc}
        << (*config);
  }

  const std::locale &GetLocale() const { return locale_; }

 private:
  const std::string config_path_ = "plugins/pawnregex.cfg";

  std::locale locale_;
};

#endif  // PAWNREGEX_PLUGIN_H_
