#include "Theme.hpp"
#include "GUI_App.hpp"

#include <unordered_map>
#include <mutex>
#include <cmath>
#include <algorithm>

#include <wx/log.h>
#include <wx/image.h>

namespace Slic3r { namespace GUI {

namespace {

std::unordered_map<std::string, wxColour> g_theme_colors;
std::once_flag g_init_flag;

inline wxColour from_hex_rgb(int hex)
{
    unsigned r = (hex >> 16) & 0xFFu;
    unsigned g = (hex >> 8) & 0xFFu;
    unsigned b = (hex) & 0xFFu;
    return wxColour(r, g, b);
}

void init_defaults()
{
    // SwitchButton - original colors except green -> bright cyan for visibility
    g_theme_colors["switch.text.checked"]   = from_hex_rgb(0xFFFFFE);
    g_theme_colors["switch.text.normal"]    = from_hex_rgb(0x6B6B6B);
    g_theme_colors["switch.track"]          = from_hex_rgb(0xD9D9D9);
    g_theme_colors["switch.thumb.checked"]  = from_hex_rgb(0x00AEAE); // was 0x00AE42 green, now cyan with same S/V
    g_theme_colors["switch.thumb.normal"]   = from_hex_rgb(0xD9D9D9);

    // SwitchBoard - original colors except green -> bright cyan
    g_theme_colors["switch_board.bg.enabled"]      = from_hex_rgb(0xEEEEEE);
    g_theme_colors["switch_board.bg.disabled"]     = from_hex_rgb(0xCECECE);
    g_theme_colors["switch_board.segment.enabled"] = from_hex_rgb(0x00AEAE); // was 0x00AE42
    g_theme_colors["switch_board.text.selected"]   = from_hex_rgb(0xFFFFFF);
    g_theme_colors["switch_board.text.unselected"] = from_hex_rgb(0x333333);

    // MultiSwitchButton - original colors except green -> bright cyan
    g_theme_colors["multi_switch.bg.not_checked"]         = from_hex_rgb(0xE8E8E8);
    g_theme_colors["multi_switch.bg.normal"]              = from_hex_rgb(0x00AEAE); // was 0x00AE42
    g_theme_colors["multi_switch.bg_grayed.normal"]       = from_hex_rgb(0x6DC4C4); // was 0x6DC48D, hue shifted
    g_theme_colors["multi_switch.text.not_checked"]       = from_hex_rgb(0x6B6B6B);
    g_theme_colors["multi_switch.text.normal"]            = from_hex_rgb(0xFFFFFE);
    g_theme_colors["multi_switch.text_grayed.not_checked"] = from_hex_rgb(0x999999);
    g_theme_colors["multi_switch.text_grayed.normal"]      = from_hex_rgb(0x99DFDF); // was 0x99DFB2, hue shifted

    // ComboBox - original colors except green -> bright cyan
    g_theme_colors["combobox.border.disabled"] = from_hex_rgb(0xDBDBDB);
    g_theme_colors["combobox.border.hovered"]  = from_hex_rgb(0x00AEAE); // was 0x00AE42
    g_theme_colors["combobox.border.normal"]   = from_hex_rgb(0xDBDBDB);

    g_theme_colors["combobox.bg.disabled"] = from_hex_rgb(0xF0F0F1);
    g_theme_colors["combobox.bg.focused"]  = from_hex_rgb(0xEDFAF2);
    g_theme_colors["combobox.bg.normal"]   = from_hex_rgb(0xFFFFFF);

    g_theme_colors["combobox.label.disabled"] = from_hex_rgb(0x909090);
    g_theme_colors["combobox.label.normal"]   = from_hex_rgb(0x262E30);

    // Button - original colors except green -> bright cyan
    g_theme_colors["button.bg.disabled"]  = from_hex_rgb(0xF0F0F1);
    g_theme_colors["button.bg.hovered_checked"] = from_hex_rgb(0x37EEEE); // was 0x37EE7C, hue shifted
    g_theme_colors["button.bg.checked"]   = from_hex_rgb(0x00AEAE); // was 0x00AE42
    g_theme_colors["button.bg.hovered"]   = from_hex_rgb(0xD3D3D3);
    g_theme_colors["button.bg.normal"]    = from_hex_rgb(0xFFFFFF);
    g_theme_colors["button.text.disabled"] = from_hex_rgb(0xD3D3D3);
    g_theme_colors["button.text.normal"]   = from_hex_rgb(0x000000);

    // AxisCtrlButton - original colors except green -> bright cyan
    g_theme_colors["axis_ctrl.border.hovered"] = from_hex_rgb(0x00AEAE); // was 0x00AE42
    g_theme_colors["axis_ctrl.bg.disabled"]   = from_hex_rgb(0xEEEEEE);
    g_theme_colors["axis_ctrl.bg.pressed"]    = from_hex_rgb(0xACACAC);
    g_theme_colors["axis_ctrl.bg.hovered"]    = from_hex_rgb(0xEEEEEE);
    g_theme_colors["axis_ctrl.bg.normal"]     = from_hex_rgb(0xEEEEEE);
    g_theme_colors["axis_ctrl.text.disabled"] = from_hex_rgb(0x6B6B6B);
    g_theme_colors["axis_ctrl.text.normal"]   = from_hex_rgb(0x000000);

    // SideButton - original colors except green -> bright cyan
    g_theme_colors["side_button.border.pressed"] = from_hex_rgb(0x178181); // was 0x17813F dark green, now dark cyan
    g_theme_colors["side_button.border.hovered"] = from_hex_rgb(0x30DDDD); // was 0x30DD70 bright green, now bright cyan
    g_theme_colors["side_button.border.normal"]  = from_hex_rgb(0x00AEAE); // was 0x00AE42 medium green, now medium cyan
    g_theme_colors["side_button.bg.pressed"]     = from_hex_rgb(0x178181); // was 0x17813F
    g_theme_colors["side_button.bg.hovered"]     = from_hex_rgb(0x30DDDD); // was 0x30DD70
    g_theme_colors["side_button.bg.normal"]      = from_hex_rgb(0x00AEAE); // was 0x00AE42

    // ProgressBar & MultiNozzleSync - original colors except green -> bright cyan
    g_theme_colors["progress.button.disabled"] = from_hex_rgb(0x909090);
    g_theme_colors["progress.button.pressed"]  = from_hex_rgb(0x1B8888); // was 0x1B8844, hue shifted
    g_theme_colors["progress.button.hovered"]  = from_hex_rgb(0x3DCBCB); // was 0x3DCB73, hue shifted
    g_theme_colors["progress.button.normal"]   = from_hex_rgb(0x00AEAE); // was 0x00AE42
    g_theme_colors["progress.text.normal"]     = from_hex_rgb(0xFFFFFE);

    // TextInput - original colors except green -> bright cyan
    g_theme_colors["textinput.label.disabled"] = from_hex_rgb(0x909090);
    g_theme_colors["textinput.label.normal"]   = from_hex_rgb(0x6B6B6B);
    g_theme_colors["textinput.text.disabled"]  = from_hex_rgb(0x909090);
    g_theme_colors["textinput.text.normal"]    = from_hex_rgb(0x262E30);
    g_theme_colors["textinput.border.disabled"] = from_hex_rgb(0xDBDBDB);
    g_theme_colors["textinput.border.hovered"]  = from_hex_rgb(0x00AEAE); // was 0x00AE42
    g_theme_colors["textinput.border.normal"]   = from_hex_rgb(0xDBDBDB);
    g_theme_colors["textinput.bg.disabled"]     = from_hex_rgb(0xF0F0F1);
    g_theme_colors["textinput.bg.normal"]       = from_hex_rgb(0xFFFFFF);

    // SpinInput - original colors except green -> bright cyan
    g_theme_colors["spininput.label.disabled"] = from_hex_rgb(0x909090);
    g_theme_colors["spininput.label.normal"]   = from_hex_rgb(0x6B6B6B);
    g_theme_colors["spininput.text.disabled"]  = from_hex_rgb(0x909090);
    g_theme_colors["spininput.text.normal"]    = from_hex_rgb(0x262E30);
    g_theme_colors["spininput.border.disabled"] = from_hex_rgb(0xDBDBDB);
    g_theme_colors["spininput.border.hovered"]  = from_hex_rgb(0x00AEAE); // was 0x00AE42
    g_theme_colors["spininput.border.normal"]   = from_hex_rgb(0xDBDBDB);
    g_theme_colors["spininput.bg.disabled"]     = from_hex_rgb(0xF0F0F1);
    g_theme_colors["spininput.bg.normal"]       = from_hex_rgb(0xFFFFFF);

    // TempInput - original colors except green -> bright cyan
    g_theme_colors["tempinput.label.disabled"] = from_hex_rgb(0xACACAC);
    g_theme_colors["tempinput.label.normal"]   = from_hex_rgb(0x323A3C);
    g_theme_colors["tempinput.text.disabled"]  = from_hex_rgb(0xACACAC);
    g_theme_colors["tempinput.text.normal"]    = from_hex_rgb(0x6B6B6B);
    g_theme_colors["tempinput.border.disabled"] = from_hex_rgb(0xFFFFFF);
    g_theme_colors["tempinput.border.focused"]  = from_hex_rgb(0x00AEAE); // was 0x00AE42
    g_theme_colors["tempinput.border.hovered"]  = from_hex_rgb(0x00AEAE); // was 0x00AE42
    g_theme_colors["tempinput.border.normal"]   = from_hex_rgb(0xFFFFFF);
    g_theme_colors["tempinput.bg.disabled"]     = from_hex_rgb(0xFFFFFF);
    g_theme_colors["tempinput.bg.normal"]       = from_hex_rgb(0xFFFFFF);

    // AMSMaterialsSetting - original colors except green -> bright cyan
    g_theme_colors["ams_material.border.focused"] = from_hex_rgb(0xDBDBDB);
    g_theme_colors["ams_material.border.hovered"] = from_hex_rgb(0x00AEAE); // was 0x00AE42

    // DropDown menu items - original colors except green -> bright cyan
    g_theme_colors["dropdown.selector_border.hovered"] = from_hex_rgb(0x00AEAE); // was 0x00AE42
    g_theme_colors["dropdown.selector_border.normal"]  = from_hex_rgb(0xFFFFFF);
    g_theme_colors["dropdown.selector_bg.checked"]     = from_hex_rgb(0xEDFAF2);
    g_theme_colors["dropdown.selector_bg.normal"]      = from_hex_rgb(0xFFFFFF);
}

inline void ensure_init()
{
    std::call_once(g_init_flag, [](){ init_defaults(); });
}

// HSV conversion helpers for registry hue shifts (float variant).
void rgb_to_hsv(unsigned r, unsigned g, unsigned b, float &h, float &s, float &v)
{
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f;
    float maxc = std::fmax(std::fmax(rf, gf), bf);
    float minc = std::fmin(std::fmin(rf, gf), bf);
    v = maxc;
    float delta = maxc - minc;
    s = (maxc == 0.f) ? 0.f : delta / maxc;
    if (delta == 0.f) {
        h = 0.f;
    } else {
        if (maxc == rf)      h = 60.f * std::fmod(((gf - bf) / delta), 6.f);
        else if (maxc == gf) h = 60.f * (((bf - rf) / delta) + 2.f);
        else                 h = 60.f * (((rf - gf) / delta) + 4.f);
        if (h < 0.f) h += 360.f;
    }
}

void hsv_to_rgb(float h, float s, float v, unsigned &r, unsigned &g, unsigned &b)
{
    float c = v * s;
    float x = c * (1.f - std::fabs(std::fmod(h / 60.f, 2.f) - 1.f));
    float m = v - c;
    float rf = 0.f, gf = 0.f, bf = 0.f;
    if (h < 60.f)       { rf = c; gf = x; bf = 0; }
    else if (h < 120.f) { rf = x; gf = c; bf = 0; }
    else if (h < 180.f) { rf = 0; gf = c; bf = x; }
    else if (h < 240.f) { rf = 0; gf = x; bf = c; }
    else if (h < 300.f) { rf = x; gf = 0; bf = c; }
    else                { rf = c; gf = 0; bf = x; }
    r = static_cast<unsigned>(std::round((rf + m) * 255.f));
    g = static_cast<unsigned>(std::round((gf + m) * 255.f));
    b = static_cast<unsigned>(std::round((bf + m) * 255.f));
}

} // namespace

void Theme::setThemeColor(const std::string &key, const wxColour &value)
{
    ensure_init();
    g_theme_colors[key] = value;
}

bool Theme::hasThemeColor(const std::string &key)
{
    ensure_init();
    return g_theme_colors.find(key) != g_theme_colors.end();
}

wxColour Theme::getThemeColor(const std::string &key)
{
    ensure_init();
    auto it = g_theme_colors.find(key);
    if (it == g_theme_colors.end()) {
        wxLogError("Theme color key not found: %s", key.c_str());
        // No fallbacks: return an explicitly invalid color to catch misuse.
        return wxColour(255, 0, 255);
    }
    return it->second;
}

wxColour Theme::getThemeColorShifted(const std::string &key, float degrees)
{
    wxColour base = getThemeColor(key);
    unsigned r = base.Red(), g = base.Green(), b = base.Blue();
    float h, s, v; rgb_to_hsv(r, g, b, h, s, v);
    h = std::fmod(h + degrees, 360.f);
    if (h < 0.f) h += 360.f;
    unsigned rr, gg, bb; hsv_to_rgb(h, s, v, rr, gg, bb);
    return wxColour(rr, gg, bb);
}

// Manual RGB to HSV conversion (double variant) for legacy hue adjustments.
void rgb_to_hsv(unsigned char r, unsigned char g, unsigned char b, double &h, double &s, double &v)
{
    double rd = r / 255.0;
    double gd = g / 255.0;
    double bd = b / 255.0;

    double max_val = std::max({rd, gd, bd});
    double min_val = std::min({rd, gd, bd});
    double delta   = max_val - min_val;

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

wxColour hsv_to_rgb(double h, double s, double v)
{
    if (s <= 0.0) {
        unsigned char val = static_cast<unsigned char>(v * 255);
        return wxColour(val, val, val);
    }

    h = std::fmod(h, 1.0);
    if (h < 0) h += 1.0;
    h *= 6.0;

    int i = static_cast<int>(std::floor(h));
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

wxColour shift_hue(const wxColour &color, double hue_shift)
{
    double h, s, v;
    rgb_to_hsv(color.Red(), color.Green(), color.Blue(), h, s, v);
    h = std::fmod(h + hue_shift, 1.0);
    if (h < 0) h += 1.0;
    return hsv_to_rgb(h, s, v);
}

static StateColor make_button(const wxColour &normal,
                              const wxColour &hovered,
                              const wxColour &pressed,
                              const wxColour &disabled = wxColour(169, 169, 169))
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

ThemeColors Theme::get_theme_colors(const std::string &theme_name)
{
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
        colors.button_blue   = colors.button_green;
        colors.button_red    = colors.button_green;
    } else if (name == "Ocean Blue") {
        colors.button_green = make_button(wxColour(31, 142, 234),   // normal
                                          wxColour(74, 167, 240),   // hovered
                                          wxColour(27, 111, 184));  // pressed
        colors.button_blue   = colors.button_green;
        colors.button_purple = colors.button_green;
        colors.button_red    = colors.button_green;
    } else if (name == "Candy Red") {
        colors.button_green = make_button(wxColour(208, 27, 27),    // normal
                                          wxColour(226, 85, 85),    // hovered
                                          wxColour(160, 19, 19));   // pressed
        colors.button_red    = colors.button_green;
        colors.button_blue   = colors.button_green;
        colors.button_purple = colors.button_green;
    } else { // Bambu Green default
        colors.button_green = make_button(wxColour(0, 174, 66),    // normal
                                          wxColour(55, 238, 124),   // hovered
                                          wxColour(27, 136, 68));   // pressed
        colors.button_red    = colors.button_green;
        colors.button_blue   = colors.button_green;
        colors.button_purple = colors.button_green;
    }

    return colors;
}

} } // namespace Slic3r::GUI