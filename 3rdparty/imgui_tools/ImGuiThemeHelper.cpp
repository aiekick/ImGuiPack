// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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
#include "ImGuiThemeHelper.h"

#include <ImWidgets.h>
#include <imgui.h>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif // IMGUI_DEFINE_MATH_OPERATORS

#include <imgui_internal.h>

#ifdef THEME_HELPER_CONFIG_HEADER
#    include THEME_HELPER_CONFIG_HEADER
#else
#    include "ImGuiThemeHelperConfigHeader.h"
#endif // THEME_HELPER_CONFIG_HEADER

ImGuiThemeHelper::ImGuiThemeHelper() = default;
ImGuiThemeHelper::~ImGuiThemeHelper() = default;

bool ImGuiThemeHelper::init()
{
	return true;
}

void ImGuiThemeHelper::unit()
{

}

void ImGuiThemeHelper::AddTheme(const std::string& vThemeName, const ImGuiTheme& vImGuiTheme) {
	m_ThemeContainer[vThemeName] = vImGuiTheme;
}

void ImGuiThemeHelper::RemoveTheme(const std::string& vThemeName) {
	if (m_ThemeContainer.find(vThemeName) != m_ThemeContainer.end()) {
		m_ThemeContainer.erase(vThemeName);
	}
}

void ImGuiThemeHelper::ApplyTheme(const ImGuiTheme& vTheme) {
    m_CurrentTheme = vTheme;
    ImGui::GetStyle() = m_CurrentTheme.style;
    ImGui::CustomStyle::GoodColor = m_CurrentTheme.goodColor;
    ImGui::CustomStyle::BadColor = m_CurrentTheme.badColor;
}

void ImGuiThemeHelper::ApplyDefaultTheme() {
    if (!m_DefaultThemeName.empty()) {
        if (m_ThemeContainer.find(m_DefaultThemeName) != m_ThemeContainer.end()) {
            ApplyTheme(m_ThemeContainer.at(m_DefaultThemeName));
        }
	}
}

void ImGuiThemeHelper::SetDefaultTheme(const std::string& vDefaultTheme) {
    m_DefaultThemeName = vDefaultTheme;
}

void ImGuiThemeHelper::Draw()
{
	if (puShowImGuiStyleEdtor)
		ShowCustomImGuiStyleEditor(&puShowImGuiStyleEdtor);
}

void ImGuiThemeHelper::DrawMenu()
{
	if (ImGui::BeginMenu("General UI"))
	{
		for (auto& it : m_ThemeContainer) {
			if (ImGui::MenuItem(it.first.c_str())) {
                ApplyTheme(it.second);
			}
		}

		ImGui::Separator();

		ImGui::MenuItem("Customize", "", &puShowImGuiStyleEdtor);
		
		ImGui::Separator();
		
		if (ImGui::BeginMenu("Contrast"))
		{
			ImGui::DrawContrastWidgets();

			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("File Type Colors"))
	{
		bool fileColorUpdate = false;

		for (auto &it : m_CurrentTheme.fileTypeInfos)
		{
			fileColorUpdate |= ImGui::ColorEdit4(it.first.c_str(), &m_CurrentTheme.fileTypeInfos[it.first].color.x);
		}

		if (fileColorUpdate)
		{
			ApplyFileTypeColors();
		}

		ImGui::EndMenu();
	}
}

void ImGuiThemeHelper::ApplyFileTypeColors()
{
	for (auto &it : m_CurrentTheme.fileTypeInfos)
	{
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, it.first.c_str(), it.second.color, it.second.icon);
	}
}

#ifdef USE_XML_CONFIG

///////////////////////////////////////////////////////
//// CONFIGURATION ////////////////////////////////////
///////////////////////////////////////////////////////

