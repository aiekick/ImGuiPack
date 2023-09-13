// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright 2022-2022 Stephane Cuillerdier (aka aiekick)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "LayoutManager.h"
#include <ImGuiPack.h>

#include <imgui_internal.h>

LayoutManager::LayoutManager() = default;
LayoutManager::~LayoutManager() = default;

void LayoutManager::AddPane(
	AbstractPaneWeak vPane,
	const std::string& vName,
	const PaneCategoryName& vCategory,
	const PaneDisposal& vPaneDisposal,
	const bool& vIsOpenedDefault,
	const bool& vIsFocusedDefault)
{
	PaneFlags flag = (1 << ++m_FlagCount);
	AddPane(vPane, vName, vCategory, flag, vPaneDisposal, vIsOpenedDefault, vIsFocusedDefault);
}

void LayoutManager::AddPane(
	AbstractPaneWeak vPane,
	const std::string& vName,
	const PaneCategoryName& vCategory,
	const PaneFlags& vFlag,
	const PaneDisposal& vPaneDisposal,
	const bool& vIsOpenedDefault,
	const bool& vIsFocusedDefault)
{
	if (vFlag == 0) return;
	if (vPane.expired()) return;
	if (vName.empty()) return;
	if (m_PanesByName.find(vName) != m_PanesByName.end()) return; // pane name not already exist
	if (m_PanesByFlag.find(vFlag) != m_PanesByFlag.end()) return; // pane flag not already exist
	
	auto panePtr = vPane.lock();
	if (panePtr)
	{
		panePtr->paneName = vName;
		panePtr->paneFlag = vFlag;
		panePtr->paneDisposal = vPaneDisposal;
		panePtr->openedDefault = vIsOpenedDefault;
		panePtr->focusedDefault = vIsFocusedDefault;
        panePtr->paneCategory = vCategory;
        if (vIsOpenedDefault) {
            m_Pane_Opened_Default |= panePtr->paneFlag;
        }
        if (vIsFocusedDefault) {
            m_Pane_Focused_Default |= panePtr->paneFlag;
        }
        m_PanesByDisposal[panePtr->paneDisposal].push_back(panePtr);
		m_PanesByName[panePtr->paneName] = panePtr;
		m_PanesByFlag[panePtr->paneFlag] = panePtr;
        m_PanesInDisplayOrder[panePtr->paneCategory].push_back(panePtr);
	}
}

void LayoutManager::SetPaneDisposalSize(const PaneDisposal& vPaneDisposal, const float& vSize)
{
	if (vPaneDisposal == PaneDisposal::CENTRAL ||
		vPaneDisposal == PaneDisposal::Count)
		return;

	m_PaneDisposalSizes[(int)vPaneDisposal] = vSize;
}

void LayoutManager::RemovePane(const std::string& vName) {
    if (m_PanesByName.find(vName) != m_PanesByName.end()) {
        auto panePtr = m_PanesByName.at(vName).lock();
        if (panePtr != nullptr) {
            if (m_PanesByDisposal.find(panePtr->paneDisposal) != m_PanesByDisposal.end()) {
                auto& arr = m_PanesByDisposal.at(panePtr->paneDisposal);
                size_t idx = 0U;
                std::set<size_t> indexs;
                for (auto pane : arr) {
                    auto ptr = pane.lock();
                    if (ptr != nullptr && panePtr == ptr) {
                        indexs.emplace(idx);
					}
                    ++idx;
                }
                for (auto id : indexs) {
                    arr.erase(arr.begin() + id);
				}
            }
            if (m_PanesByName.find(panePtr->paneName) != m_PanesByName.end()) {
                m_PanesByName.erase(panePtr->paneName);
            }
            if (m_PanesByFlag.find(panePtr->paneFlag) != m_PanesByFlag.end()) {
                m_PanesByFlag.erase(panePtr->paneFlag);
            }
            if (m_PanesInDisplayOrder.find(panePtr->paneCategory) != m_PanesInDisplayOrder.end()) {
                auto& arr = m_PanesInDisplayOrder.at(panePtr->paneCategory);
                size_t idx = 0U;
                std::set<size_t> indexs;
                for (auto pane : arr) {
                    auto ptr = pane.lock();
                    if (ptr != nullptr && panePtr == ptr) {
                        indexs.emplace(idx);
                    }
                    ++idx;
                }
                for (auto id : indexs) {
                    arr.erase(arr.begin() + id);
                }
            }
            if (m_Pane_Opened_Default & panePtr->paneFlag) {
                m_Pane_Opened_Default = m_Pane_Opened_Default & ~panePtr->paneFlag;
            }
            if (m_Pane_Focused_Default & panePtr->paneFlag) {
                m_Pane_Focused_Default = m_Pane_Focused_Default & ~panePtr->paneFlag;
            }
		}
	}
}

