#ifndef __WIDGETS_HPP__
#define __WIDGETS_HPP__

#include "models.hpp"
#include <imgui.h>

/*
 * - Focus of children are managed by parents.
 * - A child should respond to inputs only if it is
 *   set as focused by parent.
 * - A child is allowed to request focus from parent if mouse is on it
 *   or similar event occurs.
 * - The parent should propagate up the request if the parent is not focused.
 */


enum class ItemWidgetAction {
    None,
    ItemUpdated,
    RequestEdit,
    RequestFocus,
    RequestContextMenu,
    HoverEnter,
    HoverExit
};
ItemWidgetAction ItemWidget(int index, Item &item, bool is_focused, bool is_hovered);

#endif // __WIDGETS_HPP__