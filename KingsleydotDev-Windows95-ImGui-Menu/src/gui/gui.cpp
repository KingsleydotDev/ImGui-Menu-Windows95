#include "gui.hpp"
#include "framework/framework.hpp"
#include "pages/pages.hpp"
#include "imgui.h"

void gui::Render() noexcept
{
    if (ImGui::IsKeyPressed(ImGuiKey_Insert, false))
    {
        open = !open;
    }

    if (!open)
    {
        return;
    }

    static bool showToolbar = false;
    static bool showStatusBar = true;

    ksd::SetToolbarVisible(showToolbar);
    ksd::BeginMain("Kingsley's Win95 Menu");

    if (ksd::BeginMenuBar())
    {
        if (ksd::BeginMenu("&File"))
        {
            ksd::MenuItem("&New", "Ctrl+N");
            ksd::MenuItem("&Open...", "Ctrl+O");
            ksd::MenuItem("&Save", "Ctrl+S");
            ksd::MenuSeparator();
            if (ksd::MenuItem("E&xit"))
            {
                open = false;
            }
            ksd::EndMenu();
        }
        if (ksd::BeginMenu("&Edit"))
        {
            ksd::MenuItem("&Undo", "Ctrl+Z", false, false);
            ksd::MenuSeparator();
            ksd::MenuItem("Cu&t", "Ctrl+X");
            ksd::MenuItem("&Copy", "Ctrl+C");
            ksd::MenuItem("&Paste", "Ctrl+V");
            ksd::EndMenu();
        }
        if (ksd::BeginMenu("&View"))
        {
            if (ksd::MenuItem("&Toolbar", nullptr, showToolbar))
            {
                showToolbar = !showToolbar;
            }
            if (ksd::MenuItem("&Status Bar", nullptr, showStatusBar))
            {
                showStatusBar = !showStatusBar;
            }
            ksd::EndMenu();
        }
        if (ksd::BeginMenu("&Help"))
        {
            if (ksd::MenuItem("&About..."))
            {
                static const ksd::MessageLink aboutLinks[] = {
                    { "YouTube", "https://www.youtube.com/watch?v=MMVKjO2yi94" },
                    { "Discord", "https://discord.gg/7JfVtn2cwE" }
                };
                ksd::OpenMessageBox("about", "About", "Made by KingsleydotDev", aboutLinks, 2);
            }
            ksd::EndMenu();
        }
        ksd::EndMenuBar();
    }

    if (showToolbar)
    {
        ksd::BeginToolbar();
        ksd::ToolButton("tb_new", 0, "New");
        ksd::ToolButton("tb_open", 1, "Open");
        ksd::ToolButton("tb_save", 2, "Save");
        ksd::ToolSeparator();
        ksd::ToolButton("tb_cut", 3, "Cut");
        ksd::ToolButton("tb_copy", 4, "Copy");
        ksd::ToolButton("tb_paste", 5, "Paste");
        ksd::EndToolbar();
    }

    int& tab = ksd::CurrentTab();
    ksd::TabButton("aimbot", 0);
    ksd::TabButton("visuals", 1);
    ksd::TabButton("world", 2);
    ksd::TabButton("skins", 3);
    ksd::TabButton("players", 4);
    ksd::TabButton("misc", 5);
    ksd::TabButton("config", 6);

    pages::RenderTab(tab);

    if (ksd::PopUpsAreClosed() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
        ImGui::IsMouseHoveringRect(ImVec2(ksd::BodyLeft(), ksd::BodyTop()), ImVec2(ksd::BodyRight(), ksd::BodyBottom())))
    {
        const ImVec2 m = ImGui::GetMousePos();
        ksd::OpenPopupMenu("body_ctx", m.x, m.y);
    }
    if (ksd::BeginPopupMenu("body_ctx"))
    {
        ksd::MenuItem("Cu&t", "Ctrl+X");
        ksd::MenuItem("&Copy", "Ctrl+C");
        ksd::MenuItem("&Paste", "Ctrl+V");
        ksd::MenuSeparator();
        ksd::MenuItem("P&roperties");
        ksd::EndPopupMenu();
    }

    ksd::DrawMessageBox();

    ksd::EndMain();

    if (ksd::CloseRequested())
    {
        open = false;
    }
}