std::string ImGuiThemeHelper::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	{
        auto colors = m_CurrentTheme.style.Colors;
		
		str += vOffset + "<ImGui_Styles>\n";
		for (auto i = 0; i < ImGuiCol_COUNT; i++)
		{
			str += vOffset + "\t<" + GetStyleColorName(i) + " value=\"" + ct::fvec4(colors[i]).string() + "\"/>\n";
		}
		str += vOffset + "\t<WindowPadding value=\"" + ct::fvec2(m_CurrentTheme.style.WindowPadding).string() + "\"/>\n";
		str += vOffset + "\t<FramePadding value=\"" + ct::fvec2(m_CurrentTheme.style.FramePadding).string() + "\"/>\n";
		str += vOffset + "\t<ItemSpacing value=\"" + ct::fvec2(m_CurrentTheme.style.ItemSpacing).string() + "\"/>\n";
		str += vOffset + "\t<ItemInnerSpacing value=\"" + ct::fvec2(m_CurrentTheme.style.ItemInnerSpacing).string() + "\"/>\n";
		str += vOffset + "\t<IndentSpacing value=\"" + ct::toStr(m_CurrentTheme.style.IndentSpacing) + "\"/>\n";
		str += vOffset + "\t<ScrollbarSize value=\"" + ct::toStr(m_CurrentTheme.style.ScrollbarSize) + "\"/>\n";
		str += vOffset + "\t<GrabMinSize value=\"" + ct::toStr(m_CurrentTheme.style.GrabMinSize) + "\"/>\n";
		str += vOffset + "\t<WindowRounding value=\"" + ct::toStr(m_CurrentTheme.style.WindowRounding) + "\"/>\n";
		str += vOffset + "\t<ChildRounding value=\"" + ct::toStr(m_CurrentTheme.style.ChildRounding) + "\"/>\n";
		str += vOffset + "\t<FrameRounding value=\"" + ct::toStr(m_CurrentTheme.style.FrameRounding) + "\"/>\n";
		str += vOffset + "\t<PopupRounding value=\"" + ct::toStr(m_CurrentTheme.style.PopupRounding) + "\"/>\n";
		str += vOffset + "\t<ScrollbarRounding value=\"" + ct::toStr(m_CurrentTheme.style.ScrollbarRounding) + "\"/>\n";
		str += vOffset + "\t<GrabRounding value=\"" + ct::toStr(m_CurrentTheme.style.GrabRounding) + "\"/>\n";
		str += vOffset + "\t<TabRounding value=\"" + ct::toStr(m_CurrentTheme.style.TabRounding) + "\"/>\n";
		str += vOffset + "\t<WindowBorderSize value=\"" + ct::toStr(m_CurrentTheme.style.WindowBorderSize) + "\"/>\n";
		str += vOffset + "\t<ChildBorderSize value=\"" + ct::toStr(m_CurrentTheme.style.ChildBorderSize) + "\"/>\n";
		str += vOffset + "\t<PopupBorderSize value=\"" + ct::toStr(m_CurrentTheme.style.PopupBorderSize) + "\"/>\n";
		str += vOffset + "\t<FrameBorderSize value=\"" + ct::toStr(m_CurrentTheme.style.FrameBorderSize) + "\"/>\n";
		str += vOffset + "\t<TabBorderSize value=\"" + ct::toStr(m_CurrentTheme.style.TabBorderSize) + "\"/>\n";
		str += vOffset + "</ImGui_Styles>\n";
	}

	{
#ifdef USE_NODEGRAPH
		str += vOffset + "<Graph_Styles>\n";
		str += vOffset + "\t<NodePadding value=\"" + ct::toStrFromImVec4(prNodeGraphStyle.NodePadding) + "\"/>\n";
		str += vOffset + "\t<NodeRounding value=\"" + ct::toStr(prNodeGraphStyle.NodeRounding) + "\"/>\n";
		str += vOffset + "\t<NodeBorderWidth value=\"" + ct::toStr(prNodeGraphStyle.NodeBorderWidth) + "\"/>\n";
		str += vOffset + "\t<HoveredNodeBorderWidth value=\"" + ct::toStr(prNodeGraphStyle.HoveredNodeBorderWidth) + "\"/>\n";
		str += vOffset + "\t<SelectedNodeBorderWidth value=\"" + ct::toStr(prNodeGraphStyle.SelectedNodeBorderWidth) + "\"/>\n";
		str += vOffset + "\t<PinRounding value=\"" + ct::toStr(prNodeGraphStyle.PinRounding) + "\"/>\n";
		str += vOffset + "\t<PinBorderWidth value=\"" + ct::toStr(prNodeGraphStyle.PinBorderWidth) + "\"/>\n";
		str += vOffset + "\t<LinkStrength value=\"" + ct::toStr(prNodeGraphStyle.LinkStrength) + "\"/>\n";
		str += vOffset + "\t<SourceDirection value=\"" + ct::toStrFromImVec2(prNodeGraphStyle.SourceDirection) + "\"/>\n";
		str += vOffset + "\t<TargetDirection value=\"" + ct::toStrFromImVec2(prNodeGraphStyle.TargetDirection) + "\"/>\n";
		str += vOffset + "\t<ScrollDuration value=\"" + ct::toStr(prNodeGraphStyle.ScrollDuration) + "\"/>\n";
		str += vOffset + "\t<FlowMarkerDistance value=\"" + ct::toStr(prNodeGraphStyle.FlowMarkerDistance) + "\"/>\n";
		str += vOffset + "\t<FlowSpeed value=\"" + ct::toStr(prNodeGraphStyle.FlowSpeed) + "\"/>\n";
		str += vOffset + "\t<FlowDuration value=\"" + ct::toStr(prNodeGraphStyle.FlowDuration) + "\"/>\n";
		str += vOffset + "\t<PivotAlignment value=\"" + ct::toStrFromImVec2(prNodeGraphStyle.PivotAlignment) + "\"/>\n";
		str += vOffset + "\t<PivotSize value=\"" + ct::toStrFromImVec2(prNodeGraphStyle.PivotSize) + "\"/>\n";
		str += vOffset + "\t<PivotScale value=\"" + ct::toStrFromImVec2(prNodeGraphStyle.PivotScale) + "\"/>\n";
		str += vOffset + "\t<PinCorners value=\"" + ct::toStr(prNodeGraphStyle.PinCorners) + "\"/>\n";
		str += vOffset + "\t<PinRadius value=\"" + ct::toStr(prNodeGraphStyle.PinRadius) + "\"/>\n";
		str += vOffset + "\t<PinArrowSize value=\"" + ct::toStr(prNodeGraphStyle.PinArrowSize) + "\"/>\n";
		str += vOffset + "\t<PinArrowWidth value=\"" + ct::toStr(prNodeGraphStyle.PinArrowWidth) + "\"/>\n";
		str += vOffset + "\t<GroupRounding value=\"" + ct::toStr(prNodeGraphStyle.GroupRounding) + "\"/>\n";
		str += vOffset + "\t<GroupBorderWidth value=\"" + ct::toStr(prNodeGraphStyle.GroupBorderWidth) + "\"/>\n";
		str += vOffset + "</Graph_Styles>\n";
#endif
	}

	{
		str += vOffset + "<FileTypes>\n";
		for (auto& it : m_CurrentTheme.fileTypeInfos)
		{
			str += vOffset + "\t<filetype value=\"" + it.first + "\" color=\"" +
				ct::fvec4(it.second.color).string() + "\"/>\n";
		}
		str += vOffset + "</FileTypes>\n";
	}

	return str;
}

