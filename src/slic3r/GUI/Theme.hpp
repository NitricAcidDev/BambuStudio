// Theme.hpp - theme color definitions
#ifndef slic3r_GUI_Theme_hpp_
#define slic3r_GUI_Theme_hpp_

#include <wx/colour.h>
#include <string>

namespace Slic3r { namespace GUI {

struct ButtonGreen {
    wxColour disabled;
    wxColour pressed;
    wxColour hovered;
    wxColour normal;
};

struct ThemeColors {
    wxColour primary;      // Main accent color
    wxColour secondary;    // Secondary color
    wxColour background;   // Background
    wxColour foreground;   // Text
    ButtonGreen button_green;
};

class Theme {
public:
    static ThemeColors get_theme_colors(const std::string& theme_name);
    static wxColour shift_hue(const wxColour& color, double target_hue);
};

}} // namespace Slic3r::GUI

#endif // slic3r_GUI_Theme_hpp_
