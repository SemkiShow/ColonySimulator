// SPDX-FileCopyrightText: 2026 SemkiShow
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Drawing.hpp"
#include <RWidgets/RLabel.hpp>
#include <RWidgets/RProgressBar.hpp>
#include <RWidgets/RWindow.hpp>
#include <atomic>
#include <string>
#include <thread>

class LoadingScreen : public RWindow
{
  public:
    LoadingScreen();

    void SetShowProgressBar(bool val) { progressBar->SetVisible(val); }

    void SetLabel(const std::string& val)
    {
        label->SetLabel(val);
        UpdateBounds();
    }

    void SetPercent(float val) { progressBar->SetValue(val); }

  private:
    std::shared_ptr<RLabel> label;
    std::shared_ptr<RProgressBar> progressBar;
};

extern std::shared_ptr<LoadingScreen> loadingScreen;

template <typename Func, typename... Args>
void ShowLoadingScreen(bool showProgressbar, Func&& f, Args&&... args)
{
    static_assert(
        std::is_invocable_v<Func, std::string&, float&, std::atomic<bool>&, Args...>,
        "Function must accept (std::string& label, float& loadingPercent, std::atomic<bool>& finished, ...) as its arguments.");

    std::string label = "Loading...";
    float loadingPercent = 0;

    std::atomic<bool> finished(false);
    std::thread thread(std::forward<Func>(f), std::ref(label), std::ref(loadingPercent),
                       std::ref(finished), std::forward<Args>(args)...);
    thread.detach();

    loadingScreen->SetShowProgressBar(showProgressbar);
    loadingScreen->SetVisible(true);

    while (!finished)
    {
        loadingScreen->SetLabel(label);
        loadingScreen->SetPercent(loadingPercent);

        DrawFrame();
    }

    loadingScreen->SetVisible(false);
}