bool ImGuiThemeHelper::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	// The value of this child identifies the name of this element
	std::string strName;
	std::string strValue;
	std::string strParentName;

	strName = vElem->Value();
	if (vElem->GetText())
		strValue = vElem->GetText();
	if (vParent != nullptr)
		strParentName = vParent->Value();

	if (strParentName == "FileTypes")
	{
		std::string fileType;
		std::string color;
		
		for (auto attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next())
		{
			std::string attName = attr->Name();
			const std::string attValue = attr->Value();

			if (attName == "value") fileType = attValue;
			if (attName == "color") color = attValue;
		}

		m_CurrentTheme.fileTypeInfos[fileType] = IGFD::FileStyle(ct::toImVec4(ct::fvariant(color).GetV4()));
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, fileType.c_str(), m_CurrentTheme.fileTypeInfos[fileType]);
	}

#ifdef USE_NODEGRAPH
	if (strParentName == "Graph_Styles")
	{
		const auto att = vElem->FirstAttribute();
		if (att && std::string(att->Name()) == "value")
		{
			strValue = att->Value();
			
			if (strName == "NodePadding")
				prNodeGraphStyle.NodePadding = ct::toImVec4(ct::fvariant(strValue).GetV4());
			if (strName == "NodeRounding")
				prNodeGraphStyle.NodeRounding = ct::fvariant(strValue).GetF();
			if (strName == "NodeBorderWidth")
				prNodeGraphStyle.NodeBorderWidth = ct::fvariant(strValue).GetF();
			if (strName == "HoveredNodeBorderWidth")
				prNodeGraphStyle.HoveredNodeBorderWidth = ct::fvariant(strValue).GetF();
			if (strName == "SelectedNodeBorderWidth")
				prNodeGraphStyle.SelectedNodeBorderWidth = ct::fvariant(strValue).GetF();
			if (strName == "PinRounding")
				prNodeGraphStyle.PinRounding = ct::fvariant(strValue).GetF();
			if (strName == "PinBorderWidth")
				prNodeGraphStyle.PinBorderWidth = ct::fvariant(strValue).GetF();
			if (strName == "LinkStrength")
				prNodeGraphStyle.LinkStrength = ct::fvariant(strValue).GetF();
			if (strName == "SourceDirection")
				prNodeGraphStyle.SourceDirection = ct::toImVec2(ct::fvariant(strValue).GetV2());
			if (strName == "TargetDirection")
				prNodeGraphStyle.TargetDirection = ct::toImVec2(ct::fvariant(strValue).GetV2());
			if (strName == "ScrollDuration")
				prNodeGraphStyle.ScrollDuration = ct::fvariant(strValue).GetF();
			if (strName == "FlowMarkerDistance")
				prNodeGraphStyle.FlowMarkerDistance = ct::fvariant(strValue).GetF();
			if (strName == "FlowSpeed")
				prNodeGraphStyle.FlowSpeed = ct::fvariant(strValue).GetF();
			if (strName == "FlowDuration")
				prNodeGraphStyle.FlowDuration = ct::fvariant(strValue).GetF();
			if (strName == "PivotAlignment")
				prNodeGraphStyle.PivotAlignment = ct::toImVec2(ct::fvariant(strValue).GetV2());
			if (strName == "PivotSize")
				prNodeGraphStyle.PivotSize = ct::toImVec2(ct::fvariant(strValue).GetV2());
			if (strName == "PivotScale")
				prNodeGraphStyle.PivotScale = ct::toImVec2(ct::fvariant(strValue).GetV2());
			if (strName == "PinCorners")
				prNodeGraphStyle.PinCorners = ct::fvariant(strValue).GetF();
			if (strName == "PinRadius")
				prNodeGraphStyle.PinRadius = ct::fvariant(strValue).GetF();
			if (strName == "PinArrowSize")
				prNodeGraphStyle.PinArrowSize = ct::fvariant(strValue).GetF();
			if (strName == "PinArrowWidth")
				prNodeGraphStyle.PinArrowWidth = ct::fvariant(strValue).GetF();
			if (strName == "GroupRounding")
				prNodeGraphStyle.GroupRounding = ct::fvariant(strValue).GetF();
			if (strName == "GroupBorderWidth")
				prNodeGraphStyle.GroupBorderWidth = ct::fvariant(strValue).GetF();
		}
	}
