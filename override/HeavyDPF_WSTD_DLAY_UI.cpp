/**
 * Copyright (c) Wasted Audio 2023 - GPL-3.0-or-later
 */

#include "DistrhoUI.hpp"
#include "ResizeHandle.hpp"
#include "veramobd.hpp"
#include "wstdcolors.hpp"


START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

class ImGuiPluginUI : public UI
{
    float fcross = 20.0f;
    float ffeedback = 25.0f;
    float fmix = 50.0f;
    float ftime = 500.0f;

    ResizeHandle fResizeHandle;

    // ----------------------------------------------------------------------------------------------------------------

public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */
    ImGuiPluginUI()
        : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true),
          fResizeHandle(this)
    {
        setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true);

        ImGuiIO& io(ImGui::GetIO());

        ImFontConfig fc;
        fc.FontDataOwnedByAtlas = true;
        fc.OversampleH = 1;
        fc.OversampleV = 1;
        fc.PixelSnapH = true;

        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 16.0f * getScaleFactor(), &fc);
        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 21.0f * getScaleFactor(), &fc);
        io.Fonts->Build();
        io.FontDefault = io.Fonts->Fonts[1];

        fResizeHandle.hide();
    }

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

   /**
      A parameter has changed on the plugin side.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override
    {
        switch (index) {
            case 0:
                fcross = value;
                break;
            case 1:
                ffeedback = value;
                break;
            case 2:
                fmix = value;
                break;
            case 3:
                ftime = value;
                break;

            default: return;
        }

        repaint();
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Widget Callbacks

   /**
      ImGui specific onDisplay function.
    */
    void onImGuiDisplay() override
    {

        const float width = getWidth();
        const float height = getHeight();
        const float margin = 0.0f;

        ImGui::SetNextWindowPos(ImVec2(margin, margin));
        ImGui::SetNextWindowSize(ImVec2(width - 2 * margin, height - 2 * margin));

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);


        style.Colors[ImGuiCol_TitleBgActive] = (ImVec4)ImColor::HSV(3.31f / 3.6f, 0.64f, 0.40f);
        style.Colors[ImGuiCol_WindowBg] = (ImVec4)ImColor::HSV(3.31f / 3.6f, 0.64f, 0.10f);

        ImGuiIO& io(ImGui::GetIO());
        ImFont* defaultFont = ImGui::GetFont();
        ImFont* titleBarFont = io.Fonts->Fonts[2];

        auto intense = (ffeedback - 20.0f) / 5.0f;

        auto CrossActive      = ColorBright(Red, intense);
        auto CrossHovered     = ColorBright(RedBr, intense);
        auto TimeActive       = ColorBright(Green, intense);
        auto TimeHovered      = ColorBright(GreenBr, intense);
        auto FeedbackActive   = ColorBright(Blue, intense);
        auto FeedbackHovered  = ColorBright(BlueBr, intense);
        auto MixActive        = ColorMix(TimeActive, Yellow, intense, fmix);
        auto MixHovered       = ColorMix(TimeHovered, YellowBr, intense, fmix);

        const float hundred = 100 * getScaleFactor();


        ImGui::PushFont(titleBarFont);
        if (ImGui::Begin("WSTD DLAY", nullptr, ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoCollapse))
        {

            ImGui::Dummy(ImVec2(0.0f, 8.0f * getScaleFactor()));
            ImGui::PushFont(defaultFont);
            auto ImGuiKnob_Flags = ImGuiKnobFlags_DoubleClickReset + ImGuiKnobFlags_ValueTooltip + ImGuiKnobFlags_NoInput + ImGuiKnobFlags_ValueTooltipHideOnClick;
            auto ImGuiKnob_FlagsDB = ImGuiKnob_Flags + ImGuiKnobFlags_dB;
            auto ImGuiKnob_FlagsLog = ImGuiKnob_Flags + ImGuiKnobFlags_Logarithmic;

            ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)TimeActive);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)TimeHovered);
            if (ImGuiKnobs::Knob("Time", &ftime, 50.0f, 5000.0f, 10.0f, "%.0fms", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_FlagsLog, 21))
            {
                if (ImGui::IsItemActivated())
                {
                    editParameter(3, true);
                    if (ImGui::IsMouseDoubleClicked(0))
                        ftime = 500.0f;
                }
                setParameterValue(3, ftime);
            }
            ImGui::PopStyleColor(2);
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)FeedbackActive);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)FeedbackHovered);
            if (ImGuiKnobs::Knob("Feedback", &ffeedback, 0.0f, 100.0f, 1.0f, "%.0f%%", ImGuiKnobVariant_Space, hundred, ImGuiKnob_Flags))
            {
                if (ImGui::IsItemActivated())
                {
                    editParameter(1, true);
                    if (ImGui::IsMouseDoubleClicked(0))
                        ffeedback = 25.0f;

                }
                setParameterValue(1, ffeedback);
            }
            ImGui::PopStyleColor(2);
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)CrossActive);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)CrossHovered);
            if (ImGuiKnobs::Knob("Cross", &fcross, 0.0f, 100.0f, 1.0f, "%.0f%%", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_Flags, 11))
            {
                if (ImGui::IsItemActivated())
                {
                    editParameter(0, true);
                    if (ImGui::IsMouseDoubleClicked(0))
                        fcross = 20.0f;

                }
                setParameterValue(0, fcross);
            }
            ImGui::PopStyleColor(2);
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)MixActive);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)MixHovered);
            if (ImGuiKnobs::Knob("Mix", &fmix, 0.0f, 100.0f, 1.0f, "%.0f%%", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_Flags, 11))
            {
                if (ImGui::IsItemActivated())
                {
                    editParameter(2, true);
                    if (ImGui::IsMouseDoubleClicked(0))
                        fmix = 50.0f;

                }
                setParameterValue(2, fmix);
            }
            ImGui::PopStyleColor(2);
            ImGui::SameLine();

            if (ImGui::IsItemDeactivated())
            {
                editParameter(0, false);
                editParameter(1, false);
                editParameter(2, false);
                editParameter(3, false);
            }

            ImGui::PopFont();
        }
        ImGui::PopFont();
        ImGui::End();
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImGuiPluginUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new ImGuiPluginUI();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
