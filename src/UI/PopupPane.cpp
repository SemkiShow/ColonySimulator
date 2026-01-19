// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/PopupPane.hpp"
#include <RCore/Api.hpp>

void PopupPane::ResetEvents()
{
    RWindow::ResetEvents();
    closeButton.ResetEvents();
}

bool PopupPane::PollEvents()
{
    if (closeButton.PollEvents()) return true;
    if (PollCentralWidgetEvents()) return true;
    return RWidget::PollEvents();
}

void PopupPane::Update()
{
    if (updateBounds)
    {
        updateBounds = false;

        auto windowSize = rui::GetWindowSize();
        SetSize(GetMaxSize());
        if (maxSize.x < 0) SetWidth(windowSize.x);
        if (maxSize.y < 0) SetHeight(windowSize.y);
        bounds = ClampBounds(bounds, minSize, maxSize);

        RVector2 closeButtonPos = GetPosition();
        closeButtonPos.x = bounds.width - margin - closeButton.GetMaxWidth();
        closeButtonPos.y += margin;

        closeButton.SetPosition(closeButtonPos);
        closeButton.UpdateBounds();
        if (closeButton.GetWidth() < 0) closeButton.Update();

        RRectangle paneBounds = AddMargin(bounds, paneMargin);
        pane.SetBounds(paneBounds);
        pane.UpdateBounds();

        if (centralWidget)
        {
            auto centralWidgetBounds = AddMargin(paneBounds, margin);
            centralWidgetBounds.width -= closeButton.GetWidth();
            centralWidget->SetBounds(centralWidgetBounds);
            centralWidget->UpdateBounds();
        }
    }
    if (centralWidget && centralWidget->IsVisible()) centralWidget->Update();

    closeButton.Update();
    pane.Update();

    for (auto it = events.begin(); it != events.end();)
    {
        if (!it->IsValid())
        {
            it = events.erase(it);
            continue;
        }

        if (it->event())
        {
            it->func();
        }
        ++it;
    }

    if (closeButton.IsClicked()) SetVisible(false);
}

void PopupPane::Draw()
{
    pane.Draw();
    RWindow::Draw();
    closeButton.Draw();
}
