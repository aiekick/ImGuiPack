/*
 * Copyright 2020 Stephane Cuillerdier (aka Aiekick)
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

#pragma once
#pragma warning(disable : 4251)

#include <imguipack.h>

#include <cstdint>  // types like uint32_t
#include <vector>
#include <string>
#include <array>
#include <functional>
#include <ctime>

#define ImWidgets_VERSION "ImWidgets v1.0"

#ifndef IS_FLOAT_DIFFERENT
#define IS_FLOAT_DIFFERENT(a, b) (fabs((a) - (b)) > FLT_EPSILON)
#endif  // IS_FLOAT_DIFFERENT

#ifndef IS_FLOAT_EQUAL
#define IS_FLOAT_EQUAL(a, b) (fabs((a) - (b)) < FLT_EPSILON)
#endif  // IS_FLOAT_EQUAL
struct ImGuiWindow;
struct GLFWWindow;

namespace ImGui {

class IMGUI_API CustomStyle {
public:
    static float puContrastRatio;
    static ImU32 puContrastedTextColor;
    static int pushId;
    static int minorNumber;
    static int majorNumber;
    static int buildNumber;
    static ImVec4 ImGuiCol_Symbol;
    static ImVec4 GoodColor;
    static ImVec4 BadColor;
    static void ResetCustomId();
};

IMGUI_API int IncPUSHID();
IMGUI_API int GetPUSHID();
IMGUI_API void SetPUSHID(int vID);

IMGUI_API ImVec4 GetGoodOrBadColorForUse(bool vUsed);  // return a "good" color if true or "bad" color if false

IMGUI_API ImVec2 GetLocalMousePos(GLFWWindow* vWin = nullptr);  // return local window mouse pos

IMGUI_API void SetContrastRatio(float vRatio);
IMGUI_API void SetContrastedTextColor(ImU32 vColor);
IMGUI_API void DrawContrastWidgets();

IMGUI_API float CalcContrastRatio(const ImU32& backGroundColor, const ImU32& foreGroundColor);
IMGUI_API bool PushStyleColorWithContrast1(const ImGuiCol& backGroundColor, const ImGuiCol& foreGroundColor, const ImU32& invertedColor, const float& minContrastRatio);
IMGUI_API bool PushStyleColorWithContrast2(const ImGuiCol& backGroundColor, const ImGuiCol& foreGroundColor, const ImVec4& invertedColor, const float& maxContrastRatio);
IMGUI_API bool PushStyleColorWithContrast3(const ImU32& backGroundColor, const ImGuiCol& foreGroundColor, const ImVec4& invertedColor, const float& maxContrastRatio);
IMGUI_API bool PushStyleColorWithContrast4(const ImU32& backGroundColor, const ImGuiCol& foreGroundColor, const ImU32& invertedColor, const float& maxContrastRatio);

IMGUI_API void AddInvertedRectFilled(ImDrawList* vDrawList, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags rounding_corners);
IMGUI_API void RenderInnerShadowFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, ImU32 fill_col_darker, ImU32 bg_Color, bool border, float rounding);
IMGUI_API void DrawShadowImage(ImTextureID vShadowImage, const ImVec2& vSize, ImU32 col);

IMGUI_API bool ImageCheckButton(ImTextureID user_texture_id,
                                bool* v,
                                const ImVec2& size,
                                const ImVec2& uv0 = ImVec2(0.0f, 0.0f),
                                const ImVec2& uv1 = ImVec2(1.0f, 1.0f),
                                const ImVec2& vHostTextureSize = ImVec2(0.0f, 0.0f),
                                int frame_padding = -1,
                                float vRectThickNess = 0.0f,
                                ImVec4 vRectColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

IMGUI_API bool BeginFramedGroup(  //
    const char* vLabel,
    ImVec2 vSize = ImVec2(0, 0),
    ImVec4 vCol = ImVec4(0.0f, 0.0f, 0.0f, 0.5f),
    ImVec4 vHoveredCol = ImVec4(0.15f, 0.15f, 0.15f, 0.5f));
IMGUI_API void EndFramedGroup(ImGuiCol vHoveredIdx = ImGuiCol_TabDimmedSelected, ImGuiCol NormalIdx = ImGuiCol_TabDimmed);
IMGUI_API void FramedGroupSeparator();
IMGUI_API void FramedGroupText(ImVec4* vTextColor, const char* vHelp, const char* vFmt, va_list vArgs);
IMGUI_API void FramedGroupText(const char* vFmt, ...);
IMGUI_API void FramedGroupTextHelp(const char* vHelp, const char* vFmt, ...);
IMGUI_API void FramedGroupText(ImVec4 vTextColor, const char* vFmt, ...);

IMGUI_API bool CollapsingHeader_SmallHeight(const char* vName, float vHeightRatio, float vWidth, bool vDefaulExpanded, bool* vIsOpen = nullptr);
IMGUI_API bool CollapsingHeader_CheckBox(const char* vName, float vWidth = -1, bool vDefaulExpanded = false, bool vShowCheckBox = false, bool* vCheckCatched = 0);
IMGUI_API bool CollapsingHeader_Button(const char* vName,
                                       float vWidth = -1,
                                       bool vDefaulExpanded = false,
                                       const char* vLabelButton = 0,
                                       bool vShowButton = false,
                                       bool* vButtonPressed = 0,
                                       ImFont* vButtonFont = nullptr);

IMGUI_API bool CheckBoxIcon(const char* vLabel, const char* vIconTrue, bool* v);
IMGUI_API bool CheckBoxBoolDefault(const char* vName, bool* vVar, bool vDefault, const char* vHelp = 0, ImFont* vLabelFont = nullptr);
IMGUI_API bool CheckBoxInt(const char* vName, int* vVar);
IMGUI_API bool CheckBoxIntDefault(const char* vName, int* vVar, int vDefault, const char* vHelp = 0, ImFont* vLabelFont = nullptr);
IMGUI_API bool CheckBoxFloat(const char* vName, float* vVar);
IMGUI_API bool CheckBoxFloatDefault(const char* vName, float* vVar, float vDefault, const char* vHelp = 0, ImFont* vLabelFont = nullptr);
IMGUI_API bool RadioFloatDefault(const char* vName, float* vVar, int vCount, float* vDefault, const char* vHelp = 0, ImFont* vLabelFont = nullptr);

IMGUI_API bool RadioButtonLabeled(float vWidth, const char* label, bool active, bool disabled);
IMGUI_API bool RadioButtonLabeled(float vWidth, const char* label, const char* help, bool active, bool disabled = false, ImFont* vLabelFont = nullptr);
IMGUI_API bool RadioButtonLabeled(float vWidth, const char* label, const char* help, bool* active, bool disabled = false, ImFont* vLabelFont = nullptr);

template <typename T>
bool RadioButtonLabeled_BitWize(float vWidth,
                                const char* vLabel,
                                const char* vHelp,
                                T* vContainer,
                                T vFlag,
                                bool vOneOrZeroAtTime = false,  // only one selcted at a time
                                bool vAlwaysOne = true,         // radio behavior, always one selected
                                T vFlagsToTakeIntoAccount = (T)0,
                                bool vDisableSelection = false,
                                ImFont* vLabelFont = nullptr)  // radio witl use only theses flags
{
    bool selected = *vContainer & vFlag;
    const bool res = RadioButtonLabeled(vWidth, vLabel, vHelp, &selected, vDisableSelection, vLabelFont);
    if (res) {
        if (selected) {
            if (vOneOrZeroAtTime) {
                if (vFlagsToTakeIntoAccount) {
                    if (vFlag & vFlagsToTakeIntoAccount) {
                        *vContainer = (T)(*vContainer & ~vFlagsToTakeIntoAccount);  // remove these flags
                        *vContainer = (T)(*vContainer | vFlag);                     // add
                    }
                } else
                    *vContainer = vFlag;  // set
            } else {
                if (vFlagsToTakeIntoAccount) {
                    if (vFlag & vFlagsToTakeIntoAccount) {
                        *vContainer = (T)(*vContainer & ~vFlagsToTakeIntoAccount);  // remove these flags
                        *vContainer = (T)(*vContainer | vFlag);                     // add
                    }
                } else
                    *vContainer = (T)(*vContainer | vFlag);  // add
            }
        } else {
            if (vOneOrZeroAtTime) {
                if (!vAlwaysOne)
                    *vContainer = (T)(0);  // remove all
            } else
                *vContainer = (T)(*vContainer & ~vFlag);  // remove one
        }
    }
    return res;
}

template <typename T>
bool RadioButtonLabeled_BitWize(float vWidth,
                                const char* vLabelOK,
                                const char* vLabelNOK,
                                const char* vHelp,
                                T* vContainer,
                                T vFlag,
                                bool vOneOrZeroAtTime = false,  // only one selcted at a time
                                bool vAlwaysOne = true,         // radio behavior, always one selected
                                T vFlagsToTakeIntoAccount = (T)0,
                                bool vDisableSelection = false,
                                ImFont* vLabelFont = nullptr)  // radio witl use only theses flags
{
    bool selected = *vContainer & vFlag;
    const char* label = (selected ? vLabelOK : vLabelNOK);
    const bool res = RadioButtonLabeled(vWidth, label, vHelp, &selected, vDisableSelection, vLabelFont);
    if (res) {
        if (selected) {
            if (vOneOrZeroAtTime) {
                if (vFlagsToTakeIntoAccount) {
                    if (vFlag & vFlagsToTakeIntoAccount) {
                        *vContainer = (T)(*vContainer & ~vFlagsToTakeIntoAccount);  // remove these flags
                        *vContainer = (T)(*vContainer | vFlag);                     // add
                    }
                } else
                    *vContainer = vFlag;  // set
            } else {
                if (vFlagsToTakeIntoAccount) {
                    if (vFlag & vFlagsToTakeIntoAccount) {
                        *vContainer = (T)(*vContainer & ~vFlagsToTakeIntoAccount);  // remove these flags
                        *vContainer = (T)(*vContainer | vFlag);                     // add
                    }
                } else
                    *vContainer = (T)(*vContainer | vFlag);  // add
            }
        } else {
            if (vOneOrZeroAtTime) {
                if (!vAlwaysOne)
                    *vContainer = (T)(0);  // remove all
            } else
                *vContainer = (T)(*vContainer & ~vFlag);  // remove one
        }
    }
    return res;
}

IMGUI_API bool RadioButtonLabeled(ImVec2 vSize, const char* label, bool active, bool disabled);
IMGUI_API bool RadioButtonLabeled(ImVec2 vSize, const char* label, const char* help, bool active, bool disabled = false, ImFont* vLabelFont = nullptr);
IMGUI_API bool RadioButtonLabeled(ImVec2 vSize, const char* label, const char* help, bool* active, bool disabled = false, ImFont* vLabelFont = nullptr);

template <typename T>
bool RadioButtonLabeled_BitWize(ImVec2 vSize,
                                const char* vLabel,
                                const char* vHelp,
                                T* vContainer,
                                T vFlag,
                                bool vOneOrZeroAtTime = false,  // only one selcted at a time
                                bool vAlwaysOne = true,         // radio behavior, always one selected
                                T vFlagsToTakeIntoAccount = (T)0,
                                bool vDisableSelection = false,
                                ImFont* vLabelFont = nullptr)  // radio witl use only theses flags
{
    bool selected = *vContainer & vFlag;
    const bool res = RadioButtonLabeled(vSize, vLabel, vHelp, &selected, vDisableSelection, vLabelFont);
    if (res) {
        if (selected) {
            if (vOneOrZeroAtTime) {
                if (vFlagsToTakeIntoAccount) {
                    if (vFlag & vFlagsToTakeIntoAccount) {
                        *vContainer = (T)(*vContainer & ~vFlagsToTakeIntoAccount);  // remove these flags
                        *vContainer = (T)(*vContainer | vFlag);                     // add
                    }
                } else
                    *vContainer = vFlag;  // set
            } else {
                if (vFlagsToTakeIntoAccount) {
                    if (vFlag & vFlagsToTakeIntoAccount) {
                        *vContainer = (T)(*vContainer & ~vFlagsToTakeIntoAccount);  // remove these flags
                        *vContainer = (T)(*vContainer | vFlag);                     // add
                    }
                } else
                    *vContainer = (T)(*vContainer | vFlag);  // add
            }
        } else {
            if (vOneOrZeroAtTime) {
                if (!vAlwaysOne)
                    *vContainer = (T)(0);  // remove all
            } else
                *vContainer = (T)(*vContainer & ~vFlag);  // remove one
        }
    }
    return res;
}

template <typename T>
bool RadioButtonLabeled_BitWize(ImVec2 vSize,
                                const char* vLabelOK,
                                const char* vLabelNOK,
                                const char* vHelp,
                                T* vContainer,
                                T vFlag,
                                bool vOneOrZeroAtTime = false,  // only one selcted at a time
                                bool vAlwaysOne = true,         // radio behavior, always one selected
                                T vFlagsToTakeIntoAccount = (T)0,
                                bool vDisableSelection = false,
                                ImFont* vLabelFont = nullptr)  // radio witl use only theses flags
{
    bool selected = *vContainer & vFlag;
    const char* label = (selected ? vLabelOK : vLabelNOK);
    const bool res = RadioButtonLabeled(vSize, label, vHelp, &selected, vDisableSelection, vLabelFont);
    if (res) {
        if (selected) {
            if (vOneOrZeroAtTime) {
                if (vFlagsToTakeIntoAccount) {
                    if (vFlag & vFlagsToTakeIntoAccount) {
                        *vContainer = (T)(*vContainer & ~vFlagsToTakeIntoAccount);  // remove these flags
                        *vContainer = (T)(*vContainer | vFlag);                     // add
                    }
                } else
                    *vContainer = vFlag;  // set
            } else {
                if (vFlagsToTakeIntoAccount) {
                    if (vFlag & vFlagsToTakeIntoAccount) {
                        *vContainer = (T)(*vContainer & ~vFlagsToTakeIntoAccount);  // remove these flags
                        *vContainer = (T)(*vContainer | vFlag);                     // add
                    }
                } else
                    *vContainer = (T)(*vContainer | vFlag);  // add
            }
        } else {
            if (vOneOrZeroAtTime) {
                if (!vAlwaysOne)
                    *vContainer = (T)(0);  // remove all
            } else
                *vContainer = (T)(*vContainer & ~vFlag);  // remove one
        }
    }
    return res;
}

template <typename T>
bool MenuItem(const char* label, const char* shortcut, T* vContainer, T vFlag, bool vOnlyOneSameTime = false) {
    bool selected = *vContainer & vFlag;
    const bool res = MenuItem(label, shortcut, &selected, true);
    if (res) {
        if (selected) {
            if (vOnlyOneSameTime) {
                *vContainer = vFlag;  // set
            } else {
                *vContainer = (T)(*vContainer | vFlag);  // add
            }
        } else {
            if (!vOnlyOneSameTime) {
                *vContainer = (T)(*vContainer & ~vFlag);  // remove
            }
        }
    }
    return res;
}

template <typename T>
bool Begin(const char* name, T* vContainer, T vFlag, ImGuiWindowFlags flags, bool* opened = nullptr) {
    bool check = *vContainer & vFlag;
    if (opened) {
        check |= *opened;
    }
    const bool res = Begin(name, &check, flags);
    if (check) {
        *vContainer = (T)(*vContainer | vFlag);  // add
    } else {
        *vContainer = (T)(*vContainer & ~vFlag);  // remove
        if (opened) {
            *opened = false;
        }
    }
    return res;
}

IMGUI_API bool ClickableTextUrl(const char* label, const char* url, bool vOnlined = true);
IMGUI_API bool ClickableTextFile(const char* label, const char* file, bool vOnlined = true);

IMGUI_API bool ColorEdit3Default(float vWidth, const char* vName, float* vCol, float* vDefault);
IMGUI_API bool ColorEdit4Default(float vWidth, const char* vName, float* vCol, float* vDefault);
IMGUI_API void Header(const char* vName, float width = -1);

/*template<typename T>
IMGUI_API bool CheckBoxBitWize(const char* vLabel, const char* vHelp, T* vContainer, T vFlag, bool vDef)
{
    bool check = *vContainer & vFlag;
    bool res = CheckBoxBoolDefault(vLabel, &check, vDef, vHelp);
    if (res)
    {
        if (check)
        {
            // add
            *vContainer = (T)(*vContainer | vFlag);
        }
        else
        {
            // remove
            *vContainer = (T)(*vContainer & ~vFlag);
        }
    }
    return res;
}*/

