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
    m_SortFields(m_SortingField);
}

Messaging::~Messaging() = default;

///////////////////////////////////////////////////////////////////////////////////////////
///// PUBLIC //////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void Messaging::DrawStatusBar() {
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    if (!m_Messages.empty()) {
        for (const auto& cat : m_CategorieInfos) {
            if (cat.second.count > 0U) {
                const auto& str = cat.second.icon + " " + std::to_string(cat.second.count) + "##icon";
                ImGui::PushStyleColor(ImGuiCol_Text, cat.second.color);
                const auto& use = ImGui::MenuItem(str.c_str());
                ImGui::PopStyleColor();
                if (use) {
                    m_MessageExistFlags = cat.second.flag;
                    LayoutManager::Instance()->ShowAndFocusSpecificPane(sMessagePaneId);
                    m_CurrentMsgIdx = ImMax(--m_CurrentMsgIdx, 0);
                    m_UpdateFilteredMessages();
                } 
            }
        }
    }
}

void Messaging::DrawConsolePane() {
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);

    if (m_FilteredMessages.empty() && !m_Messages.empty()) {
        m_UpdateFilteredMessages();
    }

    if (ImGui::BeginMenuBar()) {
        if (!m_Messages.empty()) {
            if (ImGui::BeginMenu(MENU_LABEL_CLEAR "##clear")) {
                if (ImGui::MenuItem("Clear All"))
                    Clear();
                ImGui::Separator();
                for (const auto& cat : m_CategorieInfos) {
                    if (cat.second.count > 0U) {
                        ImGui::PushStyleColor(ImGuiCol_Text, cat.second.color);
                        if (ImGui::MenuItem(cat.second.name.c_str())) {
                            ClearMessagesOfType(cat.second.type);
                        }
                        ImGui::PopStyleColor();
                    }
                }

                ImGui::EndMenu();
            }
        }

        bool needUpdate = false;
        for (const auto& cat : m_CategorieInfos) {
            // ImGui::PushStyleColor(ImGuiCol_Text, cat.second.color);
            needUpdate |= ImGui::RadioButtonLabeled_BitWize<MessageExistFlags>(
                0.0f, cat.second.name.c_str(), nullptr, &m_MessageExistFlags, cat.second.flag);
            // ImGui::PopStyleColor();
        }
        if (needUpdate) {
            m_UpdateFilteredMessages();
        }

        ImGui::EndMenuBar();
    }

    if (m_ShowTextPane) {
        ImGui::Separator();
        ImGui::PushID(ImGui::IncPUSHID());
        if (ImGui::ContrastedButton("X")) {
            m_ShowTextPane = false;
        }
        ImGui::SameLine();
        ImGui::TextWrapped("%s", m_MessageText);
        ImGui::PopID();
        ImGui::Separator();
    }

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Hideable |
                                   ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoHostExtendY;
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
                m_SortFields((SortingFieldEnum)(column + 1), true);
            }
        }

        if (!m_FilteredMessages.empty()) {
            bool _clicked = false;
            int _indexClicked = -1;

            static ImGuiListClipper s_messagesListClipper;
            s_messagesListClipper.Begin((int)m_FilteredMessages.size(), ImGui::GetTextLineHeightWithSpacing());
            while (s_messagesListClipper.Step()) {
                for (int i = s_messagesListClipper.DisplayStart; i < s_messagesListClipper.DisplayEnd; i++) {
                    if (i < 0) {
                        continue;
                    }

                    const auto& msg_ptr = m_FilteredMessages[i].lock();
                    if (msg_ptr != nullptr) {
                        auto ci_ptr = m_GetCategoryInfos(msg_ptr->type);
                        if (ci_ptr != nullptr) {
                            if (m_MessageExistFlags & ci_ptr->flag) {
                                if (ImGui::TableNextColumn()) {  // id
                                    ImGui::Text("%i", i);
                                }
                                if (ImGui::TableNextColumn()) {  // type
                                    ImGui::PushID(ImGui::IncPUSHID());
                                    ImGui::PushStyleColor(ImGuiCol_Text, ci_ptr->color);
                                    if (ImGui::Selectable(
                                            ci_ptr->name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns)) {
                                        _clicked = true;
                                        _indexClicked = i;
                                    }
                                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                                        m_ShowTextPane = true;
                                        _clicked = true;
                                        _indexClicked = i;
                                    }
                                    ImGui::PopStyleColor();
                                    ImGui::PopID();
                                }
                                if (ImGui::TableNextColumn()) {  // str
                                    ImGui::Text("%s", msg_ptr->desc.c_str());
                                }
                            }
                        }
                    }
                }
            }
            s_messagesListClipper.End();

            if (_clicked && _indexClicked > -1) {
                const auto& msg_ptr = m_FilteredMessages[_indexClicked].lock();
                if (msg_ptr != nullptr) {
                    if (m_ShowTextPane) {
                        size_t len = msg_ptr->desc.size();
                        if (len > 4096U) {
                            len = 4096;
                        }
                        m_MessageText[len] = '\0';
#ifdef _MSC_VER
                        strncpy_s(m_MessageText, msg_ptr->desc.c_str(), len);
#else
                        strncpy(m_MessageText, msg_ptr->desc.c_str(), len);
#endif
                    }

                    if (msg_ptr->func) {
                        msg_ptr->func(msg_ptr->data);
                    }
                }
            }
        }

        ImGui::EndTable();
    }
}

