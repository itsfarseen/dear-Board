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

    //////////////////////////////////////
    // UI Stuff 
    //////////////////////////////////////

    size_t editing_index = SIZE_MAX;
    // Which item is being edited

    size_t hovered_index = SIZE_MAX;
    // which item is being hovered

    std::string title_new;
    // Used as a buffer while renaming the board
};