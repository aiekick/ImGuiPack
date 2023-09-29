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

#include <string>
#include <vector>
#include <memory>
#include <imgui.h>
#include <cstdarg>
#include <utility>  // std::pair
#include <functional>
#include <unordered_map>

typedef uint32_t MessageType;
typedef std::string MessageDesc;
typedef int32_t MessageExistFlags;
typedef std::string IconLabel;
typedef std::string CategoryName;
typedef int32_t CategoryFlag;
typedef const char* CategoryID;

class IMGUI_API MessageData {
private:
    std::shared_ptr<void> m_Datas;

public:
    MessageData() = default;
    MessageData(std::nullptr_t) {}
    template <typename T>
    MessageData(const std::shared_ptr<T>& vDatas) {
        SetUserDatas(vDatas);
    }
    template <typename T>
    void SetUserDatas(const std::shared_ptr<T>& vDatas) {
        m_Datas = vDatas;
    }
    template <typename T>
    std::shared_ptr<T> GetUserDatas() {
        return std::static_pointer_cast<T>(m_Datas);
    }
};
typedef std::function<void(MessageData)> MessageFunc;

class IMGUI_API MessageBlock {
public:
    MessageDesc desc;
    MessageType type=0;
    MessageData data;
    MessageFunc func;
};
typedef std::shared_ptr<MessageBlock> MessageBlockPtr;
typedef std::weak_ptr<MessageBlock> MessageBlockWeak;

class IMGUI_API CategoryInfos {
public:
    MessageType type = 0;
    CategoryName name;
    IconLabel icon;
    ImVec4 color;
    CategoryFlag flag = 0;
    size_t count = 0U;
};

class IMGUI_API Messaging {
public:
    static int sMessagePaneId;
    enum class SortingFieldEnum { FIELD_NONE = 0, FIELD_ID, FIELD_TYPE, FIELD_MSG };

private:
    std::unordered_map<MessageType, CategoryInfos> m_CategorieInfos;
    int32_t m_FlagsCount = 0;

    SortingFieldEnum m_SortingField = SortingFieldEnum::FIELD_ID;

    std::string m_HeaderIdString;
    std::string m_HeaderTypeString;
    std::string m_HeaderMessageString;
    bool m_SortingDirection[3] = {true, true, true};  // true => Descending, false => Ascending

    MessageExistFlags m_MessageExistFlags = 0;

    int32_t m_CurrentMsgIdx = 0;
    std::vector<MessageBlockPtr> m_Messages;
    std::vector<MessageBlockWeak> m_FilteredMessages;

    bool m_ShowTextPane = false;
    char m_MessageText[4096 + 1] = "\0";

public:
    void ClearMessagesOfType(const MessageType& vMessageType);
    void Clear();

    void DrawStatusBar();
    void DrawConsolePane();

    void AddCategory(const MessageType& vMessageType,
        const CategoryName& vCategoryName,
        const IconLabel& vIconLabel,
        const ImVec4& vColor);
    void AddMessage(
        const MessageType& vMessageType,
        const bool& vSelect,
        const MessageData& vDatas,
        const MessageFunc& vFunction,
        const char* fmt,
        ...);
    void AddMessage(const std::string& vMsg,
        const MessageType& vMessageType,
        bool vSelect,
        const MessageData& vDatas,
        const MessageFunc& vFunction);
    
private:
    bool m_DrawMessage(const size_t& vMsgIdx);
    bool m_DrawMessage(const MessageBlockWeak& vMsg, const size_t& vMsgIdx);
    void m_SortFields(SortingFieldEnum vSortingField = SortingFieldEnum::FIELD_NONE, bool vCanChangeOrder = false);

    void m_UpdateFilteredMessages();
    void m_AddToFilteredMessages(const MessageBlockPtr& vMessageBlockPtr);
    const CategoryInfos* m_GetCategoryInfos(const MessageType& vMessageType);

    void m_AddMessage(const MessageType& vMessageType,
        bool vSelect,
        const MessageData& vDatas,
        const MessageFunc& vFunction,
        const char* fmt,
        va_list args);

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
    Messaging();                                               // Prevent construction
    Messaging(const Messaging&){};                             // Prevent construction by copying
    Messaging& operator=(const Messaging&) { return *this; };  // Prevent assignment
    ~Messaging();                                              // Prevent unwanted destruction
};
