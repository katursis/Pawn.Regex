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

bool Plugin::OnLoad() {
  ReadConfig();

  RegisterNative<&Script::Regex_New>("Regex_New");
  RegisterNative<&Script::Regex_Delete>("Regex_Delete");

  RegisterNative<&Script::Regex_Check>("Regex_Check");
  RegisterNative<&Script::Regex_Match>("Regex_Match");
  RegisterNative<&Script::Regex_Search>("Regex_Search");
  RegisterNative<&Script::Regex_Replace>("Regex_Replace");

  RegisterNative<&Script::Match_GetGroup>("Match_GetGroup");
  RegisterNative<&Script::Match_Free>("Match_Free");

  Log("\n\n"
      "    | %s %s | 2016 - %s"
      "\n"
      "    |--------------------------------"
      "\n"
      "    | Author and maintainer: katursis"
      "\n\n\n"
      "    | Compiled: %s at %s"
      "\n"
      "    |--------------------------------------------------------------"
      "\n"
      "    | Repository: https://github.com/katursis/%s"
      "\n",
      Name(), VersionAsString().c_str(), &__DATE__[7], __DATE__, __TIME__,
      Name());

  return true;
}

void Plugin::OnUnload() {
  SaveConfig();

  Log("plugin unloaded");
}

void Plugin::ReadConfig() {
  std::fstream{config_path_, std::fstream::out | std::fstream::app};

  const auto config = cpptoml::parse_file(config_path_);

  locale_ =
      std::locale{config->get_as<std::string>("LocaleName").value_or("C")};
}

void Plugin::SaveConfig() {
  auto config = cpptoml::make_table();

  config->insert("LocaleName", locale_.name());

  std::fstream{config_path_, std::fstream::out | std::fstream::trunc}
      << (*config);
}
