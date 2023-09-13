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

#pragma once

#include <ImGuiPack.h>

#ifdef USE_XML_CONFIG
#    include <ctools/ConfigAbstract.h>
#endif  // USE_XML_CONFIG

#include <AbstractPane.h>
#include <imgui.h>
#include <array>
#include <string>
#include <map>

class ProjectFile;
class IMGUI_API LayoutManager
#ifdef USE_XML_CONFIG
    : public conf::ConfigAbstract
#endif // USE_XML_CONFIG
{
private:
	ImGuiID m_DockSpaceID = 0;
	bool m_FirstLayout = false;
	bool m_FirstStart = true;
	std::string m_MenuLabel;
	std::string m_DefaultMenuLabel;
	std::array<float, (size_t)PaneDisposal::Count> m_PaneDisposalSizes = 
	{	0.0f, // central size is ignored because dependant of others
		200.0f, // left size
		200.0f, // right size
		200.0f, // bottom size
		200.0f // top size
	};

protected:
    std::map<PaneDisposal, std::vector<AbstractPaneWeak>> m_PanesByDisposal;
	std::map<std::string, AbstractPaneWeak> m_PanesByName;
	std::map<PaneCategoryName, std::vector<AbstractPaneWeak>> m_PanesInDisplayOrder;
	std::map<PaneFlags, AbstractPaneWeak> m_PanesByFlag;
	int32_t m_FlagCount = 0U;

public:
	PaneFlags m_Pane_Focused_Default = 0;
	PaneFlags m_Pane_Opened_Default = 0;
	PaneFlags pane_Shown = 0;
	PaneFlags m_Pane_Focused = 0;
	PaneFlags m_Pane_Hovered = 0;
	PaneFlags m_Pane_LastHovered = 0;
	ImVec2 m_LastSize;

public:
	void AddPane(
		AbstractPaneWeak vPane,
		const std::string& vName,
		const PaneCategoryName& vCategory,
		const PaneDisposal& vPaneDisposal,
		const bool& vIsOpenedDefault,
		const bool& vIsFocusedDefault);
	void AddPane(
		AbstractPaneWeak vPane,
		const std::string& vName,
		const PaneCategoryName& vCategory,
		const PaneFlags& vFlag,
		const PaneDisposal& vPaneDisposal,
		const bool& vIsOpenedDefault,
		const bool& vIsFocusedDefault);
	void SetPaneDisposalSize(const PaneDisposal& vPaneDisposal, const float& vSize);
	void RemovePane(const std::string& vName);

public
        :
	void Init(const std::string& vMenuLabel, const std::string& vDefaultMenuLabel);
	void Unit();

	bool InitPanes();
	void UnitPanes();

	void InitAfterFirstDisplay(const ImVec2& vSize);
	bool BeginDockSpace(const ImGuiDockNodeFlags& vFlags);
	void EndDockSpace();
	bool IsDockSpaceHoleHovered();

	void ApplyInitialDockingLayout(const ImVec2& vSize = ImVec2(0, 0));

	virtual void DisplayMenu(const ImVec2& vSize);
    virtual bool DrawWidgets(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr = nullptr, const std::string& vUserDatas = {});
    virtual bool DrawOverlays(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr = nullptr, const std::string& vUserDatas = {});
    virtual bool DrawPanes(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr = nullptr, const std::string& vUserDatas = {});
    virtual bool DrawDialogsAndPopups(const uint32_t& vCurrentFrame, const ImVec2& vMaxSize, ImGuiContext* vContextPtr = nullptr, const std::string& vUserDatas = {});

public:
	void ShowSpecificPane(const PaneFlags& vPane);
	void HideSpecificPane(const PaneFlags& vPane);
	void FocusSpecificPane(const PaneFlags& vPane);
	void FocusSpecificPane(const std::string& vlabel);
	void ShowAndFocusSpecificPane(const PaneFlags& vPane);
	bool IsSpecificPaneFocused(const PaneFlags& vPane);
	bool IsSpecificPaneFocused(const std::string& vlabel);
	void AddSpecificPaneToExisting(const std::string& vNewPane, const std::string& vExistingPane);

private: // configuration
	PaneFlags Internal_GetFocusedPanes();
	void Internal_SetFocusedPanes(const PaneFlags& vActivePanes);

#ifdef USE_XML_CONFIG
public: // configuration
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
    bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
#endif // USE_XML_CONFIG

public: // singleton
	static LayoutManager *Instance(LayoutManager* vCopy = nullptr, bool vForce = false)
	{
		static LayoutManager _instance;
		static LayoutManager* _instance_copy = nullptr;
		if (vCopy || vForce)
		{
			_instance_copy = vCopy;
		}
		if (_instance_copy)
		{
			return _instance_copy;
		}
		return &_instance;
	}

protected:
	LayoutManager(); // Prevent construction
	LayoutManager(const LayoutManager&) = delete;
	LayoutManager& operator =(const LayoutManager&) = delete;
	virtual ~LayoutManager(); // Prevent unwanted destruction
};

