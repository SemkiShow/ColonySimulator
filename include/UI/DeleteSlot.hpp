// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Progress.hpp"
#include <RCore/Translations.hpp>
#include <RWidgets/Windows/ROkCancelWindow.hpp>

class DeleteSlotMenu : public ROkCancelWindow
{
  public:
    DeleteSlotMenu();

    void SetSlotIdx(int val)
    {
        slotIdx = val;
        SetMessageText(GetText("Are you sure you want to delete map ") + saveSlots[slotIdx].name +
                       "?");
    }

  private:
    int slotIdx = 0;
};

extern std::shared_ptr<DeleteSlotMenu> deleteSlotMenu;
