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

#ifdef USE_XML_CONFIG
#    include <ctools/ConfigAbstract.h>
#    include <ctools/cTools.h>
#endif // USE_XML_CONFIG

#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || defined(WIN64) || defined(_WIN64) || defined(_MSC_VER)
#if defined(ImGuiPack_EXPORTS)
#define IMGUI_API __declspec(dllexport)
#define IMPLOT_API __declspec(dllexport)
#define IGFD_API __declspec(dllexport)
#define IMGUI_IMPL_API __declspec(dllexport)
#elif defined(IMGUI_PACK_SHARED_LIBS)
#define IMGUI_API __declspec(dllimport)
#define IMPLOT_API __declspec(dllimport)
#define IGFD_API __declspec(dllimport)
#define IMGUI_IMPL_API __declspec(dllimport)
#else
#define IMGUI_API
#define IMPLOT_API
#define IGFD_API
#define IMGUI_IMPL_API
#endif
#else
#define IMGUI_API
#define IMPLOT_API
#define IGFD_API
#define IMGUI_IMPL_API
#endif

#include <ImGuiFileDialog.h>
#include <imgui.h>
#include <string>
#include <map>

namespace IGFD {
class FileStyle;
}

class IMGUI_API ImGuiTheme {
public:
	ImVec4 colors[ImGuiCol_COUNT];
	std::map<std::string, IGFD::FileStyle> fileTypeInfos;
	ImGuiStyle style;
    ImVec4 goodColor = ImVec4(0.00f, 0.35f, 0.00f, 1.00f);
    ImVec4 badColor  = ImVec4(0.35f, 0.00f, 0.00f, 1.00f);
};

class IMGUI_API ImGuiThemeHelper
#ifdef USE_XML_CONFIG 
	: public conf::ConfigAbstract
#endif // USE_XML_CONFIG
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

    void DrawFileStyle();

public:  // configuration
#ifdef USE_XML_CONFIG
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
    bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
#endif // USE_XML_CONFIG

	ImGuiStyle GetImGuiStyle() { return m_CurrentTheme.style; }

private:
	void ApplyFileTypeColors();

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

