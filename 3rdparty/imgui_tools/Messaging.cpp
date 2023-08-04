// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2022-2023 Stephane Cuillerdier (aka Aiekick)
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

#include "Messaging.h"
#include <ImWidgets.h>
#include <LayoutManager.h>
#include <imgui_internal.h>
#include <forward_list>

#ifdef MESSAGING_CONFIG_HEADER
#include MESSAGING_CONFIG_HEADER
#else
#include "MessagingConfigHeader.h"
#endif  // MESSAGING_CONFIG_HEADER

///////////////////////////////////////////////////////////////////////////////////////////
///// STATIC //////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

int Messaging::sMessagePaneId = 0;

///////////////////////////////////////////////////////////////////////////////////////////
///// CONSTRUCTORS ////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

Messaging::Messaging() {
    SortFields(m_SortingField);
}

Messaging::~Messaging() = default;

///////////////////////////////////////////////////////////////////////////////////////////
///// PRIVATE /////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

static char Messaging_Message_Buffer[2048] = "\0";

void Messaging::AddMessage(MessageTypeEnum vType, bool vSelect, const MessageData& vDatas, const MessageFunc& vFunction, const char* fmt, va_list args) {
    const auto size = vsnprintf(Messaging_Message_Buffer, 2047, fmt, args);
    if (size > 0) {
        AddMessage(std::string(Messaging_Message_Buffer, size), vType, vSelect, vDatas, vFunction);
    }
}

void Messaging::AddMessage(const std::string& vMsg, MessageTypeEnum vType, bool vSelect, const MessageData& vDatas, const MessageFunc& vFunction) {
    if (vSelect) {
        currentMsgIdx = (int32_t)puMessages.size();
    }

    puMessages.emplace_back(vMsg, vType, vDatas, vFunction);
}

bool Messaging::DrawMessage(const size_t& vMsgIdx) {
    auto res = false;

    if (vMsgIdx < puMessages.size()) {
        res |= DrawMessage(puMessages[vMsgIdx]);
    }

    return res;
}

bool Messaging::DrawMessage(const MessageBlock& vMsg) {
    if (std::get<1>(vMsg) == MessageTypeEnum::MESSAGE_TYPE_INFOS) {
        ImGui::Text("%s ", BAR_LABEL_INFOS);
    } else if (std::get<1>(vMsg) == MessageTypeEnum::MESSAGE_TYPE_WARNING) {
        ImGui::PushStyleColor(ImGuiCol_Text, m_WarningColor);
        ImGui::Text("%s ", BAR_LABEL_WARNINGS);
        ImGui::PopStyleColor();
    } else if (std::get<1>(vMsg) == MessageTypeEnum::MESSAGE_TYPE_ERROR) {
        ImGui::PushStyleColor(ImGuiCol_Text, m_ErrorColor);
        ImGui::Text("%s ", BAR_LABEL_ERROR);
        ImGui::PopStyleColor();
    }

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (!(window->Flags & ImGuiWindowFlags_MenuBar))
        ImGui::SameLine();  // used only for when displayed in list. no effect when diplsayed in status bar

    ImGui::PushID(&vMsg);
    const auto check = ImGui::Selectable_FramedText("%s##Messaging", std::get<0>(vMsg).c_str());
    ImGui::PopID();
    if (check) {
        LayoutManager::Instance()->ShowAndFocusSpecificPane(sMessagePaneId);
        const auto datas = std::get<2>(vMsg);
        const auto& func = std::get<3>(vMsg);
        if (func)
            func(datas);
    }
    return check;
}

