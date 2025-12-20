#ifndef THEME_HPP
#define THEME_HPP

#include "Widgets/StateColor.hpp"
#include <string>

namespace Slic3r {
namespace GUI {

struct ThemeColors {
    StateColor button_green;
    StateColor button_red;
    StateColor button_blue;
    StateColor button_purple;
};

class Theme {
public:
    static ThemeColors get_theme_colors(const std::string& theme_name = "Bambu Green");
};

} // namespace GUI
} // namespace Slic3r

#endif // THEME_HPP
