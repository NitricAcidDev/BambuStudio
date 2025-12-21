#include "Theme.hpp"
#include <wx/colour.h>
#include <wx/image.h>
#include <cmath>
#include <algorithm>


static wxColour ThemeThemeGreen() {
    auto *app = dynamic_cast<Slic3r::GUI::GUI_App*>(&Slic3r::GUI::wxGetApp());
    if (app && app->IsMainLoopRunning()) {
        return app->get_theme_colors().button_green.colorForStates(StateColor::Normal | StateColor::Enabled);
    }
    return wxColour(0, 174, 66);
}

static wxColour ThemeThemeGreenHovered() {
    auto *app = dynamic_cast<Slic3r::GUI::GUI_App*>(&Slic3r::GUI::wxGetApp());
    if (app && app->IsMainLoopRunning()) {
        return app->get_theme_colors().button_green.colorForStates(StateColor::Hovered | StateColor::Enabled);
    }
    return wxColour(61, 203, 115);
}

static wxColour ThemeThemeGreenPressed() {
    auto *app = dynamic_cast<Slic3r::GUI::GUI_App*>(&Slic3r::GUI::wxGetApp());
    if (app && app->IsMainLoopRunning()) {
        return app->get_theme_colors().button_green.colorForStates(StateColor::Pressed | StateColor::Enabled);
    }
    return wxColour(27, 136, 68);
}

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

static StateColor make_button(const wxColour& normal,
                              const wxColour& hovered,
                              const wxColour& pressed,
                              const wxColour& disabled = wxColour(169, 169, 169))
{
    StateColor sc;
    sc.append(disabled, StateColor::Disabled);
    sc.append(normal, StateColor::Normal | StateColor::Enabled);
    sc.append(hovered, StateColor::Hovered | StateColor::Enabled);
    sc.append(pressed, StateColor::Pressed | StateColor::Enabled);
    // Checked state follows normal to avoid gray unpressed arrows.
    sc.append(normal, StateColor::Checked | StateColor::Enabled);
    return sc;
}

ThemeColors Theme::get_theme_colors(const std::string& theme_name) {
    // Accept both user-facing names and stored ids (theme_color preference)
    std::string name = theme_name;
    if (name == "bambu_green") name = "Bambu Green";
    else if (name == "space_purple") name = "Space Purple";
    else if (name == "ocean_blue") name = "Ocean Blue";
    else if (name == "candy_red") name = "Candy Red";

    ThemeColors colors;

    if (name == "Space Purple") {
        colors.button_green = make_button(wxColour(123, 92, 255),  // normal
                                          wxColour(154, 127, 255), // hovered
                                          wxColour(90, 58, 207));  // pressed
        colors.button_purple = colors.button_green;
        colors.button_blue = colors.button_green;
        colors.button_red = colors.button_green;
    } else if (name == "Ocean Blue") {
        colors.button_green = make_button(wxColour(31, 142, 234),   // normal
                                          wxColour(74, 167, 240),   // hovered
                                          wxColour(27, 111, 184));  // pressed
        colors.button_blue = colors.button_green;
        colors.button_purple = colors.button_green;
        colors.button_red = colors.button_green;
    } else if (name == "Candy Red") {
        colors.button_green = make_button(wxColour(208, 27, 27),    // normal
                                          wxColour(226, 85, 85),    // hovered
                                          wxColour(160, 19, 19));   // pressed
        colors.button_red = colors.button_green;
        colors.button_blue = colors.button_green;
        colors.button_purple = colors.button_green;
    } else { // Bambu Green default
        colors.button_green = make_button(ThemeThemeGreen(),     // normal
                                          wxColour(55, 238, 124),   // hovered
                                          ThemeThemeGreenPressed());   // pressed
        colors.button_red = colors.button_green;
        colors.button_blue = colors.button_green;
        colors.button_purple = colors.button_green;
    }

    return colors;
}

} // namespace GUI
} // namespace Slic3r