///////////////////////////////////////////////////////////////////////////////////////////
///// PUBLIC //////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void Messaging::DrawBar() {
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);

    ImGui::Text("Messages :");

    if (ImGui::MenuItem(MENU_LABEL_REFRESH "##Refresh")) {
        LayoutManager::Instance()->ShowAndFocusSpecificPane(sMessagePaneId);
        Clear();
    }

    if (!puMessages.empty()) {
        // on type of message only
        if (puMessageExistFlags == MESSAGE_EXIST_INFOS || puMessageExistFlags == MESSAGE_EXIST_WARNING || puMessageExistFlags == MESSAGE_EXIST_ERROR) {
            if (ImGui::MenuItem(MENU_LABEL_CLEAR "##clear")) {
                Clear();
            }
        } else {
            if (ImGui::BeginMenu(MENU_LABEL_CLEAR "##clear")) {
                if (ImGui::MenuItem("All"))
                    Clear();
                ImGui::Separator();
                if (puMessageExistFlags & MESSAGE_EXIST_INFOS)
                    if (ImGui::MenuItem("Infos"))
                        ClearInfos();
                if (puMessageExistFlags & MESSAGE_EXIST_WARNING)
                    if (ImGui::MenuItem("Warnings"))
                        ClearWarnings();
                if (puMessageExistFlags & MESSAGE_EXIST_ERROR)
                    if (ImGui::MenuItem("Errors"))
                        ClearErrors();

                ImGui::EndMenu();
            }
        }
    }
    if (!puMessages.empty()) {
        if (puMessages.size() > 1) {
            if (ImGui::MenuItem(MENU_LABEL_ARROW_LEFT "##left")) {
                LayoutManager::Instance()->ShowAndFocusSpecificPane(sMessagePaneId);
                currentMsgIdx = ImMax(--currentMsgIdx, 0);
            }
            if (ImGui::MenuItem(MENU_LABEL_ARROW_RIGHT "##right")) {
                LayoutManager::Instance()->ShowAndFocusSpecificPane(sMessagePaneId);
                currentMsgIdx = ImMax(++currentMsgIdx, (int32_t)puMessages.size() - 1);
            }
            if (ImGui::BeginMenu(MENU_LABEL_ARROW_UP "##up")) {
                LayoutManager::Instance()->ShowAndFocusSpecificPane(sMessagePaneId);
                for (auto& msg : puMessages) {
                    if (DrawMessage(msg))
                        break;
                }
                ImGui::EndMenu();
            }
        }
        currentMsgIdx = ImClamp(currentMsgIdx, 0, (int32_t)puMessages.size() - 1);
        DrawMessage(currentMsgIdx);
    }
}

void Messaging::SortFields(SortingFieldEnum vSortingField, bool vCanChangeOrder) {
    if (vSortingField != SortingFieldEnum::FIELD_NONE) {
        m_HeaderIdString      = " Id";
        m_HeaderTypeString    = " Type";
        m_HeaderMessageString = " Message";
    }

    if (vSortingField == SortingFieldEnum::FIELD_ID) {
        if (vCanChangeOrder && m_SortingField == vSortingField)
            m_SortingDirection[0] = !m_SortingDirection[0];

        if (m_SortingDirection[0]) {
            m_HeaderIdString = MENU_LABEL_ARROW_BOTTOM + m_HeaderIdString;

            /*std::sort(m_FileList.begin(), m_FileList.end(),
                [](const FileInfoStruct& a, const FileInfoStruct& b) -> bool
                {
                    if (a.fileName[0] == '.' && b.fileName[0] != '.') return true;
                    if (a.fileName[0] != '.' && b.fileName[0] == '.') return false;
                    if (a.fileName[0] == '.' && b.fileName[0] == '.')
                    {
                        if (a.fileName.length() == 1) return false;
                        if (b.fileName.length() == 1) return true;
                        return (stricmp(a.fileName.c_str() + 1, b.fileName.c_str() + 1) < 0);
                    }

                    if (a.type != b.type) return (a.type == 'd'); // directory in first
                    return (stricmp(a.fileName.c_str(), b.fileName.c_str()) < 0); // sort in insensitive case
                });*/
        } else {
            m_HeaderIdString = MENU_LABEL_ARROW_UP + m_HeaderIdString;

            /*std::sort(m_FileList.begin(), m_FileList.end(),
                [](const FileInfoStruct& a, const FileInfoStruct& b) -> bool
                {
                    if (a.fileName[0] == '.' && b.fileName[0] != '.') return false;
                    if (a.fileName[0] != '.' && b.fileName[0] == '.') return true;
                    if (a.fileName[0] == '.' && b.fileName[0] == '.')
                    {
                        if (a.fileName.length() == 1) return true;
                        if (b.fileName.length() == 1) return false;
                        return (stricmp(a.fileName.c_str() + 1, b.fileName.c_str() + 1) > 0);
                    }

                    if (a.type != b.type) return (a.type != 'd'); // directory in last
                    return (stricmp(a.fileName.c_str(), b.fileName.c_str()) > 0); // sort in insensitive case
                });*/
        }
    } else if (vSortingField == SortingFieldEnum::FIELD_TYPE) {
        if (vCanChangeOrder && m_SortingField == vSortingField)
            m_SortingDirection[1] = !m_SortingDirection[1];

        if (m_SortingDirection[1]) {
            m_HeaderTypeString = MENU_LABEL_ARROW_BOTTOM + m_HeaderTypeString;
        } else {
            m_HeaderTypeString = MENU_LABEL_ARROW_UP + m_HeaderTypeString;
        }
    } else if (vSortingField == SortingFieldEnum::FIELD_MSG) {
        if (vCanChangeOrder && m_SortingField == vSortingField)
            m_SortingDirection[2] = !m_SortingDirection[2];

        if (m_SortingDirection[2]) {
            m_HeaderMessageString = MENU_LABEL_ARROW_BOTTOM + m_HeaderMessageString;
        } else {
            m_HeaderMessageString = MENU_LABEL_ARROW_UP + m_HeaderMessageString;
        }
    }

    if (vSortingField != SortingFieldEnum::FIELD_NONE) {
        m_SortingField = vSortingField;
    }

    UpdateFilteredMessages();
}

