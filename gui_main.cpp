#include "gui_main.hpp"
#include "models.hpp"
#include <cmath>
#include <fstream>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }


std::vector<Board> BOARDS; 
static void save_to_disk(); 
static void load_from_disk(); 

void gui_setup(int argc, char **argv) {
    ImGui::GetStyle().WindowMinSize = ImVec2(200, 200);
    load_from_disk();
}

static void render_board(size_t index, Board *board);
enum class ContextMenuAction {
        None,
        RenameBoard,
        DeleteBoard,
        DeleteEntry,
        MoveEntry
};
static ContextMenuAction render_board_context_menu(size_t index, Board *board);
static std::optional<Board> new_board_function();

void gui_loop() {
    size_t index = 0;
    ImGui::SetNextWindowPos({20.0, 20.0});
    bool break_the_loop = false;
    for (Board &board: BOARDS) {
        std::stringstream window_id;
        window_id << board.title;
        window_id << "###" << index;
        ImGui::Begin(window_id.str().c_str()); 
        {
            auto action = render_board_context_menu(index, &board);
            render_board(index, &board);
            switch(action) {
                case ContextMenuAction::None:
                    break;
                case ContextMenuAction::DeleteBoard:
                    ImGui::OpenPopup("###DELETE_BOARD");
                    break;
                case ContextMenuAction::RenameBoard:
                    board.title_new = board.title;
                    ImGui::OpenPopup("###RENAME_BOARD");
                    break;
                case ContextMenuAction::DeleteEntry:
                    ImGui::OpenPopup("###DELETE_ENTRY");
                    break;
                case ContextMenuAction::MoveEntry:
                    ImGui::OpenPopup("###MOVE_ENTRY");
                    break;
            }

            if(ImGui::BeginPopup("###DELETE_BOARD")) {
                ImGui::Text("Delete board");
                ImGui::Separator();
                ImGui::Text(board.title.c_str());
                ImGui::Text("This action is irreversible");
                if(ImGui::Button("Delete")) {
                    BOARDS.erase(BOARDS.begin()+index);
                    break_the_loop = true;
                    ImGui::CloseCurrentPopup();
                    save_to_disk();
                }
                ImGui::SameLine();
                if(ImGui::Button("Cancel") ||
                   ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape), false)) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if(ImGui::BeginPopup("###RENAME_BOARD")) {
                ImGui::Text("Rename board");
                ImGui::Separator();
                if(ImGui::InputText("###BOARD_RENAME", &board.title_new, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    board.title = board.title_new;
                    ImGui::CloseCurrentPopup();
                    save_to_disk();
                }
                if(ImGui::Button("Rename")) {
                    board.title = board.title_new;
                    ImGui::CloseCurrentPopup();
                    save_to_disk();
                }
                ImGui::SameLine();
                if(ImGui::Button("Cancel") ||
                   ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape), false)) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if(ImGui::BeginPopup("###DELETE_ENTRY")) {
                assert(board.hovered_index != SIZE_MAX);
                ImGui::Text("Delete entry");
                ImGui::Separator();
                auto entry = board.items[board.hovered_index];
                ImGui::Text(entry.text.c_str());
                ImGui::Text("This action is irreversible");
                if(ImGui::Button("Delete")) {
                    board.items.erase(board.items.begin()+board.hovered_index);
                    ImGui::CloseCurrentPopup();
                    save_to_disk();
                }
                ImGui::SameLine();
                if(ImGui::Button("Cancel") ||
                   ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape), false)) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if(ImGui::BeginPopup("###MOVE_ENTRY")) {
                auto entry_index = board.hovered_index;
                assert(entry_index != SIZE_MAX);
                ImGui::Text("Move entry");
                ImGui::Separator();
                size_t index = 0;
                size_t selected_index = SIZE_MAX;
                for(auto &board: BOARDS) {
                    if(ImGui::MenuItem(board.title.c_str())) {
                        selected_index = index;
                        break;
                    }
                    index ++;
                }
                if(ImGui::MenuItem("Cancel") ||
                   ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape), false)) {
                    ImGui::CloseCurrentPopup();
                }
                if(selected_index != SIZE_MAX) {
                    auto &from_board = board;
                    auto &to_board = BOARDS[selected_index];
                    auto entry = from_board.items[entry_index];
                    from_board.items.erase(from_board.items.begin() + entry_index);
                    to_board.items.push_back(entry);
                    ImGui::CloseCurrentPopup();
                    save_to_disk();
                }
                ImGui::EndPopup();
            }

            if(ImGui::IsWindowHovered() &&
               board.hovered_index == SIZE_MAX &&
               ImGui::IsMouseDoubleClicked(0)) {
                   board.items.push_back({"New Entry"});
            }
            {
                auto next_pos = ImGui::GetWindowPos();
                next_pos.x += ImGui::GetWindowWidth() + 20;
                ImGui::SetNextWindowPos(next_pos);
            }
        }
        ImGui::End();
        if(break_the_loop) break;
        index++;
    }
    if(auto board = new_board_function()) {
        BOARDS.push_back(*board);
        save_to_disk();
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
                save_to_disk();
            }
            if(!ImGui::IsItemFocused()) {
                ImGui::SetKeyboardFocusHere();
            }
            if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape), false)) {
                board->editing_index = SIZE_MAX;
                save_to_disk();
            }
        } else {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0, 10.0));
                bool is_done_copy = entry.is_done;
                ImGui::Checkbox("##CHECK_BOX", &is_done_copy);
                // IsItemClicked() on the whole group will flip entry.is_done
                // Here a temporary is created for Checkbox() to avoid double flipping

                ImGui::SameLine();
                ImU32 text_color = ImGui::GetColorU32(ImGuiCol_Text);
                bool is_hovered = board->hovered_index == index;
                if(entry.is_done) {
                    text_color = is_hovered?
                                    IM_COL32(100, 100, 150, 255):
                                    IM_COL32(100, 100, 100, 255);
                } else if(is_hovered) {
                    text_color = IM_COL32(100, 100, 255, 255);
                }
                ImGui::PushStyleColor(ImGuiCol_Text, text_color);
                    ImGui::TextWrapped(entry.text.c_str());
                ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImGui::Separator();
        }
        ImGui::EndGroup();
        ImGui::PopItemWidth();
        if(ImGui::IsItemHovered()) {
            board->hovered_index = index;
        }
        if(ImGui::IsWindowHovered() && 
           board->hovered_index == index &&
           !ImGui::IsItemHovered()) {
            board->hovered_index = SIZE_MAX;
        }
        if (board->editing_index == SIZE_MAX && ImGui::IsItemClicked()) {
            entry.is_done = !entry.is_done;
            save_to_disk();
        }
        if(ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0)) {
            board->editing_index = index;
        }
        ImGui::PopID();
        index ++;
    }
}

