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

#ifndef NANO_Slider_HPP_INCLUDED
#define NANO_Slider_HPP_INCLUDED

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"

#include <string>

START_NAMESPACE_DGL

class NanoSlider : public NanoSubWidget,
               public SliderEventHandler
{
public:
    explicit NanoSlider(Widget* parent, SliderEventHandler::Callback* cb);

    void setBackgroundColor(Color color);
    void setFontScale(float scale);
    void setLabel(const std::string& label);
    void setLabelColor(Color color);
    void setHandleColor(Color color);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

private:
    Color backgroundColor;
    Color handleColor;
    Color labelColor;
    std::string label;
    float fontScale;

    DISTRHO_LEAK_DETECTOR(NanoSlider)
};

END_NAMESPACE_DGL

#endif // NANO_Slider_HPP_INCLUDED