void Messaging::UpdateFilteredMessages() {
    if (!puMessages.empty()) {
        puFilteredMessages.clear();

        for (const auto& msg : puMessages) {
            const auto& type = std::get<1>(msg);

            if (type == MessageTypeEnum::MESSAGE_TYPE_INFOS && (puMessageExistFlags & MESSAGE_EXIST_INFOS)) {
                puFilteredMessages.push_back(msg);
            } else if (type == MessageTypeEnum::MESSAGE_TYPE_ERROR && (puMessageExistFlags & MESSAGE_EXIST_ERROR)) {
                puFilteredMessages.push_back(msg);
            } else if (type == MessageTypeEnum::MESSAGE_TYPE_WARNING && (puMessageExistFlags & MESSAGE_EXIST_WARNING)) {
                puFilteredMessages.push_back(msg);
            }
        }
    }
}

void Messaging::AddToFilteredMessages(const MessageBlock& vMessageBlock) {
    const auto& type = std::get<1>(vMessageBlock);

    if (type == MessageTypeEnum::MESSAGE_TYPE_INFOS && (puMessageExistFlags & MESSAGE_EXIST_INFOS)) {
        puFilteredMessages.push_back(vMessageBlock);
    } else if (type == MessageTypeEnum::MESSAGE_TYPE_ERROR && (puMessageExistFlags & MESSAGE_EXIST_ERROR)) {
        puFilteredMessages.push_back(vMessageBlock);
    } else if (type == MessageTypeEnum::MESSAGE_TYPE_WARNING && (puMessageExistFlags & MESSAGE_EXIST_WARNING)) {
        puFilteredMessages.push_back(vMessageBlock);
    }
}

