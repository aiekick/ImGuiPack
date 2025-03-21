/*
 * Copyright 2020 Stephane Cuillerdier (aka Aiekick)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

#include <imguipack.h>

#include <string>
#include <map>

#ifdef CUSTOM_THEME_HELPER_CONFIG_HEADER
#include CUSTOM_THEME_HELPER_CONFIG_HEADER
#else
#include "ImGuiThemeHelperConfigHeader.h"
#endif  // THEME_HELPER_CONFIG_HEADER

#ifdef USE_IMGUI_FILE_DIALOG
namespace IGFD {
class FileStyle;
}
#endif

class IMGUI_API ImGuiTheme {
public:
    ImVec4 colors[ImGuiCol_COUNT];
#ifdef USE_IMGUI_FILE_DIALOG
    std::map<std::string, IGFD::FileStyle> fileTypeInfos;
#endif  // USE_IMGUI_FILE_DIALOG
	ImGuiStyle style;
    ImVec4 goodColor = ImVec4(0.00f, 0.35f, 0.00f, 1.00f);
    ImVec4 badColor  = ImVec4(0.35f, 0.00f, 0.00f, 1.00f);
};

class IMGUI_API ImGuiThemeHelper
#ifdef EZ_TOOLS_XML_CONFIG 
	: public ez::xml::Config
#endif // EZ_TOOLS_XML_CONFIG
{
public:
	bool puShowImGuiStyleEdtor = false;

private:
	std::map<std::string, ImGuiTheme> m_ThemeContainer;
	ImGuiTheme m_CurrentTheme;
    std::string m_DefaultThemeName;

public:
    bool init();
    void unit();

	void AddTheme(const std::string& vThemeName, const ImGuiTheme& vImGuiTheme);
	void RemoveTheme(const std::string& vThemeName);
    void ApplyTheme(const ImGuiTheme& vTheme);

    void ApplyDefaultTheme();
    void SetDefaultTheme(const std::string& vDefaultTheme);

	void Draw();
	void DrawMenu();
    void ShowCustomImGuiStyleEditor(bool* vOpen, ImGuiStyle* ref = nullptr);

#ifdef USE_IMGUI_FILE_DIALOG
    void DrawFileStyle();
#endif  // USE_IMGUI_FILE_DIALOG

public:  // configuration
#ifdef EZ_TOOLS_XML_CONFIG
    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;
#endif // EZ_TOOLS_XML_CONFIG

    ImGuiTheme GetCurrentThem() { return m_CurrentTheme; }
	ImGuiStyle GetImGuiStyle() { return m_CurrentTheme.style; }

private:
#ifdef USE_IMGUI_FILE_DIALOG
    void ApplyFileTypeColors();
#endif  // USE_IMGUI_FILE_DIALOG

	// ImGuiColorTextEdit Palette
	std::string GetStyleColorName(ImGuiCol idx);
    int GetImGuiColFromName(const std::string& vName);

public:  // singleton
    static ImGuiThemeHelper* Instance(ImGuiThemeHelper* vCopy = nullptr, bool vForce = false) {
        static ImGuiThemeHelper _instance;
        static ImGuiThemeHelper* _instance_copy = nullptr;
        if (vCopy || vForce) {
            _instance_copy = vCopy;
        }
        if (_instance_copy) {
            return _instance_copy;
        }
        return &_instance;
    }

public:
	ImGuiThemeHelper(); // Prevent construction
	ImGuiThemeHelper(const ImGuiThemeHelper&) {}; // Prevent construction by copying
	ImGuiThemeHelper& operator =(const ImGuiThemeHelper&) { return *this; }; // Prevent assignment
	~ImGuiThemeHelper(); // Prevent unwanted destruction
};

