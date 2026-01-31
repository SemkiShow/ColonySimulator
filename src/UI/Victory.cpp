// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/Victory.hpp"
#include "Languages.hpp"
#include "Progress.hpp"
#include "Settings.hpp"
#include "Sound.hpp"
#include <RCore/Translations.hpp>
#include <RWidgets/RLabelButton.hpp>
#include <RWidgets/RVBoxLayout.hpp>
#include <cassert>
#include <raylib.h>

std::shared_ptr<VictoryMenu> victoryMenu;

VictoryMenu::VictoryMenu()
{
    auto layout = std::make_shared<RVBoxLayout>();
    SetCentralWidget(layout);

    label = std::make_shared<RLabel>();
    label->SetAlignment(RAlign::HCenter);
    layout->AddWidget(label);

    auto okButton = std::make_shared<RLabelButton>(_("Ok"));
    okButton->SetAlignment(RAlign::HCenter);
    okButton->SetAlignment(RAlign::Bottom);
    layout->AddWidget(okButton);

    Connect([okButton] { return okButton->IsClicked(); }, [this] { SetVisible(false); });
}

void VictoryMenu::Show()
{
    assert(IsSlotValid(currentSlot));
    SetVisible(true);
    auto time = saveSlots[currentSlot].time;
    if (bestTime < 0) bestTime = time;
    label->SetLabel(GetText("You won!") + '\n' + GetText("Time elapsed: ") + FormatTime(time) +
                    '\n' + GetText("Best time: ") + FormatTime(bestTime));
    if (time < bestTime)
        label->SetTint({"#009800ff"});
    else
        label->SetThemeList(RThemeList::Text);
    bestTime = std::min(bestTime, time);
    PlaySound(victorySound);
}
