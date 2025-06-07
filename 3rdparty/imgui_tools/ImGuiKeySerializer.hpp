#pragma once

#include <imguipack.h>

#include <string>
#include <array>
#include <stdexcept>

class ImGuiKeySerializer {
private:
    // clang-format off
    // copied from imgui.cpp
    std::array<std::string, ImGuiKey_NamedKey_COUNT> m_keyNames = {
        "Tab", "LeftArrow", "RightArrow", "UpArrow", "DownArrow", "PageUp", "PageDown",
        "Home", "End", "Insert", "Delete", "Backspace", "Space", "Enter", "Escape",
        "LeftCtrl", "LeftShift", "LeftAlt", "LeftSuper", "RightCtrl", "RightShift", "RightAlt", "RightSuper", "Menu",
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H",
        "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
        "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
        "F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24",
        "Apostrophe", "Comma", "Minus", "Period", "Slash", "Semicolon", "Equal", "LeftBracket",
        "Backslash", "RightBracket", "GraveAccent", "CapsLock", "ScrollLock", "NumLock", "PrintScreen",
        "Pause", "Keypad0", "Keypad1", "Keypad2", "Keypad3", "Keypad4", "Keypad5", "Keypad6",
        "Keypad7", "Keypad8", "Keypad9", "KeypadDecimal", "KeypadDivide", "KeypadMultiply",
        "KeypadSubtract", "KeypadAdd", "KeypadEnter", "KeypadEqual",
        "AppBack", "AppForward",
        "GamepadStart", "GamepadBack",
        "GamepadFaceLeft", "GamepadFaceRight", "GamepadFaceUp", "GamepadFaceDown",
        "GamepadDpadLeft", "GamepadDpadRight", "GamepadDpadUp", "GamepadDpadDown",
        "GamepadL1", "GamepadR1", "GamepadL2", "GamepadR2", "GamepadL3", "GamepadR3",
        "GamepadLStickLeft", "GamepadLStickRight", "GamepadLStickUp", "GamepadLStickDown",
        "GamepadRStickLeft", "GamepadRStickRight", "GamepadRStickUp", "GamepadRStickDown",
        "MouseLeft", "MouseRight", "MouseMiddle", "MouseX1", "MouseX2", "MouseWheelX", "MouseWheelY",
        "ModCtrl", "ModShift", "ModAlt", "ModSuper", // ReservedForModXXX are showing the ModXXX names.
    };
    // clang-format on

public:
    static std::string ImGuiKeyToString(ImGuiKey key) {
        ImGuiKeySerializer instance;
        if (key >= ImGuiKey_NamedKey_BEGIN && key < ImGuiKey_NamedKey_END)
            return instance.m_keyNames[key - ImGuiKey_NamedKey_BEGIN];
        return "Unknown";
    }

    static ImGuiKey StringToImGuiKey(const std::string& str) {
        ImGuiKeySerializer instance;
        for (int i = 0; i < instance.m_keyNames.size(); ++i) {
            if (instance.m_keyNames[i] == str)
                return static_cast<ImGuiKey>(ImGuiKey_NamedKey_BEGIN + i);
        }
        return ImGuiKey_None;
    }
};
