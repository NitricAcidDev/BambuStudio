#ifndef SLIC3R_GUI_THEME_HPP
#define SLIC3R_GUI_THEME_HPP

#include <wx/colour.h>
#include <string>

namespace Slic3r { namespace GUI { namespace Theme {

// Returns the theme color registered under `key`.
// Keys must exist; this function does not provide fallbacks.
wxColour getThemeColor(const std::string &key);

// Returns the theme color for `key` with hue shifted by `degrees`.
// Positive values rotate hue clockwise in HSV space.
wxColour getThemeColorShifted(const std::string &key, float degrees);

// Registers or updates a theme color value for `key`.
void setThemeColor(const std::string &key, const wxColour &value);

// Checks whether a theme color exists for `key`.
bool hasThemeColor(const std::string &key);

}}} // namespace Slic3r::GUI::Theme

#endif // SLIC3R_GUI_THEME_HPP
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
