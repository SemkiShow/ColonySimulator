// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#include "UI/DeleteSlotMenu.hpp"
#include "Languages.hpp"

std::shared_ptr<DeleteSlotMenu> deleteSlotMenu;

DeleteSlotMenu::DeleteSlotMenu() : ROkCancelWindow("", _("Yes"), _("No"))
{
    Connect([this] { return IsOkClicked(); }, [this] { saveSlots[slotIdx].deleteLater = true; });
}
