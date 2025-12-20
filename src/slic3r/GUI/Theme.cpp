#include "Theme.hpp"
#include <wx/colour.h>
#include <wx/image.h>
#include <cmath>
#include <algorithm>

namespace Slic3r {
namespace GUI {

// Manual RGB to HSV conversion
void rgb_to_hsv(unsigned char r, unsigned char g, unsigned char b, double& h, double& s, double& v) {
    double rd = r / 255.0;
    double gd = g / 255.0;
    double bd = b / 255.0;
    
    double max_val = std::max({rd, gd, bd});
    double min_val = std::min({rd, gd, bd});
    double delta = max_val - min_val;
    
    v = max_val;
    
    if (delta < 0.00001) {
        s = 0;
        h = 0;
        return;
    }
    
    if (max_val > 0.0) {
        s = delta / max_val;
    } else {
        s = 0.0;
        h = 0.0;
        return;
    }
    
    if (rd >= max_val)
        h = (gd - bd) / delta;
    else if (gd >= max_val)
        h = 2.0 + (bd - rd) / delta;
    else
        h = 4.0 + (rd - gd) / delta;
    
    h /= 6.0;
    if (h < 0.0)
        h += 1.0;
}

// Manual HSV to RGB conversion
wxColour hsv_to_rgb(double h, double s, double v) {
    if (s <= 0.0) {
        unsigned char val = static_cast<unsigned char>(v * 255);
        return wxColour(val, val, val);
    }
    
    h = fmod(h, 1.0);
    if (h < 0) h += 1.0;
    h *= 6.0;
    
    int i = static_cast<int>(floor(h));
    double f = h - i;
    double p = v * (1.0 - s);
    double q = v * (1.0 - s * f);
    double t = v * (1.0 - s * (1.0 - f));
    
    double r, g, b;
    switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }
    
    return wxColour(
        static_cast<unsigned char>(r * 255),
        static_cast<unsigned char>(g * 255),
        static_cast<unsigned char>(b * 255)
    );
}

wxColour shift_hue(const wxColour& color, double hue_shift) {
    double h, s, v;
    rgb_to_hsv(color.Red(), color.Green(), color.Blue(), h, s, v);
    h = fmod(h + hue_shift, 1.0);
    if (h < 0) h += 1.0;
    return hsv_to_rgb(h, s, v);
}

ThemeColors Theme::get_theme_colors(const std::string& theme_name) {
    // Base colors for Bambu Green
    wxColour base_green(0, 174, 66); // 0x00AE42
    wxColour light_green(55, 238, 124); // 0x37EE7C
    wxColour darker_green(0, 100, 0);
    wxColour disabled_green(169, 169, 169);

    double hue_shift = 0.0;
    if (theme_name == "Space Purple") {
        hue_shift = 0.75; // Purple
    } else if (theme_name == "Ocean Blue") {
        hue_shift = 0.58; // Blue
    } else if (theme_name == "Candy Red") {
        hue_shift = 0.0; // Red
    } // Bambu Green: 0

    wxColour normal = shift_hue(base_green, hue_shift);
    wxColour hovered = shift_hue(light_green, hue_shift);
    wxColour pressed = shift_hue(darker_green, hue_shift);
    wxColour disabled = disabled_green;

    ThemeColors colors;
    colors.button_green.append(disabled, StateColor::Disabled);
    colors.button_green.append(hovered, StateColor::Hovered | StateColor::Checked);
    colors.button_green.append(normal, StateColor::Checked);
    colors.button_green.append(pressed, StateColor::Pressed);
    colors.button_green.append(wxColour(255, 255, 255), StateColor::Normal);

    // For simplicity, use same for others
    colors.button_red = colors.button_green;
    colors.button_blue = colors.button_green;
    colors.button_purple = colors.button_green;

    return colors;
}

} // namespace GUI
} // namespace Slic3r