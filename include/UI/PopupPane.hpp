// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <RWidgets/Buttons/RIconButton.hpp>
#include <RWidgets/Panes/RPaneRounded.hpp>
#include <RWidgets/Windows/RWindow.hpp>

class PopupPane : public RWindow
{
  public:
    PopupPane() : closeButton(RIcon::Cross) {}
    virtual ~PopupPane() = default;

    void ResetEvents() override;
    bool PollEvents() override;
    void Update() override;
    void Draw() override;

    bool IsCloseButtonClicked() { return closeButton.IsClicked(); }

    void SetPaneMargin(float val) { paneMargin = val; }

    float GetPaneMargin() { return paneMargin; }

  protected:
    float paneMargin = 10;

  private:
    RIconButton closeButton;
    RPaneRounded pane;
};
