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

#include "NanoSlider.hpp"
#include "Window.hpp"

START_NAMESPACE_DGL

NanoSlider::NanoSlider(Widget *const parent, SliderEventHandler::Callback *const cb)
    : NanoWidget(parent),
      SliderEventHandler(this),
      backgroundColor(32, 32, 32),
      labelColor(255, 255, 255),
      label("Slider"),
      fontScale(1.0f)
{
#ifdef DGL_NO_SHARED_RESOURCES
    createFontFromFile("sans", "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf");
#else
    loadSharedResources();
#endif
    SliderEventHandler::setCallback(cb);
}

void NanoSlider::setBackgroundColor(const Color color)
{
    backgroundColor = color;
}

void NanoSlider::setHandleColor(const Color color)
{
    handleColor = color;
}

void NanoSlider::setFontScale(const float scale)
{
    fontScale = scale;
}

void NanoSlider::setLabel(const std::string &label2)
{
    label = label2;
}

void NanoSlider::setLabelColor(const Color color)
{
    labelColor = color;
}

void NanoSlider::onNanoDisplay()
{
    const uint w = getWidth();
    const uint h = getHeight();
    const float val = getValue();
    beginPath();
    fillColor(backgroundColor);
    rect(0, 0, w, h);
    fill();
    closePath();
    beginPath();
    fillColor(handleColor);
    if (isInverted())
    {
        rect(0, h - h * val, w, h);
    }
    else
    {
        rect(0, 0, w, h * val);
    }
    fill();
    closePath();
}

bool NanoSlider::onMouse(const MouseEvent &ev)
{
    return SliderEventHandler::mouseEvent(ev);
}

bool NanoSlider::onMotion(const MotionEvent &ev)
{
    return SliderEventHandler::motionEvent(ev);
}

END_NAMESPACE_DGL
