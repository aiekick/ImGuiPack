#pragma once

#ifdef USE_IMGUI_COLOR_TEXT_EDIT

#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || defined(WIN64) || defined(_WIN64) || defined(_MSC_VER)
#if defined(ImGuiPack_EXPORTS)
#define IMGUI_API __declspec(dllexport)
#elif defined(BUILD_IMGUI_PACK_SHARED_LIBS)
#define IMGUI_API __declspec(dllimport)
#else
#define IMGUI_API
#endif
#else
#define IMGUI_API
#endif

#include <3rdparty/imgui_imguicolortextedit/TextEditor.h>

#define FIND_POPUP_TEXT_FIELD_LENGTH 128

class IMGUI_API ImCodeEditor {
public:
    typedef void (*OnFocusedCallback)(int folderViewId);
    typedef void (*OnShowInFolderViewCallback)(const std::string& filePath, int folderViewId);

private:
    OnFocusedCallback onFocusedCallback = nullptr;
    OnShowInFolderViewCallback onShowInFolderViewCallback = nullptr;
    TextEditor::LanguageDefinition m_Type;
    std::map<int32_t, std::string> m_ErrorMarkers;
    ImFont* m_CodeFontPtr = nullptr;
    int m_Id = -1;
    int m_CreatedFromFolderView = -1;
    TextEditor m_Editor;
    bool m_ShowDebugPanel = false;
    std::string m_PanelName;
    std::string m_RelatedFile;
    int m_TabSize = 4;
    float m_LineSpacing = 1.0f;
    int m_UndoIndexInDisk = 0;
    char m_CtrlfTextToFind[FIND_POPUP_TEXT_FIELD_LENGTH] = "";
    bool m_CtrlfCaseSensitive = false;

public:
    ImCodeEditor() = default;
    ~ImCodeEditor() = default;
    bool init();
    void unit();

    void OnImGui();
    void SetSelection(int startLine, int startChar, int endLine, int endChar);
    void SetRelatedFile(const std::string& vFile);
    const std::string& GetRelatedFile();
    void OnFolderViewDeleted(int folderViewId);
    void SetShowDebugPanel(bool value);

    void SetCode(const std::string& vCode, const TextEditor::LanguageDefinition& vType);

    void ClearErrorMarkers();
    void AddErrorMarker(const size_t& vErrorLine, const std::string& vErrorMsg);

private:
    void OnReloadCommand();
    void OnLoadFromCommand();
    void OnSaveCommand();
};

#endif  // USE_IMGUI_COLOR_TEXT_EDIT
