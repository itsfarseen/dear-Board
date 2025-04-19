#include "widgets.hpp"
#include <imgui.h>
#include <iostream>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

static const ImColor ITEM_BG        = IM_COL32(30,30,30,255);
static const ImColor ITEM_DONE_BG   = IM_COL32(60,60,60,50);
static const ImColor ITEM_HOVER_BG  = IM_COL32(95 ,141,211,128);
static const ImColor ITEM_FG        = IM_COL32(200,200,200,255);
static const ImColor ITEM_HOVER_FG  = IM_COL32(255,255,255,255);
static const ImColor ITEM_DONE_FG   = IM_COL32(100,100,100,255);

/*
 TODO
 ----
 * Fix non-smooth hover switch due to IsItemHovered() being inactive for a few pixels from border.
 * Improve padding, margins.
 * Background color of lists.
 * Background color of main window.
 */ 

static bool IsMouseInsideRect(ImVec2 start, ImVec2 end);

ItemWidgetAction ItemWidget(int index, Item &item, bool is_focused, bool is_hovered) {

    bool is_item_updated = false;

    ImGui::PushID(index);
    auto drawlist = ImGui::GetWindowDrawList();
    drawlist->ChannelsSplit(2);
    // Split channels to foreground and background

    drawlist->ChannelsSetCurrent(1);
    // Switch to foreground channel

    auto initial_cursor = ImGui::GetCursorScreenPos();
    ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0, 10.0));
        // Increase space between checkbox and text

            if(ImGui::Checkbox("##CHECK_BOX", &item.is_done)) {
                is_item_updated = true;
            }

            ImGui::SameLine();

            ImU32 text_color = ITEM_FG;
            if(item.is_done) text_color = ITEM_DONE_FG;
            if(is_hovered)   text_color = ITEM_HOVER_FG;

            ImGui::PushStyleColor(ImGuiCol_Text, text_color);
                ImGui::TextWrapped("%s", item.text.c_str());
            ImGui::PopStyleColor();

        ImGui::PopStyleVar();
        
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(ImGui::GetWindowContentRegionWidth() - ImGui::GetCursorPosX(), 0));

        ImGui::NewLine();
    ImGui::EndGroup();

    drawlist->ChannelsSetCurrent(0);
    // Switch to background channel

    auto final_cursor = initial_cursor + ImGui::GetItemRectSize();
    auto bg_start = initial_cursor; 
    auto bg_end = final_cursor;

    drawlist->AddRectFilled(bg_start, bg_end, 
                            is_focused?ITEM_BG:
                            is_hovered?ITEM_HOVER_BG:
                            item.is_done?ITEM_DONE_BG:
                                       ITEM_BG);
    // Draw background color

    // auto line_start = ImVec2(0, final_cursor.y-1);
    // auto line_end = ImVec2(final_cursor.x, final_cursor.y-1);
    // drawlist->AddLine(line_start, line_end, IM_COL32_WHITE);

    drawlist->ChannelsMerge();
    ImGui::PopID();

    if(is_item_updated) return ItemWidgetAction::ItemUpdated;
    // ItemUpdated action triggers saving to disk. It has highest priority.

    if(is_focused) {
    // Process keyboard events only if focused

        if(ImGui::IsKeyReleased(ImGuiKey_Space)) {
            item.is_done = !item.is_done;
        }
        if(ImGui::IsKeyReleased(ImGuiKey_Enter)) {
            return ItemWidgetAction::RequestEdit;
        }
    }

    if(IsMouseInsideRect(initial_cursor, final_cursor)) {
        // Process mouse events only if hovered

        if(ImGui::IsMouseDoubleClicked(0)) {
            return ItemWidgetAction::RequestEdit;
        } 
        if(ImGui::IsMouseClicked(0)) {
           return ItemWidgetAction::RequestFocus; 
        }
        if(ImGui::IsMouseClicked(1)) {
            return ItemWidgetAction::RequestContextMenu;
        }
    }

    if(  ImGui::IsItemHovered() && !is_hovered ) return ItemWidgetAction::HoverEnter;
    if( !ImGui::IsItemHovered() &&  is_hovered ) return ItemWidgetAction::HoverExit;
    // is_hovered will be passed in by the parent. 
    // It holds the hovered state in previous frame.

    return ItemWidgetAction::None;
}


static bool IsMouseInsideRect(ImVec2 start, ImVec2 end) {
    auto mouse_pos = ImGui::GetMousePos();
    return mouse_pos.x >= start.x && mouse_pos.y >= start.y &&
           mouse_pos.x <= end.x   && mouse_pos.y <= end.y;
}
