/*
Copyright 2022-2023 Stephane Cuillerdier (aka aiekick)

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
#pragma warning(disable : 4251)

#include <imgui.h>
#include <functional>
#include <cstdarg>
#include <string>
#include <utility>  // std::pair
#include <vector>
#include <memory>

class IMGUI_API MessageData {
private:
    std::shared_ptr<void> puDatas;

public:
    MessageData() = default;
    MessageData(std::nullptr_t) {
    }
    template <typename T>
    MessageData(const std::shared_ptr<T>& vDatas) {
        SetUserDatas(vDatas);
    }
    template <typename T>
    void SetUserDatas(const std::shared_ptr<T>& vDatas) {
        puDatas = vDatas;
    }
    template <typename T>
    std::shared_ptr<T> GetUserDatas() {
        return std::static_pointer_cast<T>(puDatas);
    }
};

class ProjectManager;
class IMGUI_API Messaging {
public:
    static int sMessagePaneId;
    typedef std::function<void(MessageData)> MessageFunc;
    enum MessageTypeEnum { MESSAGE_TYPE_INFOS = 0, MESSAGE_TYPE_ERROR, MESSAGE_TYPE_WARNING };
    typedef std::tuple<std::string, MessageTypeEnum, MessageData, MessageFunc> MessageBlock;

private:
    const ImVec4& m_ErrorColor   = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    const ImVec4& m_WarningColor = ImVec4(0.8f, 0.8f, 0.0f, 1.0f);

    enum _MessageExistFlags { MESSAGE_EXIST_NONE = 0, MESSAGE_EXIST_INFOS = (1 << 0), MESSAGE_EXIST_ERROR = (1 << 1), MESSAGE_EXIST_WARNING = (1 << 2) };

    typedef int MessageExistFlags;
    MessageExistFlags puMessageExistFlags = MESSAGE_EXIST_INFOS | MESSAGE_EXIST_ERROR | MESSAGE_EXIST_WARNING;

    enum class SortingFieldEnum { FIELD_NONE = 0, FIELD_ID, FIELD_TYPE, FIELD_MSG } m_SortingField = SortingFieldEnum::FIELD_ID;

    std::string m_HeaderIdString;
    std::string m_HeaderTypeString;
    std::string m_HeaderMessageString;
    bool m_SortingDirection[3] = {true, true, true};  // true => Descending, false => Ascending

    int32_t currentMsgIdx = 0;
    std::vector<MessageBlock> puMessages;
    std::vector<MessageBlock> puFilteredMessages;

public:
    void DrawBar();
    void DrawConsole();
    void AddInfos(const bool& vSelect, const MessageData& vDatas, const MessageFunc& vFunction, const char* fmt, ...);    // select => set currentMsgIdx to this msg idx
    void AddWarning(const bool& vSelect, const MessageData& vDatas, const MessageFunc& vFunction, const char* fmt, ...);  // select => set currentMsgIdx to this msg idx
    void AddError(const bool& vSelect, const MessageData& vDatas, const MessageFunc& vFunction, const char* fmt, ...);    // select => set currentMsgIdx to this msg idx
    void ClearErrors();
    void ClearWarnings();
    void ClearInfos();
    void Clear();

    void AddMessage(const std::string& vMsg, MessageTypeEnum vType, bool vSelect, const MessageData& vDatas, const MessageFunc& vFunction);
    void AddMessage(MessageTypeEnum vType, bool vSelect, const MessageData& vDatas, const MessageFunc& vFunction, const char* fmt, va_list args);

private:
    bool DrawMessage(const size_t& vMsgIdx);
    bool DrawMessage(const MessageBlock& vMsg);
    void SortFields(SortingFieldEnum vSortingField = SortingFieldEnum::FIELD_NONE, bool vCanChangeOrder = false);

    void UpdateFilteredMessages();
    void AddToFilteredMessages(const MessageBlock& vMessageBlock);

public:  // singleton
    static Messaging* Instance(Messaging* vCopy = nullptr, bool vForce = false) {
        static Messaging _instance;
        static Messaging* _instance_copy = nullptr;
        if (vCopy || vForce) {
            _instance_copy = vCopy;
        }
        if (_instance_copy) {
            return _instance_copy;
        }
        return &_instance;
    }

public:
    Messaging();                    // Prevent construction
    Messaging(const Messaging&){};  // Prevent construction by copying
    Messaging& operator=(const Messaging&) {
        return *this;
    };             // Prevent assignment
    ~Messaging();  // Prevent unwanted destruction
};
