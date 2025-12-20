#include "Theme.hpp"
#include <algorithm>

namespace Slic3r { namespace GUI {

wxColour Theme::shift_hue(const wxColour& color, double target_hue) {
    // Convert RGB to HSV
    double r = color.Red() / 255.0;
    double g = color.Green() / 255.0;
    double b = color.Blue() / 255.0;

    double max = std::max({r, g, b});
    double min = std::min({r, g, b});
    double delta = max - min;

    double h, s, v;
    v = max;
    if (delta == 0) {
        h = 0;
        s = 0;
    } else {
        s = delta / max;
        if (r == max) h = (g - b) / delta;
        else if (g == max) h = 2 + (b - r) / delta;
        else h = 4 + (r - g) / delta;
        h *= 60;
        if (h < 0) h += 360;
    }

    // Set new hue
    h = target_hue;

    // Convert back to RGB
    double c = v * s;
    double x = c * (1 - std::abs(std::fmod(h / 60, 2) - 1));
    double m = v - c;

    double r2, g2, b2;
    if (h >= 0 && h < 60) { r2 = c; g2 = x; b2 = 0; }
    else if (h >= 60 && h < 120) { r2 = x; g2 = c; b2 = 0; }
    else if (h >= 120 && h < 180) { r2 = 0; g2 = c; b2 = x; }
    else if (h >= 180 && h < 240) { r2 = 0; g2 = x; b2 = c; }
    else if (h >= 240 && h < 300) { r2 = x; g2 = 0; b2 = c; }
    else { r2 = c; g2 = 0; b2 = x; }

    return wxColour((r2 + m) * 255, (g2 + m) * 255, (b2 + m) * 255);
}

ThemeColors Theme::get_theme_colors(const std::string& theme_name) {
    ThemeColors colors;
    // Base colors
    wxColour base_primary(0, 174, 66); // #00AE42
    wxColour base_secondary(144, 144, 144); // DESIGN_GRAY600_COLOR
    wxColour base_background(248, 248, 248); // DESIGN_SELECTOR_NOMORE_COLOR
    wxColour base_foreground(38, 46, 48); // DESIGN_GRAY900_COLOR

    double target_hue = 138; // Default green hue
    if (theme_name == "space_purple") {
        target_hue = 286;
    } else if (theme_name == "ocean_blue") {
        target_hue = 192;
    } else if (theme_name == "candy_red") {
        target_hue = 343;
    }

    colors.primary = shift_hue(base_primary, target_hue);
    colors.secondary = shift_hue(base_secondary, target_hue);
    colors.background = base_background; // Don't shift grays
    colors.foreground = base_foreground;

    return colors;
}

}} // namespace Slic3r::GUI