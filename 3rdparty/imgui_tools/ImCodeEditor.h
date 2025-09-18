#pragma once

#ifdef USE_IMGUI_COLOR_TEXT_EDIT

#include <imguipack.h>
#include <3rdparty/imgui_imguicolortextedit/TextEditor.h>
#include <3rdparty/imgui_imguicolortextedit/TextDiff.h>

#include <algorithm>
#include <functional>
#include <string>

#define FIND_POPUP_TEXT_FIELD_LENGTH 128

class IMGUI_API ImCodeEditor {
private:
    ImFont* mp_font{};

    std::string m_originalText;
    TextEditor m_editor;
    TextDiff m_diff;
    std::string m_filename;
    size_t m_version{};
    bool m_done{false};
    std::string m_errorMessage;
    std::function<void()> m_onConfirmClose;

    float m_fontSize{17.0f};

    // editor state
    enum class State {  //
        edit,
        diff,
        newFile,
        confirmClose,
        confirmQuit,
        confirmError
    } m_state = State::edit;

public:
    void setFont(ImFont* vpFont);

    // file releated functions
    void newFile();
    void openFile(const std::string& path);
    void saveFile();

    // manage program exit
    void tryToQuit();
    inline bool isDone() const { return m_done; }

    // render the editor
    void render();

private:
    // private functions
    void renderMenuBar();
    void renderStatusBar();

    void showDiff();
    void showConfirmClose(std::function<void()> callback);
    void showConfirmQuit();
    void showError(const std::string& message);

    void renderDiff();
    void renderConfirmClose();
    void renderConfirmQuit();
    void renderConfirmError();

    bool isDirty() const { return m_editor.GetUndoIndex() != m_version; }
    bool isSavable() const { return isDirty() && m_filename != "untitled"; }

    void increaseFontSIze() { m_fontSize = ImClamp(m_fontSize + 1.0f, 8.0f, 24.0f); }
    void decreaseFontSIze() { m_fontSize = ImClamp(m_fontSize - 1.0f, 8.0f, 24.0f); }
};

#endif  // USE_IMGUI_COLOR_TEXT_EDIT
