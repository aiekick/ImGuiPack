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

#include <memory> // smart ptr
#include <string>
#include <cstdint> // int32_t

typedef int PaneFlags;
enum class PaneDisposal
{
	CENTRAL = 0,
	LEFT,
	RIGHT,
	BOTTOM,
	TOP,
	Count
};

class AbstractPane;
typedef std::shared_ptr<AbstractPane> AbstractPanePtr;
typedef std::weak_ptr<AbstractPane> AbstractPaneWeak;

class BaseNode;
class ProjectFile;
class AbstractPane
{
public:
	std::string m_PaneName;
	PaneFlags m_PaneFlag = 0;
	PaneDisposal m_PaneDisposal = PaneDisposal::CENTRAL;
	bool m_OpenedDefault = false;
	bool m_FocusedDefault = false;
	bool m_ShowPaneAtFirstCall = false;
	bool m_HidePaneAtFirstCall = false;

public:
	int m_PaneWidgetId = 0;
	int NewWidgetId() { return ++m_PaneWidgetId; }

public:
	virtual bool Init() = 0;
	virtual void Unit() = 0;
    virtual int32_t DrawPanes(const uint32_t& vCurrentFrame, const int32_t& vWidgetId, const std::string& vUserDatas, PaneFlags& vInOutPaneShown) = 0;
    virtual void DrawDialogsAndPopups(const uint32_t& vCurrentFrame, const std::string& vUserDatas) = 0;
    virtual int32_t DrawWidgets(const uint32_t& vCurrentFrame, const int32_t& vWidgetId, const std::string& vUserDatas) = 0;

public:
	virtual void ShowPane() { m_ShowPaneAtFirstCall = true; };
	virtual void HidePane() { m_HidePaneAtFirstCall = true; };
	virtual bool CanWeDisplay() { return true; };
};