void LayoutManager::Init(const std::string& vMenuLabel, const std::string& vDefaultMenuLabel)
{
	assert(!vMenuLabel.empty());
	assert(!vDefaultMenuLabel.empty());

	m_MenuLabel = vMenuLabel;
	m_DefaultMenuLabel = vDefaultMenuLabel;

    ImGuiContext& g = *GImGui;

    ImFileHandle f;
    if ((f = ImFileOpen(g.IO.IniFilename, "r")) == NULL) {
        m_FirstLayout = true;  // need default layout
	}
}

void LayoutManager::Unit()
{
	m_PanesByDisposal.clear();
	m_PanesByName.clear();
	m_PanesInDisplayOrder.clear();
	m_PanesByFlag.clear();
}

bool LayoutManager::InitPanes()
{
	bool res = true;

	for (const auto& pane : m_PanesByFlag)
	{
		auto panePtr = pane.second.lock();
		if (panePtr)
		{
			res &= panePtr->Init();
		}
	}

	return res;
}

void LayoutManager::UnitPanes()
{
	for (const auto& pane : m_PanesByFlag)
	{
		auto panePtr = pane.second.lock();
		if (panePtr)
		{
			panePtr->Unit();
		}
	}
}

void LayoutManager::InitAfterFirstDisplay(const ImVec2& vSize)
{
	if (m_FirstLayout)
	{
		ApplyInitialDockingLayout(vSize);
		m_FirstLayout = false;
	}

	if (m_FirstStart)
	{
		// focus after start of panes
		Internal_SetFocusedPanes(m_Pane_Focused);
		m_FirstStart = false;
	}
}

bool LayoutManager::BeginDockSpace(const ImGuiDockNodeFlags& vFlags)
{
	const auto viewport = ImGui::GetMainViewport();

	m_LastSize = viewport->Size;

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	auto host_window_flags = 0;
	host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if (vFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		host_window_flags |= ImGuiWindowFlags_NoBackground;

	char label[100 + 1];
	ImFormatString(label, 100, "DockSpaceViewport_%08X", viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	const auto res = ImGui::Begin(label, nullptr, host_window_flags);
	ImGui::PopStyleVar(3);

	m_DockSpaceID = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(m_DockSpaceID, ImVec2(0.0f, 0.0f), vFlags);

	return res;
}

void LayoutManager::EndDockSpace()
{
	ImGui::End();
}

bool LayoutManager::IsDockSpaceHoleHovered()
{
    auto  central_node_ptr = ImGui::DockBuilderGetCentralNode(m_DockSpaceID);
    if (central_node_ptr != nullptr) {
        return ImGui::IsMouseHoveringRect(central_node_ptr->Pos, central_node_ptr->Pos + central_node_ptr->Size, true);
	}
    return false;
}

void LayoutManager::ApplyInitialDockingLayout(const ImVec2& vSize)
{
	ImVec2 _size = vSize;

	if (IS_FLOAT_EQUAL(_size.x, 0.0f) || IS_FLOAT_EQUAL(_size.y, 0.0f))
	{
		if (IS_FLOAT_EQUAL(m_LastSize.x, 0.0f) || IS_FLOAT_EQUAL(m_LastSize.y, 0.0f))
		{
			return;
		}

		_size = m_LastSize;
	}

	ImGui::DockBuilderRemoveNode(m_DockSpaceID); // Clear out existing layout
	ImGui::DockBuilderAddNode(m_DockSpaceID, ImGuiDockNodeFlags_DockSpace); // Add empty node
	ImGui::DockBuilderSetNodeSize(m_DockSpaceID, _size);

	// just for readability
	const auto& left_size = m_PaneDisposalSizes[1];
	const auto& right_size = m_PaneDisposalSizes[2];
	const auto& bottom_size = m_PaneDisposalSizes[3];
	const auto& top_size = m_PaneDisposalSizes[4];

	auto dockMainID = m_DockSpaceID; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
	const auto dockLeftID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, left_size / _size.x, nullptr, &dockMainID);
	const auto dockRightID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, right_size / (_size.x - left_size), nullptr, &dockMainID);
	const auto dockBottomID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, bottom_size / _size.y, nullptr, &dockMainID);
	const auto dockTopID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Up, top_size / (_size.y - bottom_size), nullptr, &dockMainID);

	for (const auto& pane : m_PanesByName)
	{
		auto panePtr = pane.second.lock();
		if (panePtr)
		{
			switch (panePtr->paneDisposal)
			{
			case PaneDisposal::CENTRAL:
			{
				ImGui::DockBuilderDockWindow(pane.first.c_str(), dockMainID);
				break;
			}
			case PaneDisposal::LEFT:
			{
				ImGui::DockBuilderDockWindow(pane.first.c_str(), dockLeftID);
				break;
			}
			case PaneDisposal::RIGHT:
			{
				ImGui::DockBuilderDockWindow(pane.first.c_str(), dockRightID);
				break;
			}
			case PaneDisposal::BOTTOM:
			{
				ImGui::DockBuilderDockWindow(pane.first.c_str(), dockBottomID);
				break;
			}
			case PaneDisposal::TOP:
			{
				ImGui::DockBuilderDockWindow(pane.first.c_str(), dockTopID);
				break;
			}
			};
		}
	}
	
	ImGui::DockBuilderFinish(m_DockSpaceID);

	pane_Shown = m_Pane_Opened_Default; // will show when pane will be passed
	m_Pane_Focused = m_Pane_Focused_Default;

	Internal_SetFocusedPanes(m_Pane_Focused);
}

