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

#ifdef USE_XML_CONFIG
#    include <ConfigAbstract.h>
#endif // USE_XML_CONFIG

#include <ImGuiFileDialog.h>
#include <imgui.h>
#include <string>
#include <map>

class IMGUI_API ImGuiTheme {
public:
	ImVec4 colors[ImGuiCol_COUNT];
	std::map<std::string, IGFD::FileStyle> fileTypeInfos;
	ImGuiStyle style;
	ImVec4 goodColor = ImVec4(0.00f, 0.35f, 0.00f, 1.00f);
	ImVec4 badColor = ImVec4(0.35f, 0.00f, 0.00f, 1.00f);
};

class IMGUI_API ThemeHelper
#ifdef USE_XML_CONFIG 
	: public conf::ConfigAbstract
#endif // USE_XML_CONFIG
{
public:
	bool puShowImGuiStyleEdtor = false;

private:
	std::map<std::string, ImGuiTheme> m_ThemeContainer;
	ImGuiTheme m_CurrentTheme;

public:
    bool init();
    void unit();

	void AddTheme(const std::string& vThemeName, const ImGuiTheme& vImGuiTheme);
	void RemoveTheme(const std::string& vThemeName);

	void Draw();
	void DrawMenu();
    void ShowCustomImGuiStyleEditor(bool* vOpen, ImGuiStyle* ref = nullptr);

public: // configuration
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

public:
	ThemeHelper(); // Prevent construction
	ThemeHelper(const ThemeHelper&) {}; // Prevent construction by copying
	ThemeHelper& operator =(const ThemeHelper&) { return *this; }; // Prevent assignment
	~ThemeHelper(); // Prevent unwanted destruction
};