#endif

	if (strParentName == "ImGui_Styles")
	{
		const auto att = vElem->FirstAttribute();
		if (att && std::string(att->Name()) == "value")
		{
			strValue = att->Value();
			const auto colors = m_CurrentTheme.style.Colors;

			if (strName.find("ImGuiCol") != std::string::npos)
			{
				const auto id = GetImGuiColFromName(strName);
				if (id >= 0)
				{
					colors[id] = ct::toImVec4(ct::fvariant(strValue).GetV4());
					return false;
				}
			}

			if (strName == "WindowPadding") m_CurrentTheme.style.WindowPadding = ct::toImVec2(ct::fvariant(strValue).GetV2());
			else if (strName == "FramePadding") m_CurrentTheme.style.FramePadding = ct::toImVec2(ct::fvariant(strValue).GetV2());
			else if (strName == "ItemSpacing") m_CurrentTheme.style.ItemSpacing = ct::toImVec2(ct::fvariant(strValue).GetV2());
			else if (strName == "ItemInnerSpacing") m_CurrentTheme.style.ItemInnerSpacing = ct::toImVec2(ct::fvariant(strValue).GetV2());
			else if (strName == "IndentSpacing") m_CurrentTheme.style.IndentSpacing = ct::fvariant(strValue).GetF();
			else if (strName == "ScrollbarSize") m_CurrentTheme.style.ScrollbarSize = ct::fvariant(strValue).GetF();
			else if (strName == "GrabMinSize") m_CurrentTheme.style.GrabMinSize = ct::fvariant(strValue).GetF();
			else if (strName == "WindowRounding") m_CurrentTheme.style.WindowRounding = ct::fvariant(strValue).GetF();
			else if (strName == "ChildRounding") m_CurrentTheme.style.ChildRounding = ct::fvariant(strValue).GetF();
			else if (strName == "FrameRounding") m_CurrentTheme.style.FrameRounding = ct::fvariant(strValue).GetF();
			else if (strName == "PopupRounding") m_CurrentTheme.style.PopupRounding = ct::fvariant(strValue).GetF();
			else if (strName == "ScrollbarRounding") m_CurrentTheme.style.ScrollbarRounding = ct::fvariant(strValue).GetF();
			else if (strName == "GrabRounding") m_CurrentTheme.style.GrabRounding = ct::fvariant(strValue).GetF();
			else if (strName == "TabRounding") m_CurrentTheme.style.TabRounding = ct::fvariant(strValue).GetF();
			else if (strName == "WindowBorderSize") m_CurrentTheme.style.WindowBorderSize = ct::fvariant(strValue).GetF();
			else if (strName == "ChildBorderSize") m_CurrentTheme.style.ChildBorderSize = ct::fvariant(strValue).GetF();
			else if (strName == "PopupBorderSize") m_CurrentTheme.style.PopupBorderSize = ct::fvariant(strValue).GetF();
			else if (strName == "FrameBorderSize") m_CurrentTheme.style.FrameBorderSize = ct::fvariant(strValue).GetF();
			else if (strName == "TabBorderSize") m_CurrentTheme.style.TabBorderSize = ct::fvariant(strValue).GetF();
		}
	}

	return true;
}
#endif // USE_XML_CONFIG

///////////////////////////////////////////////////////
//// PRIVVATE /////////////////////////////////////////
///////////////////////////////////////////////////////

std::string ImGuiThemeHelper::GetStyleColorName(ImGuiCol idx)
{
	switch (idx)
	{
	case ImGuiCol_Text: return "ImGuiCol_Text";
	case ImGuiCol_TextDisabled: return "ImGuiCol_TextDisabled";
	case ImGuiCol_WindowBg: return "ImGuiCol_WindowBg";
	case ImGuiCol_ChildBg: return "ImGuiCol_ChildBg";
	case ImGuiCol_PopupBg: return "ImGuiCol_PopupBg";
	case ImGuiCol_Border: return "ImGuiCol_Border";
	case ImGuiCol_BorderShadow: return "ImGuiCol_BorderShadow";
	case ImGuiCol_FrameBg: return "ImGuiCol_FrameBg";
	case ImGuiCol_FrameBgHovered: return "ImGuiCol_FrameBgHovered";
	case ImGuiCol_FrameBgActive: return "ImGuiCol_FrameBgActive";
	case ImGuiCol_TitleBg: return "ImGuiCol_TitleBg";
	case ImGuiCol_TitleBgActive: return "ImGuiCol_TitleBgActive";
	case ImGuiCol_TitleBgCollapsed: return "ImGuiCol_TitleBgCollapsed";
	case ImGuiCol_MenuBarBg: return "ImGuiCol_MenuBarBg";
	case ImGuiCol_ScrollbarBg: return "ImGuiCol_ScrollbarBg";
	case ImGuiCol_ScrollbarGrab: return "ImGuiCol_ScrollbarGrab";
	case ImGuiCol_ScrollbarGrabHovered: return "ImGuiCol_ScrollbarGrabHovered";
	case ImGuiCol_ScrollbarGrabActive: return "ImGuiCol_ScrollbarGrabActive";
	case ImGuiCol_CheckMark: return "ImGuiCol_CheckMark";
	case ImGuiCol_SliderGrab: return "ImGuiCol_SliderGrab";
	case ImGuiCol_SliderGrabActive: return "ImGuiCol_SliderGrabActive";
	case ImGuiCol_Button: return "ImGuiCol_Button";
	case ImGuiCol_ButtonHovered: return "ImGuiCol_ButtonHovered";
	case ImGuiCol_ButtonActive: return "ImGuiCol_ButtonActive";
	case ImGuiCol_Header: return "ImGuiCol_Header";
	case ImGuiCol_HeaderHovered: return "ImGuiCol_HeaderHovered";
	case ImGuiCol_HeaderActive: return "ImGuiCol_HeaderActive";
	case ImGuiCol_Separator: return "ImGuiCol_Separator";
	case ImGuiCol_SeparatorHovered: return "ImGuiCol_SeparatorHovered";
	case ImGuiCol_SeparatorActive: return "ImGuiCol_SeparatorActive";
	case ImGuiCol_ResizeGrip: return "ImGuiCol_ResizeGrip";
	case ImGuiCol_ResizeGripHovered: return "ImGuiCol_ResizeGripHovered";
	case ImGuiCol_ResizeGripActive: return "ImGuiCol_ResizeGripActive";
	case ImGuiCol_Tab: return "ImGuiCol_Tab";
	case ImGuiCol_TabHovered: return "ImGuiCol_TabHovered";
	case ImGuiCol_TabActive: return "ImGuiCol_TabActive";
	case ImGuiCol_TabUnfocused: return "ImGuiCol_TabUnfocused";
	case ImGuiCol_TabUnfocusedActive: return "ImGuiCol_TabUnfocusedActive";
	case ImGuiCol_DockingPreview: return "ImGuiCol_DockingPreview";
	case ImGuiCol_DockingEmptyBg: return "ImGuiCol_DockingEmptyBg";
	case ImGuiCol_PlotLines: return "ImGuiCol_PlotLines";
	case ImGuiCol_PlotLinesHovered: return "ImGuiCol_PlotLinesHovered";
	case ImGuiCol_PlotHistogram: return "ImGuiCol_PlotHistogram";
	case ImGuiCol_PlotHistogramHovered: return "ImGuiCol_PlotHistogramHovered";
	case ImGuiCol_TableHeaderBg: return "ImGuiCol_TableHeaderBg";
	case ImGuiCol_TableBorderStrong: return "ImGuiCol_TableBorderStrong";
	case ImGuiCol_TableBorderLight: return "ImGuiCol_TableBorderLight";
	case ImGuiCol_TableRowBg: return "ImGuiCol_TableRowBg";
	case ImGuiCol_TableRowBgAlt: return "ImGuiCol_TableRowBgAlt";
	case ImGuiCol_TextSelectedBg: return "ImGuiCol_TextSelectedBg";
	case ImGuiCol_DragDropTarget: return "ImGuiCol_DragDropTarget";
	case ImGuiCol_NavHighlight: return "ImGuiCol_NavHighlight";
	case ImGuiCol_NavWindowingHighlight: return "ImGuiCol_NavWindowingHighlight";
	case ImGuiCol_NavWindowingDimBg: return "ImGuiCol_NavWindowingDimBg";
	case ImGuiCol_ModalWindowDimBg: return "ImGuiCol_ModalWindowDimBg";
	default:;
	}
	return "ImGuiCol_Unknown";
}