template<typename T>
static bool LayoutManager_MenuItem(const char* label, const char* shortcut, T* vContainer, T vFlag, bool vOnlyOneSameTime = false)
{
	bool selected = *vContainer & vFlag;
	const bool res = ImGui::MenuItem(label, shortcut, &selected, true);
	if (res) 
	{
		if (selected) 
		{
			if (vOnlyOneSameTime) 
				*vContainer = vFlag; // set
			else 
				*vContainer = (T)(*vContainer | vFlag);// add
		}
		else if (!vOnlyOneSameTime) 
				*vContainer = (T)(*vContainer & ~vFlag); // remove
	}
	return res;
}

void LayoutManager::DisplayMenu(const ImVec2& vSize)
{
	if (ImGui::BeginMenu(m_MenuLabel.c_str()))
	{
		if (ImGui::MenuItem(m_DefaultMenuLabel.c_str()))
		{
			ApplyInitialDockingLayout(vSize);
		}

		ImGui::Separator();

		bool _menuOpened = false;
		for (const auto& paneCategory : m_PanesInDisplayOrder)
		{
			_menuOpened = false;

			if (!paneCategory.first.empty())
			{
				_menuOpened = ImGui::BeginMenu(paneCategory.first.c_str());
			}

			if (paneCategory.first.empty() || _menuOpened)
			{
				for (auto pane : paneCategory.second)
				{
					auto panePtr = pane.lock();
					if (panePtr && panePtr->CanWeDisplay())
					{
						LayoutManager_MenuItem<PaneFlags>(panePtr->paneName.c_str(), "", &pane_Shown, panePtr->paneFlag);
					}
				}
			}
			
			if (_menuOpened)
			{
				ImGui::EndMenu();
			}
		}
		
		ImGui::EndMenu();
	}
}

bool LayoutManager::DrawPanes(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr, const std::string& vUserDatas) {
    bool change = false;
	for (const auto& pane : m_PanesByFlag)
	{
		auto panePtr = pane.second.lock();
		if (panePtr && panePtr->CanWeDisplay())
		{
			if (panePtr->showPaneAtFirstCall)
			{
				ShowSpecificPane(panePtr->paneFlag);
				panePtr->showPaneAtFirstCall = false;
			}

			if (panePtr->hidePaneAtFirstCall)
			{
				HideSpecificPane(panePtr->paneFlag);
				panePtr->hidePaneAtFirstCall = false;
			}

			change = panePtr->DrawPanes(vCurrentFrame, pane_Shown, vContextPtr, vUserDatas);
		}
	}
	return change;
}

bool LayoutManager::DrawDialogsAndPopups(const uint32_t& vCurrentFrame, const ImVec2& vMaxSize, ImGuiContext* vContextPtr, const std::string& vUserDatas) {
    bool change = false;
    for (const auto& pane : m_PanesByFlag)
	{
		auto panePtr = pane.second.lock();
		if (panePtr && panePtr->CanWeDisplay())
		{
            change = panePtr->DrawDialogsAndPopups(vCurrentFrame, vMaxSize, vContextPtr, vUserDatas);
		}
    }
    return change;
}

bool LayoutManager::DrawWidgets(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr, const std::string& vUserDatas) {
    bool change = false;
    for (const auto& pane : m_PanesByFlag)
	{
		auto panePtr = pane.second.lock();
		if (panePtr && panePtr->CanWeDisplay())
		{
            change = panePtr->DrawWidgets(vCurrentFrame, vContextPtr, vUserDatas);
		}
    }
    return change;
}

bool LayoutManager::DrawOverlays(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr, const std::string& vUserDatas) {
    bool change = false;
    for (const auto& pane : m_PanesByFlag) {
        auto panePtr = pane.second.lock();
        if (panePtr && panePtr->CanWeDisplay()) {
            change = panePtr->DrawOverlays(vCurrentFrame, vRect, vContextPtr, vUserDatas);
        }
    }
    return change;
}

void LayoutManager::ShowSpecificPane(const PaneFlags& vPane)
{
	pane_Shown = (PaneFlags)((int32_t)pane_Shown | (int32_t)vPane);
}