IMGUI_API void ImageRect(ImTextureID user_texture_id,
                         const ImVec2& pos,
                         const ImVec2& size,
                         const ImVec2& uv0 = ImVec2(0, 0),
                         const ImVec2& uv1 = ImVec2(1, 1),
                         const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
                         const ImVec4& border_col = ImVec4(0, 0, 0, 0));

#ifdef USE_COLOR_EDIT_WIDGETS
IMGUI_API bool ColorEdit3ForNode(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
IMGUI_API bool ColorEdit4ForNode(const char* label, float col[4], ImGuiColorEditFlags flags = 0);
#endif  // USE_COLOR_EDIT_WIDGETS

IMGUI_API void Spacing(float vSpace);
IMGUI_API ImGuiWindow* GetHoveredWindow();

IMGUI_API bool BeginMainStatusBar();
IMGUI_API void EndMainStatusBar();

IMGUI_API bool BeginLeftToolBar(float vWidth);
IMGUI_API void EndLeftToolBar();

IMGUI_API bool ContrastedButton_For_Dialogs(  //
    const char* label,
    const ImVec2& size_arg = ImVec2(0, 0));
IMGUI_API bool ContrastedButton(  //
    const char* label,
    const char* help = nullptr,
    ImFont* imfont = nullptr,
    float vWidth = 0.0f,
    const ImVec2& size_arg = ImVec2(0, 0),
    ImGuiButtonFlags flags = 0);
IMGUI_API bool ToggleContrastedButton(  //
    const char* vLabelTrue,
    const char* vLabelFalse,
    bool* vValue,
    const char* vHelp = nullptr,
    ImFont* vImfont = nullptr);
IMGUI_API bool ButtonNoFrame(  //
    const char* vName,
    ImVec2 size = ImVec2(-1, -1),
    ImVec4 vColor = ImVec4(1, 1, 1, 1),
    const char* vHelp = 0,
    ImFont* vLabelFont = nullptr);
IMGUI_API bool SmallContrastedButton(const char* label, const char* vHelp = nullptr);
IMGUI_API void SpacingFromStart(float offset_from_start_x);
IMGUI_API bool Selectable_FramedText(const char* fmt, ...);

IMGUI_API void ImageZoomPoint(  //
    ImTextureID vUserTextureId,
    const float vWidth,
    const ImVec2& vCenter,
    const ImVec2& vPoint,
    const ImVec2& vRadiusInPixels);
// void ImageZoomLine(ImTextureID vUserTextureId, const float vWidth, const ImVec2& vStart, const ImVec2& vEnd);

IMGUI_API bool InputText_Validation(  //
    const char* label,
    char* buf,
    size_t buf_size,
    const bool* vValidation = nullptr,
    const char* vValidationHelp = nullptr,
    ImGuiInputTextFlags flags = 0,
    ImGuiInputTextCallback callback = nullptr,
    void* user_data = nullptr);

IMGUI_API void HelpMarker(const char* desc);

#ifdef USE_GRADIENT
IMGUI_API void RenderGradFrame(  //
    ImVec2 p_min,
    ImVec2 p_max,
    ImU32 fill_start_col,
    ImU32 fill_end_col,
    bool border,
    float rounding);
IMGUI_API bool GradButton(  //
    const char* label,
    const ImVec2& size_arg = ImVec2(0, 0),
    ImGuiButtonFlags flags = ImGuiButtonFlags_None);
#endif // USE_GRADIENT

IMGUI_API bool TransparentButton(  //
    const char* label,
    const ImVec2& size_arg = ImVec2(0, 0),
    ImGuiButtonFlags flags = 0);

IMGUI_API void PlainImageWithBG(  //
    ImTextureID vTexId,
    const ImVec2& size,
    const ImVec4& bg_col,
    const ImVec4& tint_col);

IMGUI_API void ImageRatio(  //
    ImTextureID vTexId,
    float vRatioX,
    float vWidth,
    ImVec4 vColor,
    float /*vBorderThick*/);

#ifdef USE_OPENGL
// show overlay text on mousehover // l'epaisseur du cadre vient de BorderColor.w
IMGUI_API bool TextureOverLay(  //
    float vWidth,
                              ez::texture* vTex,
                              ImVec4 vBorderColor,
                              const char* vOverlayText,
                              ImVec4 vOverLayTextColor,
                              ImVec4 vOverLayBgColor,
                              ImVec2 vUV0 = ImVec2(0, 0),
                              ImVec2 vUV1 = ImVec2(1, 1));
#endif

////////////////////////////////////////////////////////////
///// SLIDERS
////////////////////////////////////////////////////////////

IMGUI_API bool SliderScalarCompact(  //
    float width,
    const char* label,
    ImGuiDataType data_type,
    void* p_data,
    const void* p_min,
    const void* p_max,
    const void* p_step,
    const char* format = nullptr);
IMGUI_API bool SliderInt32Compact(  //
    float width,
    const char* label,
    int32_t* v,
    int32_t v_min,
    int32_t v_max,
    int32_t v_step,
    const char* format = "%d");
IMGUI_API bool SliderInt64Compact(  //
    float width,
    const char* label,
    int64_t* v,
    int64_t v_min,
    int64_t v_max,
    int64_t v_step,
    const char* format = "%d");
IMGUI_API bool SliderUIntCompact(  //
    float width,
    const char* label,
    uint32_t* v,
    uint32_t v_min,
    uint32_t v_max,
    uint32_t v_step,
    const char* format = "%d");
IMGUI_API bool SliderUInt64Compact(  //
    float width,
    const char* label,
    uint64_t* v,
    uint64_t v_min,
    uint64_t v_max,
    uint64_t v_step,
    const char* format = "%d");
IMGUI_API bool SliderSizeTCompact(  //
    float width,
    const char* label,
    size_t* v,
    size_t v_min,
    size_t v_max,
    size_t v_step,
    const char* format = "%d");
IMGUI_API bool SliderFloatCompact(  //
    float width,
    const char* label,
    float* v,
    float v_min,
    float v_max,
    float v_step,
    const char* format = "%.3f");
IMGUI_API bool SliderDoubleCompact(  //
    float width,
    const char* label,
    double* v,
    double v_min,
    double v_max,
    double v_step,
    const char* format = "%.3f");

IMGUI_API bool SliderScalarDefaultCompact(  //
    float width,
    const char* label,
    ImGuiDataType data_type,
    void* p_data,
    const void* p_min,
    const void* p_max,
    const void* p_default,
    const void* p_step = nullptr,
    const char* format = nullptr);
IMGUI_API bool SliderIntDefaultCompact(  //
    float width,
    const char* label,
    int* v,
    int v_min,
    int v_max,
    int v_default,
    int v_step = 0,
    const char* format = "%d");
IMGUI_API bool SliderUIntDefaultCompact(  //
    float width,
    const char* label,
    uint32_t* v,
    uint32_t v_min,
    uint32_t v_max,
    uint32_t v_default,
    uint32_t v_step = 0,
    const char* format = "%d");
IMGUI_API bool SliderSizeTDefaultCompact(  //
    float width,
    const char* label,
    size_t* v,
    size_t v_min,
    size_t v_max,
    size_t v_default,
    size_t v_step = 0,
    const char* format = "%d");
IMGUI_API bool SliderI64DefaultCompact(  //
    float width,
    const char* label,
    int64_t* v,
    int64_t v_min,
    int64_t v_max,
    int64_t v_default,
    int64_t v_step = 0,
    const char* format = "%d");
IMGUI_API bool SliderFloatDefaultCompact(  //
    float width,
    const char* label,
    float* v,
    float v_min,
    float v_max,
    float v_default,
    float v_step = 0.0f,
    const char* format = "%.3f");
IMGUI_API bool SliderDoubleDefaultCompact(  //
    float width,
    const char* label,
    double* v,
    double v_min,
    double v_max,
    double v_default,
    double v_step = 0.0,
    const char* format = "%.3f");

IMGUI_API bool SliderScalar(  //
    float width,
    const char* label,
    ImGuiDataType data_type,
    void* p_data,
    const void* p_min,
    const void* p_max,
    const void* p_step = nullptr,
    const char* format = nullptr,
    ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderInt(  //
    float width,
    const char* label,
    int* v,
    int v_min,
    int v_max,
    int v_step = 0,
    const char* format = "%d",
    ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderUInt(  //
    float width,
    const char* label,
    uint32_t* v,
    uint32_t v_min,
    uint32_t v_max,
    uint32_t v_step = 0,
    const char* format = "%d",
    ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderSizeT(  //
    float width,
    const char* label,
    size_t* v,
    size_t v_min,
    size_t v_max,
    size_t v_step = 0,
    const char* format = "%zu",
    ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderFloat(  //
    float width,
    const char* label,
    float* v,
    float v_min,
    float v_max,
    float v_step = 0.0f,
    const char* format = "%.3f",
    ImGuiSliderFlags flags = 0);  // adjust format to decorate the value with a prefix or a
                                  // suffix for in-slider labels or unit display.
IMGUI_API bool SliderDouble(      //
    float width,
    const char* label,
    double* v,
    double v_min,
    double v_max,
    double v_step,
    const char* format = "%.3f",
    ImGuiSliderFlags flags = 0);     // adjust format to decorate the value with a prefix or a
                                     // suffix for in-slider labels or unit display.
IMGUI_API bool SliderScalarDefault(  //
    float width,
    const char* label,
    ImGuiDataType data_type,
    void* p_data,
    const void* p_min,
    const void* p_max,
    const void* p_default,
    const void* p_step,
    const char* format = nullptr,
    ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderIntDefault(  //
    float width,
    const char* label,
    int* v,
    int v_min,
    int v_max,
    int v_default,
    int v_step = 0,
    const char* format = "%d",
    ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderUIntDefault(  //
    float width,
    const char* label,
    uint32_t* v,
    uint32_t v_min,
    uint32_t v_max,
    uint32_t v_default,
    uint32_t v_step = 0,
    const char* format = "%d",
    ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderSizeTDefault(  //
    float width,
    const char* label,
    size_t* v,
    size_t v_min,
    size_t v_max,
    size_t v_default,
    size_t v_step = 0,
    const char* format = "%zu",
    ImGuiSliderFlags flags = 0);
IMGUI_API bool SliderFloatDefault(  //
    float width,
    const char* label,
    float* v,
    float v_min,
    float v_max,
    float v_default,
    float v_step = 0.0f,
    const char* format = "%.3f",
    ImGuiSliderFlags flags = 0);     // adjust format to decorate the value with a prefix or
                                     // a suffix for in-slider labels or unit display.
IMGUI_API bool SliderDoubleDefault(  //
    float width,
    const char* label,
    double* v,
    double v_min,
    double v_max,
    double v_default,
    double v_step = 0.0f,
    const char* format = "%.3f",
    ImGuiSliderFlags flags = 0);  // adjust format to decorate the value with a prefix or
                                  // a suffix for in-slider labels or unit display.
template <typename T>
bool SliderDefault(float vWidth, const char* vName, T* vVar, T vInf, T vSup, T vDefault, T vStep = (T)0, bool vForNode = false) {
    (void)(vForNode);

    bool change = false;

    if (std::is_same<T, int>::value) {
        change |= SliderScalarDefault(  //
            vWidth,
            vName,
            ImGuiDataType_S32,
            (void*)vVar,
            (void*)&vInf,
            (void*)&vSup,
            (void*)&vDefault,
            (void*)&vStep);
    } else if (std::is_same<T, float>::value) {
        change |= SliderScalarDefault(  //
            vWidth,
            vName,
            ImGuiDataType_Float,
            (void*)vVar,
            (void*)&vInf,
            (void*)&vSup,
            (void*)&vDefault,
            (void*)&vStep);
    }

    return change;
}

template <typename T>
bool SliderDefaultCompact(float vWidth, const char* vName, T* vVar, T vInf, T vSup, T vDefault, T vStep = (T)0, bool vForNode = false) {
    (void)(vForNode);

    bool change = false;

    if (std::is_same<T, int>::value) {
        change |= SliderScalarDefaultCompact(  //
            vWidth,
            vName,
            ImGuiDataType_S32,
            (void*)vVar,
            (void*)&vInf,
            (void*)&vSup,
            (void*)&vDefault,
            (void*)&vStep);
    } else if (std::is_same<T, float>::value) {
        change |= SliderScalarDefaultCompact(  //
            vWidth,
            vName,
            ImGuiDataType_Float,
            (void*)vVar,
            (void*)&vInf,
            (void*)&vSup,
            (void*)&vDefault,
            (void*)&vStep);
    }

    return change;
}

////////////////////////////////////////////////////////////
///// SPLITTER
////////////////////////////////////////////////////////////

IMGUI_API bool Splitter(  //
    bool split_vertically,
    float thickness,
    float* size1,
    float* size2,
    float min_size1,
    float min_size2,
    float splitter_long_axis_size);

////////////////////////////////////////////////////////////
///// ALIGNEMENTS
////////////////////////////////////////////////////////////

IMGUI_API void DisplayAlignedWidget(  //
    const float& vWidth,
    const std::string& vLabel,
    const float& vOffsetFromStart,
    std::function<void()> vWidget);

////////////////////////////////////////////////////////////
///// HIDDEN TEXT FOR SCREENSHOT
////////////////////////////////////////////////////////////

IMGUI_API void HideByFilledRectForHiddenMode(const bool vHidden, const char* fmt, ...);

// filled recty if vThickNess = 0
IMGUI_API void DrawRectOverText(  //
    const bool vEnabled,
    const ImVec4& vColor,
    const ImVec2& vOffset,
    const float vThickNess,
    const char* fmt,
    ...);

////////////////////////////////////////////////////////////
///// COMBO
////////////////////////////////////////////////////////////

IMGUI_API bool BeginContrastedCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);
IMGUI_API bool BeginContrastedEditCombo(  //
    const char* label,
    char* preview_value,
    size_t preview_value_size,
    ImGuiComboFlags flags = 0);
IMGUI_API bool ContrastedCombo(  //
    float vWidth,
    const char* label,
    int* current_item,
    const char* const items[],
    int items_count,
    int popup_max_height_in_items = -1);
IMGUI_API bool ContrastedCombo(  //
    float vWidth,
    const char* label,
    int* current_item,
    const char* items_separated_by_zeros,
    int popup_max_height_in_items = -1);
IMGUI_API bool ContrastedCombo(  //
    float vWidth,
    const char* label,
    int* current_item,
    bool (*items_getter)(void* data, int idx, const char** out_text),
    void* data,
    int items_count,
    int popup_max_height_in_items = -1);
IMGUI_API bool ContrastedComboVectorDefault(  //
    float vWidth,
    const char* label,
    int* current_item,
    const std::vector<std::string>& items,
    int vDefault,
    int height_in_items = -1);
IMGUI_API bool ContrastedEditCombo(  //
    float vWidth,
    const char* label,
    char* buffer,
    size_t buffer_size,
    int* current_item,
    bool (*items_getter)(void* data, int idx, const char** out_text),
    void* data,
    int items_count,
    int popup_max_height_in_items = -1);

////////////////////////////////////////////////////////////
///// MENU
////////////////////////////////////////////////////////////

bool ContrastedMenuItem(  //
    const char* label,
    const char* help = nullptr,
    bool selected = false,
    bool enabled = true);

bool ContrastedMenuItem(  //
    const char* label,
    const char* help,
    bool* p_selected,
    bool enabled = true);

bool ContrastedBeginMenu(  //
    const char* label,
    const char* help = nullptr,
    bool enabled = true);

////////////////////////////////////////////////////////////
///// INPUT
////////////////////////////////////////////////////////////

IMGUI_API bool InputFloatDefault(  //
    float vWidth,
    const char* vName,
    float* vVar,
    float vDefault,
    const char* vInputPrec = "%.3f",
    const char* vPopupPrec = "%.3f",
    bool vShowResetButton = true,
    float vStep = 0.0f,
    float vStepFast = 0.0f);
IMGUI_API bool InputDoubleDefault(  //
    float vWidth,
    const char* vName,
    double* vVar,
    double vDefault,
    const char* vInputPrec = "%.3f",
    const char* vPopupPrec = "%.3f",
    bool vShowResetButton = true,
    double vStep = 0.0,
    double vStepFast = 0.0);
IMGUI_API bool InputFloatDefaultStepper(  //
    float vWidth,
    const char* vName,
    float* vVar,
    float vDefault,
    float vStep,
    float vStepFast,
    const char* vInputPrec = "%.3f",
    const char* vPopupPrec = "%.3f",
    bool vShowResetButton = true);
IMGUI_API bool InputDoubleDefaultStepper(  //
    float vWidth,
    const float& vInputOffsetFromStart,
    const char* vName,
    double* vVar,
    double vDefault,
    double vStep,
    double vStepFast,
    const char* vInputPrec = "%.3f",
    const char* vPopupPrec = "%.3f",
    bool vShowResetButton = true);
IMGUI_API bool InputIntDefault(  //
    float vWidth,
    const char* vName,
    int* vVar,
    int vDefault,
    int step = 1,
    int step_fast = 1);
IMGUI_API bool InputUIntDefault(  //
    float vWidth,
    const char* vName,
    uint32_t* vVar,
    uint32_t vDefault,
    uint32_t step = 1U,
    uint32_t step_fast = 1U);

}  // namespace ImGui

namespace ImWidgets {
class IMGUI_API InputText {
private:
    static constexpr size_t m_Len = 4096U;
    char m_Buffer[m_Len + 1] = "";
    std::string m_DefaultString;
    std::string m_Text;

public:
    InputText() = default;
    InputText(const std::string& vText) {
        SetText(vText);
    }

    void Clear();

    // vInputOffsetFromStart set the offset from start for the alignement of the inputTexts
    bool DisplayInputText(  //
        const float& vWidth,
                          const std::string& vLabel,
                          const std::string& vDefaultText,
                          const bool vMultiline = false,
                          const float& vInputOffsetFromStart = 0.0f,
                          const bool vNeedChange = false);
    void AddText(const std::string& vText);
    void SetText(const std::string& vText);
    std::string GetText(const std::string& vNumericType = "") const;
    const char* GetConstCharPtrText() const;
    bool empty() const;
};

class IMGUI_API QuickStringCombo {
protected:
    int32_t m_Index = 0;
    std::vector<std::string> m_Array;

public:
    explicit QuickStringCombo() = default;
    explicit QuickStringCombo(const int32_t& vDefaultIndex, const std::vector<std::string>& vArray);
    virtual ~QuickStringCombo() = default;
    virtual void init(const int32_t& vDefaultIndex, const std::vector<std::string>& vArray);
    virtual void clear();
    virtual bool display(const float& vWidth, const char* vLabel);
    virtual bool displayWithColumn(const float vWidth, const char* vLabel, const float vColumnOffset);
    virtual std::string getText() const;
    virtual bool select(const std::string& vToken);
    size_t size() const;
    const std::vector<std::string>& getArray() const;
    std::vector<std::string>& getArrayRef();
    const int32_t& getIndex() const;
    int32_t& getIndexRef();
};

class IMGUI_API QuickStringEditCombo : public QuickStringCombo {
private:
    static constexpr size_t m_Len = 512U;
    char m_Buffer[m_Len + 1] = "";
    std::string m_Text;

public:
    void init(const int32_t& vDefaultIndex, const std::vector<std::string>& vArray) override;
    void clear() override;
    void finalize();
    bool display(const float& vWidth, const char* vLabel) override;
    bool displayWithColumn(const float vWidth, const char* vLabel, const float vColumnOffset) override;
    std::string getText() const override;
    bool select(const std::string& vToken) override;
    void setText(const std::string& vText);
};

#ifdef USE_IMPLOT

class IMGUI_API QuickDateTime {
private:
    std::array<char, 32> m_DateBuffer = {};
    bool m_ShowDatePicker = false;
    int m_DatePickerLevel = 0;  // day level
    ImPlotTime m_DatePicker;

public:
    QuickDateTime();
    bool display(const float vWidth, const char* vLabel);
    bool displayWithColumn(const float vWidth, const char* vLabel, const float vColumnOffset);
    std::string getDateAsString() const;
    time_t getDateAsEpoch() const;

private:
    std::time_t m_convertToEpochTime(const std::string& vIsoDateTime, const char* format) const;
};

#endif  // USE_IMPLOT

}  // namespace ImWidgets
