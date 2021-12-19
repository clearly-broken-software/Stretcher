
/*
 * Clearly Broken Software
 * Copyright (C) 2021 Rob van den Berg <rghvdberg@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoUI.hpp"
#include "NanoButton.hpp"
#include "NanoSlider.hpp"
#include "DistrhoPluginInfo.h"
#include <memory>

START_NAMESPACE_DISTRHO
// -----------------------------------------------------------------------------------------------------------

class StretcherUI : public UI,
                    public ButtonEventHandler::Callback,
                    public SliderEventHandler::Callback
{
public:
    StretcherUI()
        : UI(400, 200),
          estimatedTempo(0.0f),
          tempo(0.0f)
    {
#ifdef DGL_NO_SHARED_RESOURCES
        createFontFromFile("sans", "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf");
#else
        loadSharedResources();
#endif
        button.reset(new NanoButton(this, this));
        button->setSize(100, 40);
        button->setAbsolutePos(400 / 2 - 100 / 2, 200 / 2 - 40 / 2);
        slider.reset(new NanoSlider(this, this));
        slider->setSize(30, 100);
        slider->setAbsolutePos(75, 50);
        slider->setBackgroundColor(Color(46, 194, 179));
        slider->setHandleColor(Color(255, 159, 26));
        slider->setSliderArea(Rectangle<double>(0, 0, 30, 100));
        slider->setStartPos(Point<int>(0, 100));
        slider->setEndPos(Point<int>(0, 0));
        slider->setInverted(true);
    }

protected:
    void parameterChanged(uint32_t index, float value) override
    {
        switch (index)
        {
        case kTimeRatio:
            // norm  = (value - min) / (max - min)
            {
                const auto normValue = (value - 0.5f) / (2.0f - 0.5f);
                slider->setValue(normValue);
                tempo = estimatedTempo * (1.f / value);
            }

            repaint();
            break;
        case kEstimatedTempo:
            if (value != estimatedTempo)
            {
                estimatedTempo = value;
                tempo = estimatedTempo;
                repaint();
            }
        default:
            break;
        }
    }

    void onNanoDisplay() override
    {
        // ManausSport
        // rgb(1,22,40)
        // rgb(252,255,250)
        // rgb(46,194,179)
        // rgb(231,29,52)
        // rgb(255,159,26)

        const auto w = getWidth();
        const auto h = getHeight();
        beginPath();
        fillColor(1, 22, 40);
        rect(0, 0, w, h);
        fill();
        closePath();

        beginPath();
        fillColor(252, 255, 250);
        char buffer[64];
        sprintf(buffer, "Estimated Tempo = %f", tempo);
        text(20, 20, buffer, nullptr);
        text(50, 170, "Time Ratio", nullptr);
        closePath();
    }

    void buttonClicked(SubWidget *widget, int button) override
    {
        requestStateFile("file");
    }

    void sliderDragStarted(SubWidget *widget) {}
    void sliderDragFinished(SubWidget *widget) {}
    void sliderValueChanged(SubWidget *widget, float value)
    {
        const auto id = widget->getId();
        switch (id)
        {
        case kTimeRatio:
            //       real value = normalized value * (max - min) + min
            {
                const float realVal = value * (2.0f - .5f) + 0.5f;
                setParameterValue(kTimeRatio, realVal);
                tempo = estimatedTempo * (1.0f / realVal);
                repaint();
            }
            break;

        default:
            break;
        }
    }

    void stateChanged(const char *key, const char *value) override
    {
        // d_stdout("UI::stateChanged %s, %s", key, value);
    }

private:
    float estimatedTempo;
    float tempo;
    std::unique_ptr<NanoButton> button;
    std::unique_ptr<NanoSlider> slider;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StretcherUI)
};

UI *createUI()
{
    return new StretcherUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