int ImGuiThemeHelper::GetImGuiColFromName(const std::string& vName)
{
	if (vName == "ImGuiCol_Text") return ImGuiCol_Text;
	else if (vName == "ImGuiCol_TextDisabled") return ImGuiCol_TextDisabled;
	else if (vName == "ImGuiCol_WindowBg") return ImGuiCol_WindowBg;
	else if (vName == "ImGuiCol_ChildBg") return ImGuiCol_ChildBg;
	else if (vName == "ImGuiCol_PopupBg") return ImGuiCol_PopupBg;
	else if (vName == "ImGuiCol_Border") return ImGuiCol_Border;
	else if (vName == "ImGuiCol_BorderShadow") return ImGuiCol_BorderShadow;
	else if (vName == "ImGuiCol_FrameBg") return ImGuiCol_FrameBg;
	else if (vName == "ImGuiCol_FrameBgHovered") return ImGuiCol_FrameBgHovered;
	else if (vName == "ImGuiCol_FrameBgActive") return ImGuiCol_FrameBgActive;
	else if (vName == "ImGuiCol_TitleBg") return ImGuiCol_TitleBg;
	else if (vName == "ImGuiCol_TitleBgActive") return ImGuiCol_TitleBgActive;
	else if (vName == "ImGuiCol_TitleBgCollapsed") return ImGuiCol_TitleBgCollapsed;
	else if (vName == "ImGuiCol_MenuBarBg") return ImGuiCol_MenuBarBg;
	else if (vName == "ImGuiCol_ScrollbarBg") return ImGuiCol_ScrollbarBg;
	else if (vName == "ImGuiCol_ScrollbarGrab") return ImGuiCol_ScrollbarGrab;
	else if (vName == "ImGuiCol_ScrollbarGrabHovered") return ImGuiCol_ScrollbarGrabHovered;
	else if (vName == "ImGuiCol_ScrollbarGrabActive") return ImGuiCol_ScrollbarGrabActive;
	else if (vName == "ImGuiCol_CheckMark") return ImGuiCol_CheckMark;
	else if (vName == "ImGuiCol_SliderGrab") return ImGuiCol_SliderGrab;
	else if (vName == "ImGuiCol_SliderGrabActive") return ImGuiCol_SliderGrabActive;
	else if (vName == "ImGuiCol_Button") return ImGuiCol_Button;
	else if (vName == "ImGuiCol_ButtonHovered") return ImGuiCol_ButtonHovered;
	else if (vName == "ImGuiCol_ButtonActive") return ImGuiCol_ButtonActive;
	else if (vName == "ImGuiCol_Header") return ImGuiCol_Header;
	else if (vName == "ImGuiCol_HeaderHovered") return ImGuiCol_HeaderHovered;
	else if (vName == "ImGuiCol_HeaderActive") return ImGuiCol_HeaderActive;
	else if (vName == "ImGuiCol_Separator") return ImGuiCol_Separator;
	else if (vName == "ImGuiCol_SeparatorHovered") return ImGuiCol_SeparatorHovered;
	else if (vName == "ImGuiCol_SeparatorActive") return ImGuiCol_SeparatorActive;
	else if (vName == "ImGuiCol_ResizeGrip") return ImGuiCol_ResizeGrip;
	else if (vName == "ImGuiCol_ResizeGripHovered") return ImGuiCol_ResizeGripHovered;
	else if (vName == "ImGuiCol_ResizeGripActive") return ImGuiCol_ResizeGripActive;
	else if (vName == "ImGuiCol_Tab") return ImGuiCol_Tab;
	else if (vName == "ImGuiCol_TabHovered") return ImGuiCol_TabHovered;
	else if (vName == "ImGuiCol_TabActive") return ImGuiCol_TabActive;
	else if (vName == "ImGuiCol_TabUnfocused") return ImGuiCol_TabUnfocused;
	else if (vName == "ImGuiCol_TabUnfocusedActive") return ImGuiCol_TabUnfocusedActive;
	else if (vName == "ImGuiCol_DockingPreview") return ImGuiCol_DockingPreview;
	else if (vName == "ImGuiCol_DockingEmptyBg") return ImGuiCol_DockingEmptyBg;
	else if (vName == "ImGuiCol_PlotLines") return ImGuiCol_PlotLines;
	else if (vName == "ImGuiCol_PlotLinesHovered") return ImGuiCol_PlotLinesHovered;
	else if (vName == "ImGuiCol_PlotHistogram") return ImGuiCol_PlotHistogram;
	else if (vName == "ImGuiCol_PlotHistogramHovered") return ImGuiCol_PlotHistogramHovered;
	else if (vName == "ImGuiCol_TableHeaderBg") return ImGuiCol_TableHeaderBg;
	else if (vName == "ImGuiCol_TableBorderStrong") return ImGuiCol_TableBorderStrong;
	else if (vName == "ImGuiCol_TableBorderLight") return ImGuiCol_TableBorderLight;
	else if (vName == "ImGuiCol_TableRowBg") return ImGuiCol_TableRowBg;
	else if (vName == "ImGuiCol_TableRowBgAlt") return ImGuiCol_TableRowBgAlt;
	else if (vName == "ImGuiCol_TextSelectedBg") return ImGuiCol_TextSelectedBg;
	else if (vName == "ImGuiCol_DragDropTarget") return ImGuiCol_DragDropTarget;
	else if (vName == "ImGuiCol_NavHighlight") return ImGuiCol_NavHighlight;
	else if (vName == "ImGuiCol_NavWindowingHighlight") return ImGuiCol_NavWindowingHighlight;
	else if (vName == "ImGuiCol_NavWindowingDimBg") return ImGuiCol_NavWindowingDimBg;
	else if (vName == "ImGuiCol_ModalWindowDimBg") return ImGuiCol_ModalWindowDimBg;
	return -1;
}

