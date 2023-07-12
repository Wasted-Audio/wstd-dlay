/**
 * Copyright (c) Wasted Audio 2023 - GPL-3.0-or-later
 */

#include "DistrhoUI.hpp"
#include "ResizeHandle.hpp"
#include "veramobd.hpp"
#include "wstdcolors.hpp"


START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

struct EnumParam {
    const char* label;
    float value;
};

class ImGuiPluginUI : public UI
{
    float fcross = 20.0f;
    float ffeedback = 25.0f;
    float fmix = 50.0f;
    bool fsync = 0.0f != 0.0f;
    float ftime = 500.0f;
    float ftimesync = 1.0f;

    int default_item_id = 6;
    int current_item_id = default_item_id;
    int items_len = 13;

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
                fsync = value != 0.0f;
                break;
            case 4:
                ftime = value;
                break;
            case 5:
                ftimesync = value;
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

        auto CrossActive     = ColorBright(Red, intense);
        auto CrossHovered    = ColorBright(RedBr, intense);
        auto TimeActive      = ColorBright(Green, intense);
        auto TimeHovered     = ColorBright(GreenBr, intense);
        auto SyncSw          = ColorBright(WhiteDr, intense);
        auto SyncGr          = ColorBright(Grey, intense);
        auto SyncGrHovered   = ColorBright(GreyBr, intense);
        auto SyncAct         = ColorBright(GreenDr, intense);
        auto SyncActHovered  = ColorBright(Green, intense);
        auto FeedbackActive  = ColorBright(Blue, intense);
        auto FeedbackHovered = ColorBright(BlueBr, intense);
        auto MixActive       = ColorMix(TimeActive, Yellow, intense, fmix);
        auto MixHovered      = ColorMix(TimeHovered, YellowBr, intense, fmix);

        const float hundred = 100 * getScaleFactor();

        auto perc = 1.0f;
        auto ms = 10.0f;

        if (io.KeyShift)
        {
            perc = 0.1f;
            ms = 1.0f;
        }

        EnumParam timesync_list[] = {
            { "×6", 0.16666666666f },
            { "×5", 0.2f },
            { "×4", 0.25f },
            { "×3", 0.33333333333f },
            { "×2", 0.5f },
            { "×1.5", 0.66666666666f },
            { "×1", 1.0f },
            { "÷1.5",1.5f },
            { "÷2", 2.0f },
            { "÷3", 3.0f },
            { "÷4", 4.0f },
            { "÷5", 5.0f },
            { "÷6", 6.0f }
        };

        ImGui::PushFont(titleBarFont);
        if (ImGui::Begin("WSTD DLAY", nullptr, ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoCollapse))
        {

            ImGui::Dummy(ImVec2(0.0f, 8.0f * getScaleFactor()));
            ImGui::PushFont(defaultFont);
            auto ImGuiKnob_Flags = ImGuiKnobFlags_DoubleClickReset + ImGuiKnobFlags_ValueTooltip + ImGuiKnobFlags_NoInput + ImGuiKnobFlags_ValueTooltipHideOnClick;
            auto ImGuiKnob_FlagsDB = ImGuiKnob_Flags + ImGuiKnobFlags_dB;
            auto ImGuiKnob_FlagsLog = ImGuiKnob_Flags + ImGuiKnobFlags_Logarithmic;

            ImGui::BeginGroup();
            {
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)TimeActive);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)TimeHovered);
                if (not fsync)
                {
                    if (ImGuiKnobs::Knob(
                        "Time", &ftime, 50.0f, 5000.0f, ms, "%.0fms",
                        ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_FlagsLog, 21))
                    {
                        if (ImGui::IsItemActivated())
                        {
                            editParameter(4, true);
                            if (ImGui::IsMouseDoubleClicked(0))
                                ftime = 500.0f;
                        }
                        setParameterValue(4, ftime);
                    }
                }

                if (fsync)
                {
                    if (ImGuiKnobs::KnobInt(
                        "Time", &current_item_id, 0, items_len-1, 0.1f, timesync_list[current_item_id].label,
                        ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_Flags, items_len
                    ))
                    {
                        if (ImGui::IsItemActivated())
                        {
                            editParameter(5, true);
                            if (ImGui::IsMouseDoubleClicked(0))
                            {
                                ftimesync = timesync_list[default_item_id].value;
                                current_item_id = default_item_id;
                            }
                        }
                        ftimesync = timesync_list[current_item_id].value;
                        setParameterValue(5, ftimesync);
                    }
                }
                ImGui::SameLine();

                auto syncstring = "Sync";
                ImVec2 textSize = ImGui::CalcTextSize(syncstring);
                auto margin = (30.0f * getScaleFactor() - textSize.x)/ 2.0f;

                ImGui::Dummy(ImVec2(margin, 0.0f) * getScaleFactor()); ImGui::SameLine();
                ImGui::BeginGroup();
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 0.85f));
                    ImGui::Text(syncstring);
                    ImGui::PopStyleColor();

                    ImGui::Dummy(ImVec2(0.0f, 35.0f) * getScaleFactor());

                    // knob
                    ImGui::PushStyleColor(ImGuiCol_Text,            (ImVec4)SyncSw);

                    // inactive colors
                    ImGui::PushStyleColor(ImGuiCol_FrameBg,         (ImVec4)SyncGr);
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,  (ImVec4)SyncGrHovered);

                    // active colors
                    ImGui::PushStyleColor(ImGuiCol_Button,          (ImVec4)SyncAct);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)SyncActHovered);
                    if (ImGui::Toggle("##Sync", &fsync, ImGuiToggleFlags_Animated))
                    {
                        if (ImGui::IsItemActivated())
                        {
                            editParameter(3, true);
                            setParameterValue(3, fsync);
                        }
                    }
                    ImGui::PopStyleColor(5);
                }
                ImGui::EndGroup();
            }
            ImGui::PopStyleColor(2);
            ImGui::EndGroup();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)FeedbackActive);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)FeedbackHovered);
            if (ImGuiKnobs::Knob("Feedback", &ffeedback, 0.0f, 100.0f, perc, "%.1f%%", ImGuiKnobVariant_Space, hundred, ImGuiKnob_Flags))
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
            if (ImGuiKnobs::Knob("Cross", &fcross, 0.0f, 100.0f, perc, "%.1f%%", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_Flags, 11))
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
            if (ImGuiKnobs::Knob("Mix", &fmix, 0.0f, 100.0f, perc, "%.1f%%", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_Flags, 11))
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
                editParameter(4, false);
                editParameter(5, false);
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