void LayoutManager::HideSpecificPane(const PaneFlags& vPane)
{
	pane_Shown = (PaneFlags)((int32_t)pane_Shown & ~(int32_t)vPane);
}

void LayoutManager::FocusSpecificPane(const PaneFlags& vPane)
{
	ShowSpecificPane(vPane);

	if (m_PanesByFlag.find(vPane) != m_PanesByFlag.end())
	{
		auto panePtr = m_PanesByFlag.at(vPane).lock();
		if (panePtr)
		{
			FocusSpecificPane(panePtr->paneName);
		}
	}
}

void LayoutManager::ShowAndFocusSpecificPane(const PaneFlags& vPane)
{
	ShowSpecificPane(vPane);
	FocusSpecificPane(vPane);
}

bool LayoutManager::IsSpecificPaneFocused(const PaneFlags& vPane)
{
	if (m_PanesByFlag.find(vPane) != m_PanesByFlag.end())
	{
		auto panePtr = m_PanesByFlag.at(vPane).lock();
		if (panePtr)
		{
			return IsSpecificPaneFocused(panePtr->paneName);
		}
	}

	return false;
}

void LayoutManager::AddSpecificPaneToExisting(const std::string& vNewPane, const std::string& vExistingPane)
{
	ImGuiWindow* window = ImGui::FindWindowByName(vExistingPane.c_str());
	if (window)
	{
		auto dockid = window->DockId;
		ImGui::DockBuilderDockWindow(vNewPane.c_str(), dockid);
	}
}

///////////////////////////////////////////////////////
//// PRIVATE //////////////////////////////////////////
///////////////////////////////////////////////////////

bool LayoutManager::IsSpecificPaneFocused(const std::string& vlabel)
{
	ImGuiWindow* window = ImGui::FindWindowByName(vlabel.c_str());
	if (window)
	{
		return 
			window->DockTabIsVisible || 
			window->ViewportOwned;
	}
	return false;
}

void LayoutManager::FocusSpecificPane(const std::string& vlabel)
{
	ImGuiWindow* window = ImGui::FindWindowByName(vlabel.c_str());
	if (window)
	{
		if(!window->DockTabIsVisible)
			ImGui::FocusWindow(window);
	}
}

///////////////////////////////////////////////////////
//// CONFIGURATION PRIVATE ////////////////////////////
///////////////////////////////////////////////////////

PaneFlags LayoutManager::Internal_GetFocusedPanes()
{
	PaneFlags flag = 0;

	for (const auto& pane : m_PanesByFlag)
	{
		auto panePtr = pane.second.lock();
		if (panePtr && IsSpecificPaneFocused(panePtr->paneName))
			flag = (PaneFlags)((int32_t)flag | (int32_t)pane.first);
	}

	return flag;
}

void LayoutManager::Internal_SetFocusedPanes(const PaneFlags& vActivePanes)
{
	for (const auto& pane : m_PanesByFlag)
	{
		auto panePtr = pane.second.lock();
		if (panePtr && vActivePanes & pane.first)
			FocusSpecificPane(panePtr->paneName);
	}
}

///////////////////////////////////////////////////////
//// CONFIGURATION PUBLIC /////////////////////////////
///////////////////////////////////////////////////////

#ifdef USE_XML_CONFIG

std::string LayoutManager::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	std::string str;

	if (vUserDatas == "app")
	{
		str += vOffset + "<layout>\n";
		m_Pane_Focused = Internal_GetFocusedPanes();
		str += vOffset + "\t<panes opened=\"" + ct::ivariant((int32_t)pane_Shown).GetS() + "\" active=\"" + ct::ivariant((int32_t)m_Pane_Focused).GetS() + "\"/>\n";
		str += vOffset + "</layout>\n";
	}
	else if (vUserDatas == "project")
	{
		// per pane settings
	}

	return str;
}

bool LayoutManager::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
{
	// The value of this child identifies the name of this element
	std::string strName = "";
	std::string strValue = "";
	std::string strParentName = "";

	strName = vElem->Value();
	if (vElem->GetText())
		strValue = vElem->GetText();
	if (vParent != 0)
		strParentName = vParent->Value();

	if (vUserDatas == "app")
	{
		if (strParentName == "layout")
		{
			for (const tinyxml2::XMLAttribute* attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next())
			{
				std::string attName = attr->Name();
				std::string attValue = attr->Value();

				if (attName == "opened") {
                    pane_Shown = (PaneFlags)ct::ivariant(attValue).GetI();
                }
                else if (attName == "active") {
                    m_Pane_Focused = (PaneFlags)ct::ivariant(attValue).GetI();
                }
			}
		}
	}
	else if (vUserDatas == "project")
	{

	}

	return true;
}

#endif // USE_XML_CONFIG
