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

#include "NanoButton.hpp"

START_NAMESPACE_DISTRHO

NanoButton::NanoButton(Widget *const parent,
                       ButtonEventHandler::Callback *cb)
    : NanoWidget(parent),
      ButtonEventHandler(this)
{
    ButtonEventHandler::setCallback(cb);
    loadSharedResources();
}

void NanoButton::onNanoDisplay()
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
    rect(0, 0, w, h);
    fillColor(46, 194, 179);
    fill();
    textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
    beginPath();
    fontSize(16);
    fillColor(252, 255, 250);
    text(w / 2.0f, h / 2.0f, "Load Sample", nullptr);
    closePath();
}

bool NanoButton::onMouse(const MouseEvent &ev)
{
    return ButtonEventHandler::mouseEvent(ev);
}

bool NanoButton::onMotion(const MotionEvent &ev)
{

    return ButtonEventHandler::motionEvent(ev);
}

END_NAMESPACE_DISTRHO