inline void DrawItem(int vIdx, const ImGuiTextFilter& vFilter, const char* vName, ImVec4& vStyleColor, ImVec4& vRefColor, ImGuiColorEditFlags vFlags)
{
	if (!vFilter.PassFilter(vName))
		return;

	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::PushID(vIdx);
	ImGui::ColorEdit4("##color", (float*)&vStyleColor, ImGuiColorEditFlags_AlphaBar | vFlags);
	if (memcmp(&vStyleColor, &vRefColor, sizeof(ImVec4)) != 0)
	{
		// Tips: in a real user application, you may want to merge and use an icon font into the main font, so instead of "Save"/"Revert" you'd use icons.
		// Read the FAQ and docs/FONTS.txt about using icon fonts. It's really easy and super convenient!
		ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::ContrastedButton("Save")) vRefColor = vStyleColor;
		ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::ContrastedButton("Revert")) vStyleColor = vRefColor;
	}
	ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
	ImGui::TextUnformatted(vName);
	ImGui::PopID();
}

inline void ExportColors(ImGuiStyle& style_to_export, ImGuiStyle& ref_style, bool export_only_modified)
{
	ImGui::LogText("ImVec4* colors = m_CurrentTheme.style.Colors;" IM_NEWLINE);

	for (auto i = 0; i < ImGuiCol_COUNT; i++)
	{
		const auto& col = style_to_export.Colors[i];
		const auto name = ImGui::GetStyleColorName(i);
		if (!export_only_modified || memcmp(&col, &ref_style.Colors[i], sizeof(ImVec4)) != 0)
			ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
	}

	ImGui::LogText(IM_NEWLINE);

	ImGui::LogText("ImGui::CustomStyle::GoodColor%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, 32 - (int)strlen("ImGui::CustomStyle::GoodColor"),
		"", ImGui::CustomStyle::GoodColor.x, ImGui::CustomStyle::GoodColor.y, ImGui::CustomStyle::GoodColor.z, ImGui::CustomStyle::GoodColor.w);
	ImGui::LogText("ImGui::CustomStyle::BadColor%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, 32 - (int)strlen("ImGui::CustomStyle::BadColor"),
		"", ImGui::CustomStyle::BadColor.x, ImGui::CustomStyle::BadColor.y, ImGui::CustomStyle::BadColor.z, ImGui::CustomStyle::BadColor.w);
}

inline void ExportSize_Float(const char* name, float& size_to_export, float& ref_size, bool export_only_modified)
{
	if (!export_only_modified || memcmp(&size_to_export, &ref_size, sizeof(float)) != 0)
		ImGui::LogText("style.%s%*s= %.2ff;" IM_NEWLINE, name, 25 - (int)strlen(name), "", size_to_export);
}

inline void ExportSize_ImVec2(const char* name, ImVec2& size_to_export, ImVec2& ref_size, bool export_only_modified)
{
	if (!export_only_modified || memcmp(&size_to_export, &ref_size, sizeof(ImVec2)) != 0)
		ImGui::LogText("style.%s%*s= ImVec2(%.2ff, %.2ff);" IM_NEWLINE, name, 25 - (int)strlen(name), "", size_to_export.x, size_to_export.y);
}

