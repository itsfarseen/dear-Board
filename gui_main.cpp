#include "gui_main.hpp"
#include <cmath>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

struct Item {
    std::string text;
    bool is_done = false;
    Item(std::string text) : text(text) {}
};

struct Board {
    std::string title;
    std::vector<Item> items;
    size_t editing_index = SIZE_MAX;
};

std::vector<Board> BOARDS; 

void gui_setup(int argc, char **argv) {
    BOARDS.push_back({
        "Backlog",
        {
            {"Statically link engine to Gerber2PDFGui"}, 
            {"Start working on SCRPG Project"}, 
            {"Finish dear Board"}
        }
    });
    BOARDS.push_back({
        "New board",
        {
            {"Statically link engine to Gerber2PDFGui"}, 
            {"Start working on SCRPG Project"}, 
            {"Finish dear Board"}
        }
    });
    ImGui::GetStyle().WindowMinSize = ImVec2(200, 200);
}

static void render_board(size_t index, Board *board);
static void render_board_context_menu(size_t index, Board *board);
static std::optional<Board> new_board_function();

void gui_loop() {
    size_t index = 0;
    ImGui::SetNextWindowPos({20.0, 20.0});
    for (Board &board: BOARDS) {
        std::stringstream window_id;
        window_id << board.title;
        window_id << "###" << index;
        ImGui::Begin(window_id.str().c_str());
            render_board_context_menu(index, &board);
            render_board(index, &board);
            {
                auto next_pos = ImGui::GetWindowPos();
                next_pos.x += ImGui::GetWindowWidth() + 20;
                ImGui::SetNextWindowPos(next_pos);
            }
        ImGui::End();
        index++;
    }
    if(auto board = new_board_function()) {
        BOARDS.push_back(*board);
    }
}


static void render_board(size_t board_index, Board *board) {
    size_t index = 0;
    for(auto &entry: board->items) {
        ImGui::PushID(index);
        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
        ImGui::BeginGroup();
        if(board->editing_index == index) {
            if(ImGui::InputTextMultiline("##ENTRY", 
                            &entry.text, 
                            ImVec2(0.0, 0.0),
                            ImGuiInputTextFlags_EnterReturnsTrue|
                            ImGuiInputTextFlags_AutoSelectAll)) {
                board->editing_index = SIZE_MAX;
            }
            if(!ImGui::IsItemFocused()) {
                ImGui::SetKeyboardFocusHere();
            }
            if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape), false)) {
                board->editing_index = SIZE_MAX;
            }
        } else {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0, 10.0));
            bool is_done_copy = entry.is_done;
            ImGui::Checkbox("##CHECK_BOX", &is_done_copy);
            ImGui::SameLine();
            if(entry.is_done) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
                ImGui::TextWrapped(entry.text.c_str());
                ImGui::PopStyleColor();
            } else {
                ImGui::TextWrapped(entry.text.c_str());
            }
            ImGui::PopStyleVar();
            ImGui::Separator();
        }
        ImGui::EndGroup();
        ImGui::PopItemWidth();
        if (ImGui::IsItemClicked()) {
            entry.is_done = !entry.is_done;
        }
        if(ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0)) {
            board->editing_index = index;
        }
        ImGui::PopID();
        index ++;
    }
}

static void render_board_context_menu(size_t board_index, Board *board) {
    if(ImGui::BeginPopupContextWindow("", 1, false)) {
        ImGui::Text("Rename board: ");
        if(ImGui::InputText("", &board->title, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape), false)) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetKeyboardFocusHere();
        ImGui::EndPopup();
    }
}

static std::optional<Board> new_board_function() {
    if(!ImGui::IsAnyWindowHovered() &&
        ImGui::IsMouseDoubleClicked(0)) {
        return Board {
            "New Board",
            {}
        };
    }
    return {};
}