void Messaging::AddMessage(const std::string& vMsg,
    const MessageType& vMessageType,
    bool vSelect,
    const MessageData& vDatas,
    const MessageFunc& vFunction) {
    if (vSelect) {
        m_CurrentMsgIdx = (int32_t)m_Messages.size();
    }

    if (m_CategorieInfos.find(vMessageType) != m_CategorieInfos.end()) {
        ++m_CategorieInfos.at(vMessageType).count;
    }

    auto msg_ptr = std::make_shared<MessageBlock>();
    msg_ptr->desc = vMsg;
    msg_ptr->type = vMessageType;
    msg_ptr->data = vDatas;
    msg_ptr->func = vFunction;
    m_Messages.push_back(msg_ptr);
}

void Messaging::AddMessage(const MessageType& vMessageType,
    const bool& vSelect,
    const MessageData& vDatas,
    const MessageFunc& vFunction,
    const char* fmt,
    ...) {
    if (m_CategorieInfos.find(vMessageType) != m_CategorieInfos.end()) {
        va_list args;
        va_start(args, fmt);
        m_AddMessage(vMessageType, vSelect, vDatas, vFunction, fmt, args);
        va_end(args);
        m_MessageExistFlags = (MessageExistFlags)(m_MessageExistFlags | m_CategorieInfos.at(vMessageType).flag);
        m_SortFields(m_SortingField);
    }
}

void Messaging::Clear() {
    m_Messages.clear();
    m_FilteredMessages.clear();
}

void Messaging::ClearMessagesOfType(const MessageType& vMessageType) {
    std::forward_list<int> msgToErase;
    auto idx = 0;
    for (auto& msg_ptr : m_Messages) {
        if (msg_ptr->type == vMessageType) {
            msgToErase.push_front(idx);
        }
        ++idx;
    }

    for (auto& id : msgToErase) {
        m_Messages.erase(m_Messages.begin() + id);
    }

    if (m_CategorieInfos.find(vMessageType) != m_CategorieInfos.end()) {
        m_MessageExistFlags &= ~m_CategorieInfos.at(vMessageType).flag;
        m_CategorieInfos.at(vMessageType).count = 0U;
    }

    m_UpdateFilteredMessages();
}

///////////////////////////////////////////////////////////////////////////////////////////
///// PRIVATE /////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

static char Messaging_Message_Buffer[2048] = "\0";

void Messaging::AddCategory(const MessageType& vMessageType,
    const CategoryName& vCategoryName,
    const IconLabel& vIconLabel,
    const ImVec4& vColor) {
    auto& ci = m_CategorieInfos[vMessageType];
    ci.type = vMessageType;
    ci.name = vIconLabel + " " + vCategoryName;
    ci.icon = vIconLabel;
    ci.color = vColor;
    ci.flag = (1 << m_FlagsCount++);
}

bool Messaging::m_DrawMessage(const size_t& vMsgIdx) {
    auto res = false;

    if (vMsgIdx < m_Messages.size()) {
        res |= m_DrawMessage(m_Messages[vMsgIdx], vMsgIdx);
    }

    return res;
}

bool Messaging::m_DrawMessage(const MessageBlockWeak& vMsg, const size_t& vMsgIdx) {
    auto ptr = vMsg.lock();
    if (ptr != nullptr) {
        auto ci_ptr = m_GetCategoryInfos(ptr->type);
        if (ci_ptr != nullptr) {
            ImGui::PushStyleColor(ImGuiCol_Text, ci_ptr->color);
            ImGui::Text("%s ", ci_ptr->icon.c_str());
            ImGui::PopStyleColor();
        }

        // display only selected messages
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (!(window->Flags & ImGuiWindowFlags_MenuBar)) {
            ImGui::SameLine();
            ImGui::PushID(&vMsg);
            const auto check = ImGui::Selectable_FramedText("%s##Messaging", ptr->desc.c_str());
            ImGui::PopID();
            if (check) {
                LayoutManager::Instance()->ShowAndFocusSpecificPane(sMessagePaneId);
                if (ptr->func) {
                    ptr->func(ptr->data);
                }
            }

            return check;
        }
    }

    return false;
}

void Messaging::m_SortFields(SortingFieldEnum vSortingField, bool vCanChangeOrder) {
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

    m_UpdateFilteredMessages();
}

void Messaging::m_UpdateFilteredMessages() {
    m_FilteredMessages.clear();
    if (!m_Messages.empty()) {
        for (auto msg_ptr : m_Messages) {
            m_AddToFilteredMessages(msg_ptr);
        }
    }
}

void Messaging::m_AddToFilteredMessages(const MessageBlockPtr& vMessageBlockPtr) {
    if (vMessageBlockPtr != nullptr) {
        auto ci_ptr = m_GetCategoryInfos(vMessageBlockPtr->type);
        if (ci_ptr != nullptr) {
            if (m_MessageExistFlags & ci_ptr->flag) {
                m_FilteredMessages.push_back(vMessageBlockPtr);
            }
        }
    }
}

const CategoryInfos* Messaging::m_GetCategoryInfos(const MessageType& vMessageType) {
    if (m_CategorieInfos.find(vMessageType) != m_CategorieInfos.end()) {
        return &m_CategorieInfos.at(vMessageType);
    }
    return nullptr;
}

void Messaging::m_AddMessage(const MessageType& vMessageType,
    bool vSelect,
    const MessageData& vDatas,
    const MessageFunc& vFunction,
    const char* fmt,
    va_list args) {
    const auto size = vsnprintf(Messaging_Message_Buffer, 2047, fmt, args);
    if (size > 0) {
        AddMessage(std::string(Messaging_Message_Buffer, size), vMessageType, vSelect, vDatas, vFunction);
    }
}