inline void ExportSizes(ImGuiStyle& style_to_export, ImGuiStyle& ref_style, bool export_only_modified)
{
	ImGui::LogText("ImGuiStyle& style = m_CurrentTheme.style;" IM_NEWLINE);

	{
		ImGui::LogText(IM_NEWLINE "// Main" IM_NEWLINE);

		ExportSize_ImVec2("WindowPadding", style_to_export.WindowPadding, ref_style.WindowPadding, export_only_modified);
		ExportSize_ImVec2("FramePadding", style_to_export.FramePadding, ref_style.FramePadding, export_only_modified);
		ExportSize_ImVec2("ItemSpacing", style_to_export.ItemSpacing, ref_style.ItemSpacing, export_only_modified);
		ExportSize_ImVec2("ItemInnerSpacing", style_to_export.ItemInnerSpacing, ref_style.ItemInnerSpacing, export_only_modified);
		ExportSize_ImVec2("TouchExtraPadding", style_to_export.TouchExtraPadding, ref_style.TouchExtraPadding, export_only_modified);
		ExportSize_Float("IndentSpacing", style_to_export.IndentSpacing, ref_style.IndentSpacing, export_only_modified);
		ExportSize_Float("ScrollbarSize", style_to_export.ScrollbarSize, ref_style.ScrollbarSize, export_only_modified);
		ExportSize_Float("GrabMinSize", style_to_export.GrabMinSize, ref_style.GrabMinSize, export_only_modified);
	}

	{
		ImGui::LogText(IM_NEWLINE "// Borders" IM_NEWLINE);

		ExportSize_Float("WindowBorderSize", style_to_export.WindowBorderSize, ref_style.WindowBorderSize, export_only_modified);
		ExportSize_Float("ChildBorderSize", style_to_export.ChildBorderSize, ref_style.ChildBorderSize, export_only_modified);
		ExportSize_Float("PopupBorderSize", style_to_export.PopupBorderSize, ref_style.PopupBorderSize, export_only_modified);
		ExportSize_Float("FrameBorderSize", style_to_export.FrameBorderSize, ref_style.FrameBorderSize, export_only_modified);
		ExportSize_Float("TabBorderSize", style_to_export.TabBorderSize, ref_style.TabBorderSize, export_only_modified);
	}
	
	{
		ImGui::LogText(IM_NEWLINE "// Rounding" IM_NEWLINE);

		ExportSize_Float("WindowRounding", style_to_export.WindowRounding, ref_style.WindowRounding, export_only_modified);
		ExportSize_Float("ChildRounding", style_to_export.ChildRounding, ref_style.ChildRounding, export_only_modified);
		ExportSize_Float("FrameRounding", style_to_export.FrameRounding, ref_style.FrameRounding, export_only_modified);
		ExportSize_Float("PopupRounding", style_to_export.PopupRounding, ref_style.PopupRounding, export_only_modified);
		ExportSize_Float("ScrollbarRounding", style_to_export.ScrollbarRounding, ref_style.ScrollbarRounding, export_only_modified);
		ExportSize_Float("GrabRounding", style_to_export.GrabRounding, ref_style.GrabRounding, export_only_modified);
		ExportSize_Float("TabRounding", style_to_export.TabRounding, ref_style.TabRounding, export_only_modified);
	}

	{
		ImGui::LogText(IM_NEWLINE "// Alignment" IM_NEWLINE);

		ExportSize_ImVec2("WindowTitleAlign", style_to_export.WindowTitleAlign, ref_style.WindowTitleAlign, export_only_modified);

		// for this one we could just save ImGuiDir number, but its more redable to have ImGuiDir_ name
		if (!export_only_modified || memcmp(&style_to_export.WindowMenuButtonPosition, &ref_style.WindowMenuButtonPosition, sizeof(ImGuiDir)) != 0)
		{
			const char* dirName = 0;
			switch (style_to_export.WindowMenuButtonPosition)
			{
			case ImGuiDir_None: dirName = "ImGuiDir_None"; break;
			case ImGuiDir_Left: dirName = "ImGuiDir_Left"; break;
			case ImGuiDir_Right: dirName = "ImGuiDir_Right"; break;
			};

			ImGui::LogText("style.%s%*s= %s;" IM_NEWLINE, "WindowMenuButtonPosition", 25 - (int)strlen("WindowMenuButtonPosition"), "", dirName);
		}

		// for this one we could just save ImGuiDir number, but its more redable to have ImGuiDir_ name
		if (!export_only_modified || memcmp(&style_to_export.ColorButtonPosition, &ref_style.ColorButtonPosition, sizeof(ImGuiDir)) != 0)
		{
			const char* dirName = 0;
			switch (style_to_export.ColorButtonPosition)
			{
			case ImGuiDir_Left: dirName = "ImGuiDir_Left"; break;
			case ImGuiDir_Right: dirName = "ImGuiDir_Right"; break;
			};

			ImGui::LogText("style.%s%*s= %s;" IM_NEWLINE, "ColorButtonPosition", 25 - (int)strlen("ColorButtonPosition"), "", dirName);
		}

		ExportSize_ImVec2("ButtonTextAlign", style_to_export.ButtonTextAlign, ref_style.ButtonTextAlign, export_only_modified);
		ExportSize_ImVec2("SelectableTextAlign", style_to_export.SelectableTextAlign, ref_style.SelectableTextAlign, export_only_modified);
	}
	
	{
		ImGui::LogText(IM_NEWLINE "// Safe Area Padding" IM_NEWLINE);

		ExportSize_ImVec2("DisplaySafeAreaPadding", style_to_export.DisplaySafeAreaPadding, ref_style.DisplaySafeAreaPadding, export_only_modified);
	}
}