void Messaging::DrawConsole() {
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);

    if (ImGui::BeginMenuBar()) {
        if (!puMessages.empty()) {
            // on type of message only
            if (puMessageExistFlags == MESSAGE_EXIST_INFOS || puMessageExistFlags == MESSAGE_EXIST_WARNING || puMessageExistFlags == MESSAGE_EXIST_ERROR) {
                if (ImGui::MenuItem(MENU_LABEL_CLEAR "##clear")) {
                    Clear();
                }
            } else {
                if (ImGui::BeginMenu(MENU_LABEL_CLEAR "##clear")) {
                    if (ImGui::MenuItem("All"))
                        Clear();
                    ImGui::Separator();
                    if (puMessageExistFlags & MESSAGE_EXIST_INFOS)
                        if (ImGui::MenuItem("Infos"))
                            ClearInfos();
                    if (puMessageExistFlags & MESSAGE_EXIST_WARNING)
                        if (ImGui::MenuItem("Warnings"))
                            ClearWarnings();
                    if (puMessageExistFlags & MESSAGE_EXIST_ERROR)
                        if (ImGui::MenuItem("Errors"))
                            ClearErrors();

                    ImGui::EndMenu();
                }
            }
        }

        bool needUpdate = false;
        needUpdate |= ImGui::RadioButtonLabeled_BitWize<MessageExistFlags>(0.0f, "Infos", nullptr, &puMessageExistFlags, MESSAGE_EXIST_INFOS);
        needUpdate |= ImGui::RadioButtonLabeled_BitWize<MessageExistFlags>(0.0f, "Warnings", nullptr, &puMessageExistFlags, MESSAGE_EXIST_WARNING);
        needUpdate |= ImGui::RadioButtonLabeled_BitWize<MessageExistFlags>(0.0f, "Errors", nullptr, &puMessageExistFlags, MESSAGE_EXIST_ERROR);
        if (needUpdate) {
            UpdateFilteredMessages();
        }

        ImGui::EndMenuBar();
    }

    // std::tuple<std::string, MessageTypeEnum, MessageData, MessageFunc>

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Hideable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoHostExtendY;
    if (ImGui::BeginTable("##messagesTable", 3, flags)) {
        ImGui::TableSetupScrollFreeze(0, 1);  // Make header always visible
        ImGui::TableSetupColumn(m_HeaderIdString.c_str(), ImGuiTableColumnFlags_WidthFixed, -1, 0);
        ImGui::TableSetupColumn(m_HeaderTypeString.c_str(), ImGuiTableColumnFlags_WidthFixed, -1, 0);
        ImGui::TableSetupColumn(m_HeaderMessageString.c_str(), ImGuiTableColumnFlags_WidthStretch, -1, 1);

        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
        for (int column = 0; column < 3; column++) {
            ImGui::TableSetColumnIndex(column);
            const char* column_name = ImGui::TableGetColumnName(column);  // Retrieve name passed to TableSetupColumn()
            ImGui::PushID(column);
            ImGui::TableHeader(column_name);
            ImGui::PopID();
            if (ImGui::IsItemClicked()) {
                SortFields((SortingFieldEnum)(column + 1), true);
            }
        }

        if (!puFilteredMessages.empty()) {
            bool _clicked     = false;
            int _indexClicked = -1;

            static ImGuiListClipper s_messagesListClipper;
            s_messagesListClipper.Begin((int)puFilteredMessages.size(), ImGui::GetTextLineHeightWithSpacing());
            while (s_messagesListClipper.Step()) {
                for (int i = s_messagesListClipper.DisplayStart; i < s_messagesListClipper.DisplayEnd; i++) {
                    if (i < 0)
                        continue;

                    const auto& msg  = puFilteredMessages[i];
                    const auto& type = std::get<1>(msg);

                    if (type == MessageTypeEnum::MESSAGE_TYPE_INFOS && (puMessageExistFlags & MESSAGE_EXIST_INFOS)) {
                        if (ImGui::TableNextColumn())  // id
                        {
                            ImGui::Text("%i", i);
                        }
                        if (ImGui::TableNextColumn())  // type
                        {
                            if (ImGui::Selectable("Infos", false, ImGuiSelectableFlags_SpanAllColumns)) {
                                _clicked      = true;
                                _indexClicked = i;
                            }
                        }
                        if (ImGui::TableNextColumn())  // str
                        {
                            auto str = std::get<0>(msg);
                            ImGui::Text("%s", str.c_str());
                        }
                    } else if (type == MessageTypeEnum::MESSAGE_TYPE_ERROR && (puMessageExistFlags & MESSAGE_EXIST_ERROR)) {
                        if (ImGui::TableNextColumn())  // id
                        {
                            ImGui::Text("%i", i);
                        }
                        if (ImGui::TableNextColumn())  // type
                        {
                            if (ImGui::Selectable("Error", false, ImGuiSelectableFlags_SpanAllColumns)) {
                                _clicked      = true;
                                _indexClicked = i;
                            }
                        }
                        if (ImGui::TableNextColumn())  // str
                        {
                            auto str = std::get<0>(msg);
                            ImGui::Text("%s", str.c_str());
                        }
                    } else if (type == MessageTypeEnum::MESSAGE_TYPE_WARNING && (puMessageExistFlags & MESSAGE_EXIST_WARNING)) {
                        if (ImGui::TableNextColumn())  // id
                        {
                            ImGui::Text("%i", i);
                        }
                        if (ImGui::TableNextColumn())  // type
                        {
                            if (ImGui::Selectable("Warnings", false, ImGuiSelectableFlags_SpanAllColumns)) {
                                _clicked      = true;
                                _indexClicked = i;
                            }
                        }
                        if (ImGui::TableNextColumn())  // str
                        {
                            auto str = std::get<0>(msg);
                            ImGui::Text("%s", str.c_str());
                        }
                    }
                }
            }
            s_messagesListClipper.End();

            if (_clicked && _indexClicked > -1) {
                const auto& msg  = puFilteredMessages[_indexClicked];
                const auto datas = std::get<2>(msg);
                const auto& func = std::get<3>(msg);
                if (func)
                    func(datas);
            }
        }

        ImGui::EndTable();
    }
}

