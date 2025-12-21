#ifndef THEME_HPP
#define THEME_HPP

#include "Widgets/StateColor.hpp"

#include <string>
#include <wx/colour.h>

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
    static ThemeColors get_theme_colors(const std::string &theme_name = "Bambu Green");

    // Centralized color registry accessors (no fallbacks).
    static wxColour getThemeColor(const std::string &key);
    static wxColour getThemeColorShifted(const std::string &key, float degrees);
    static void     setThemeColor(const std::string &key, const wxColour &value);
    static bool     hasThemeColor(const std::string &key);
};

} // namespace GUI
} // namespace Slic3r

#endif // THEME_HPP