static ContextMenuAction render_board_context_menu(size_t board_index, Board *board) {
    ContextMenuAction action = ContextMenuAction::None;
    if(ImGui::BeginPopupContextWindow(NULL, 1, false)) {
        if(board->hovered_index != SIZE_MAX) {
            ImGui::Separator();
            ImGui::Text("Entry");
            ImGui::Separator();
            if(ImGui::MenuItem("Delete")) {
                action = ContextMenuAction::DeleteEntry;
                ImGui::CloseCurrentPopup();
            }
            if(ImGui::MenuItem("Move")) {
                action = ContextMenuAction::MoveEntry;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::Separator();
        ImGui::Text("Board");
        ImGui::Separator();
        if(ImGui::MenuItem("Rename")) {
            action = ContextMenuAction::RenameBoard;
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::MenuItem("Delete")) {
            action = ContextMenuAction::DeleteBoard;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    return action;
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


static void save_to_disk() {
    auto file = std::ofstream("dear_board.data");
    for(auto const &board: BOARDS) {
        file << "#" << board.title << std::endl;
        for(auto const &item: board.items) {
            if(item.is_done) {
                file << "[x] ";
            } else {
                file << "[ ] ";
            }
            for(char c: item.text) {
                if(c == '\n') {
                    file << "\n... ";
                } else {
                    file << c;
                }
            }
            file << std::endl;
        }
        file << std::endl;
        file << std::endl;
    }
}

static void load_from_disk() {
    auto file = std::ifstream("dear_board.data");
    if(!file.good()) return;
    std::string line; 

    BOARDS.clear();
    while(true) {
        std::getline(file, line);
        if(file.eof()) break;

        if(line[0] == '#') {
            Board b;
            b.title = std::string(line.c_str()+1);

            while(true) {
                std::getline(file, line);
                if(file.eof()) break;

                if(line[0] == '[' && line[2] == ']') {
                    bool is_done = line[1] == 'x';
                    Item item {std::string(line.c_str()+4)};
                    item.is_done = is_done;
                    b.items.push_back(std::move(item));
                } else if(line[0] == '.' && 
                          line[1] == '.' && 
                          line[2] == '.') {
                    Item &item = b.items.back();
                    item.text += "\n";
                    item.text += std::string(line.c_str()+4);
                } else {
                    break;
                }
            }

            BOARDS.push_back(std::move(b));
            std::getline(file, line);
            // To getrid of last of the two empty lines after each board.
        } else {
            std::cerr << "ERROR: Malformed data file" << std::endl;
            break;
        }
    }

}