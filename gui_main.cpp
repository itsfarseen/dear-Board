#include "gui_main.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <imgui.h>
#include <imgui_stdlib.h>


struct Board {
    std::string title;
    std::vector<std::string> items;
};

std::vector<Board> BOARDS; 

void gui_setup(int argc, char **argv) {
    BOARDS.push_back({
        "Backlog",
        {
            "Statically link engine to Gerber2PDFGui", 
            "Start working on SCRPG Project", 
            "Finish dear Board"
        }
    });
}


static void render_board(Board *board);

void gui_loop() {
    for (Board &board: BOARDS) {
        render_board(&board);
    }
}


static void render_board(Board *board) {
    ImGui::Begin(board->title.c_str());
    static size_t editing_index = SIZE_MAX;

    size_t index = 0;
    for(auto &entry: board->items) {
        ImGui::PushID(index);
        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
        if(editing_index == index) {
            if(ImGui::InputTextMultiline("##EDIT_ENTRY", 
                            &entry, 
                            ImVec2(0.0, 0.0),
                            ImGuiInputTextFlags_EnterReturnsTrue)) {
                editing_index = SIZE_MAX;
            }
        } else {
            ImGui::TextWrapped(entry.c_str());
        }
        ImGui::PopItemWidth();
        if(ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0)) {
            editing_index = index;
        }
        ImGui::Separator();
        ImGui::PopID();
        index ++;
    }
    ImGui::End();
}