void ImGuiThemeHelper::ShowCustomImGuiStyleEditor(bool* vOpen, ImGuiStyle* ref)
{
	if (ImGui::Begin("Styles Editor", vOpen))
	{
		// You can pass in a reference ImGuiStyle structure to compare to, revert to and save to (else it compares to an internally stored reference)
		auto& style = m_CurrentTheme.style;
		static ImGuiStyle ref_saved_style;
		static ImVec4 ref_Good_Color;
		static ImVec4 ref_Bad_Color;

		// Default to using internal storage as reference
		static auto init = true;
		if (init && ref == nullptr)
			ref_saved_style = style;
		init = false;
		if (ref == nullptr)
			ref = &ref_saved_style;

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

		if (ImGui::ShowStyleSelector("Colors##Selector"))
			ref_saved_style = style;
		ImGui::ShowFontSelector("Fonts##Selector");

		// Simplified Settings
		if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
			style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
		
		auto window_border = (style.WindowBorderSize > 0.0f);
		if (ImGui::Checkbox("WindowBorder", &window_border))
			style.WindowBorderSize = window_border ? 1.0f : 0.0f;

		ImGui::SameLine();

		auto frame_border = (style.FrameBorderSize > 0.0f);
		if (ImGui::Checkbox("FrameBorder", &frame_border))
			style.FrameBorderSize = frame_border ? 1.0f : 0.0f;

		ImGui::SameLine();

		auto popup_border = (style.PopupBorderSize > 0.0f);
		if (ImGui::Checkbox("PopupBorder", &popup_border))
			style.PopupBorderSize = popup_border ? 1.0f : 0.0f;

#ifdef USE_SHADOW
		// Custom Shadow
		ImGui::Checkbox("Use Shadow", &puUseShadow);
		if (puUseShadow)
		{
			ImGui::SliderFloatDefaultCompact(300.0f, "Inner Shadow", &puShadowStrength, 2.0f, 0.0f, 0.5f);
			ImGui::Checkbox("Use Texture for Shadow", &puUseTextureForShadow);
		}
#endif

		// Save/Revert button
		if (ImGui::ContrastedButton("Save Ref"))
		{
			*ref = ref_saved_style = style;
			ref_Good_Color = ImGui::CustomStyle::GoodColor;
			ref_Bad_Color = ImGui::CustomStyle::BadColor;
		}
		ImGui::SameLine();
		if (ImGui::ContrastedButton("Revert Ref"))
		{
			style = *ref;
			ImGui::CustomStyle::GoodColor = ref_Good_Color;
			ImGui::CustomStyle::BadColor = ref_Bad_Color;
		}
		ImGui::SameLine();
		ImGui::HelpMarker("Save/Revert in local non-persistent storage. Default Colors definition are not affected. Use \"Export\" below to save them somewhere.");

		ImGui::Separator();
		
		static auto output_dest = 0;
		static auto output_only_modified = true;
		if (ImGui::ContrastedButton("Export Sizes and Colors"))
		{
			if (output_dest == 0)
				ImGui::LogToClipboard();
			else
				ImGui::LogToTTY();

			ExportColors(style, *ref, output_only_modified);

			ImGui::LogText(IM_NEWLINE);

			ExportSizes(style, *ref, output_only_modified);

			ImGui::LogFinish();
		}
		ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type_size_and_colors", &output_dest, "To Clipboard\0To TTY\0");
		ImGui::SameLine(); ImGui::Checkbox("Only Modified##size_and_colors", &output_only_modified);

		ImGui::Separator();

		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Sizes"))
			{
				static auto output_dest_sizes = 0;
				static auto output_only_modified_sizes = true;
				if (ImGui::ContrastedButton("Export"))
				{
					if (output_dest_sizes == 0)
						ImGui::LogToClipboard();
					else
						ImGui::LogToTTY();

					ExportSizes(style, *ref, output_only_modified_sizes);

					ImGui::LogFinish();
				}
				ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest_sizes, "To Clipboard\0To TTY\0");
				ImGui::SameLine(); ImGui::Checkbox("Only Modified", &output_only_modified_sizes);

				ImGui::Separator();

				ImGui::BeginChild("##sizes", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
				ImGui::PushItemWidth(-160);

				ImGui::Text("Main");
				ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
				ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
				ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");

				ImGui::Text("Borders");
				ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");

				ImGui::Text("Rounding");
				ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");

				ImGui::Text("Alignment");
				ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
				auto window_menu_button_position = style.WindowMenuButtonPosition + 1;
				if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
					style.WindowMenuButtonPosition = window_menu_button_position - 1;
				ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
				ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); ImGui::HelpMarker("Alignment applies when a button is larger than its text content.");
				ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); ImGui::HelpMarker("Alignment applies when a selectable is larger than its text content.");
				
				ImGui::Text("Safe Area Padding"); ImGui::SameLine(); ImGui::HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
				ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
				
				ImGui::PopItemWidth();
				ImGui::EndChild();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Colors"))
			{
				static auto output_dest_colors = 0;
				static auto output_only_modified_colors = true;
				if (ImGui::ContrastedButton("Export"))
				{
					if (output_dest_colors == 0)
						ImGui::LogToClipboard();
					else
						ImGui::LogToTTY();

					ExportColors(style, *ref, output_only_modified_colors);

					ImGui::LogFinish();
				}
				ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest_colors, "To Clipboard\0To TTY\0");
				ImGui::SameLine(); ImGui::Checkbox("Only Modified", &output_only_modified_colors);

				ImGui::Separator();

				static ImGuiTextFilter filter;
				filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

				static auto alpha_flags = 0;
				if (ImGui::RadioButton("Opaque", alpha_flags == 0)) { alpha_flags = 0; } ImGui::SameLine();
				if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
				if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
				ImGui::HelpMarker("In the color list:\nLeft-click on colored square to open color picker,\nRight-click to open edit options menu.");

				ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
				ImGui::PushItemWidth(-160);

				DrawItem(14582, filter, "Good Color", ImGui::CustomStyle::GoodColor, ref_Good_Color, alpha_flags);
				DrawItem(114583, filter, "Bad Color", ImGui::CustomStyle::BadColor, ref_Bad_Color, alpha_flags);

				for (auto i = 0; i < ImGuiCol_COUNT; i++)
				{
					DrawItem(i, filter, ImGui::GetStyleColorName(i), style.Colors[i], ref->Colors[i], alpha_flags);
				}
				ImGui::PopItemWidth();
				ImGui::EndChild();

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::PopItemWidth();
	}
	ImGui::End();

	memcpy(&ImGui::GetStyle(), &m_CurrentTheme.style, sizeof(ImGuiStyle));
}

void ImGuiThemeHelper::DrawFileStyle() {
    bool fileColorUpdate = false;

    for (auto& it : m_CurrentTheme.fileTypeInfos) {
        fileColorUpdate |= ImGui::ColorEdit4(it.first.c_str(), &m_CurrentTheme.fileTypeInfos[it.first].color.x);
    }

    if (fileColorUpdate) {
        ApplyFileTypeColors();
    }
}