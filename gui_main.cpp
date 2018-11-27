#include "gui_main.hpp"
#include <imgui.h>
#include <imgui_stdlib.h>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

struct Item {
    std::string text;
    bool is_done = false;
    Item(std::string text) : text(text) {}
};

struct Board {
    std::string title;
    std::vector<Item> items;
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
    ImGui::GetStyle().WindowMinSize = ImVec2(200, 200);
}


static void render_board(size_t index, Board *board);
static void render_board_context_menu(size_t index, Board *board);
static std::optional<Board> new_board_function();

void gui_loop() {
    size_t index = 0;
    for (Board &board: BOARDS) {
        render_board(index, &board);
        render_board_context_menu(index, &board);
        index++;
    }
    if(auto board = new_board_function()) {
        BOARDS.push_back(*board);
    }
}


static void render_board(size_t board_index, Board *board) {
    std::stringstream window_id;
    window_id << board->title;
    window_id << "###" << board_index;
    ImGui::Begin(window_id.str().c_str());
    static size_t editing_index = SIZE_MAX;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0, 10.0));
    size_t index = 0;
    for(auto &entry: board->items) {
        ImGui::PushID(index);
        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
        ImGui::BeginGroup();
        if(editing_index == index) {
            ImGui::SetKeyboardFocusHere();
            if(ImGui::InputTextMultiline("##ENTRY", 
                            &entry.text, 
                            ImVec2(0.0, 0.0),
                            ImGuiInputTextFlags_EnterReturnsTrue|
                            ImGuiInputTextFlags_AutoSelectAll)) {
                editing_index = SIZE_MAX;
            }
            if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape), false)) {
                editing_index = SIZE_MAX;
            }
        } else {
            ImGui::Checkbox("##CHECK_BOX", &entry.is_done);
            ImGui::SameLine();
            if(entry.is_done) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
                ImGui::TextWrapped(entry.text.c_str());
                ImGui::PopStyleColor();
            } else {
                ImGui::TextWrapped(entry.text.c_str());
            }
        }
        ImGui::EndGroup();
        ImGui::PopItemWidth();
        if(ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0)) {
            editing_index = index;
        }
        if (ImGui::IsItemClicked() && !ImGui::IsMouseDoubleClicked(0)) {
            entry.is_done = !entry.is_done;
        }
        ImGui::Separator();
        ImGui::PopID();
        index ++;
    }
    ImGui::PopStyleVar();
    ImGui::End();
}

static void render_board_context_menu(size_t board_index, Board *board) {
    std::stringstream window_id;
    window_id << board->title;
    window_id << "###" << board_index;
    ImGui::Begin(window_id.str().c_str());
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
    ImGui::End();
}

static std::optional<Board> new_board_function() {
    return {};
}