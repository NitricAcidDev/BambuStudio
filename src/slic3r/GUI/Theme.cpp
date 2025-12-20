#include "Theme.hpp"
#include <wx/colour.h>
#include <cmath>

namespace Slic3r {
namespace GUI {

wxColour shift_hue(const wxColour& color, double hue_shift) {
    double h, s, v;
    color.GetHSB(&h, &s, &v);
    h = fmod(h + hue_shift, 1.0);
    if (h < 0) h += 1.0;
    wxColour new_color;
    new_color.SetHSB(h, s, v);
    return new_color;
}

ThemeColors Theme::get_theme_colors(const std::string& theme_name) {
    // Base colors for Bambu Green
    wxColour base_green(0, 128, 0); // Dark green
    wxColour light_green(144, 238, 144); // Light green
    wxColour darker_green(0, 100, 0);
    wxColour disabled_green(169, 169, 169);

    double hue_shift = 0.0;
    if (theme_name == "Space Purple") {
        hue_shift = 0.75; // Purple
    } else if (theme_name == "Ocean Blue") {
        hue_shift = 0.58; // Blue
    } else if (theme_name == "Candy Red") {
        hue_shift = 0.0; // Red, but adjust
    } // Bambu Green: 0

    wxColour normal = shift_hue(base_green, hue_shift);
    wxColour hovered = shift_hue(light_green, hue_shift);
    wxColour pressed = shift_hue(darker_green, hue_shift);
    wxColour disabled = disabled_green;

    ThemeColors colors;
    colors.button_green = StateColor(
        std::make_pair(normal, StateColor::Normal),
        std::make_pair(hovered, StateColor::Hovered),
        std::make_pair(pressed, StateColor::Pressed),
        std::make_pair(disabled, StateColor::Disabled)
    );

    // For simplicity, use same for others
    colors.button_red = colors.button_green;
    colors.button_blue = colors.button_green;
    colors.button_purple = colors.button_green;

    return colors;
}

} // namespace GUI
} // namespace Slic3r