void Messaging::AddInfos(const bool& vSelect, const MessageData& vDatas, const MessageFunc& vFunction, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    AddMessage(MessageTypeEnum::MESSAGE_TYPE_INFOS, vSelect, vDatas, vFunction, fmt, args);
    va_end(args);
    puMessageExistFlags = (MessageExistFlags)(puMessageExistFlags | MESSAGE_EXIST_INFOS);
    SortFields(m_SortingField);
}

void Messaging::AddWarning(const bool& vSelect, const MessageData& vDatas, const MessageFunc& vFunction, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    AddMessage(MessageTypeEnum::MESSAGE_TYPE_WARNING, vSelect, vDatas, vFunction, fmt, args);
    va_end(args);
    puMessageExistFlags = (MessageExistFlags)(puMessageExistFlags | MESSAGE_EXIST_WARNING);
    SortFields(m_SortingField);
}

void Messaging::AddError(const bool& vSelect, const MessageData& vDatas, const MessageFunc& vFunction, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    AddMessage(MessageTypeEnum::MESSAGE_TYPE_ERROR, vSelect, vDatas, vFunction, fmt, args);
    va_end(args);
    puMessageExistFlags = (MessageExistFlags)(puMessageExistFlags | MESSAGE_EXIST_ERROR);
    SortFields(m_SortingField);
}

void Messaging::ClearErrors() {
    std::forward_list<int> msgToErase;
    auto idx = 0;
    for (auto& msg : puMessages) {
        if (std::get<1>(msg) == MessageTypeEnum::MESSAGE_TYPE_ERROR)
            msgToErase.push_front(idx);
        ++idx;
    }

    for (auto& id : msgToErase) {
        puMessages.erase(puMessages.begin() + id);
    }

    puMessageExistFlags &= ~MESSAGE_EXIST_ERROR;
}

void Messaging::ClearWarnings() {
    std::forward_list<int> msgToErase;
    auto idx = 0;
    for (auto& msg : puMessages) {
        if (std::get<1>(msg) == MessageTypeEnum::MESSAGE_TYPE_WARNING)
            msgToErase.push_front(idx);
        ++idx;
    }

    for (auto& id : msgToErase) {
        puMessages.erase(puMessages.begin() + id);
    }

    puMessageExistFlags &= ~MESSAGE_EXIST_WARNING;
}

void Messaging::ClearInfos() {
    std::forward_list<int> msgToErase;
    auto idx = 0;
    for (auto& msg : puMessages) {
        if (std::get<1>(msg) == MessageTypeEnum::MESSAGE_TYPE_INFOS)
            msgToErase.push_front(idx);
        ++idx;
    }

    for (auto& id : msgToErase) {
        puMessages.erase(puMessages.begin() + id);
    }

    puMessageExistFlags &= ~MESSAGE_EXIST_INFOS;
}

void Messaging::Clear() {
    puMessages.clear();
    puFilteredMessages.clear();
}