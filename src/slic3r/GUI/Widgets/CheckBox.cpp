#include "CheckBox.hpp"

#include "../GUI_App.hpp"
#include "../Theme.hpp"
#include "../wxExtensions.hpp"

#include <wx/settings.h>
#include <iomanip>
#include <sstream>
#include <vector>

namespace {
std::string to_hex(const wxColour &c)
{
    std::ostringstream oss;
    oss << "#" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << int(c.Red())
        << std::setw(2) << std::setfill('0') << int(c.Green())
        << std::setw(2) << std::setfill('0') << int(c.Blue());
    return oss.str();
}
}

CheckBox::CheckBox(wxWindow *parent, int id)
    : wxBitmapToggleButton(parent, id, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    , m_on(this, "check_on", 18)
    , m_half(this, "check_half", 18)
    , m_off(this, "check_off", 18)
    , m_on_disabled(this, "check_on_disabled", 18)
    , m_half_disabled(this, "check_half_disabled", 18)
    , m_off_disabled(this, "check_off_disabled", 18)
    , m_on_focused(this, "check_on_focused", 18)
    , m_half_focused(this, "check_half_focused", 18)
    , m_off_focused(this, "check_off_focused", 18)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	if (parent)
		SetBackgroundColour(parent->GetBackgroundColour());
	Bind(wxEVT_TOGGLEBUTTON, [this](auto& e) { m_half_checked = false; update(); e.Skip(); });
	Bind(wxEVT_PAINT, [this](wxPaintEvent& e) {
		wxPaintDC dc(this);
		dc.SetBackground(wxBrush(GetBackgroundColour()));
		dc.Clear();
		wxSize sz = GetSize();
		auto bmp = GetBitmapLabel();
		if (bmp.IsOk()) {
			wxSize bmpSz = bmp.GetSize();
			dc.DrawBitmap(bmp, (sz.x - bmpSz.x) / 2, (sz.y - bmpSz.y) / 2, true);
		}
	});
#ifdef __WXOSX__ // State not fully implement on MacOS
    Bind(wxEVT_SET_FOCUS, &CheckBox::updateBitmap, this);
    Bind(wxEVT_KILL_FOCUS, &CheckBox::updateBitmap, this);
    Bind(wxEVT_ENTER_WINDOW, &CheckBox::updateBitmap, this);
    Bind(wxEVT_LEAVE_WINDOW, &CheckBox::updateBitmap, this);
#endif
	auto bmpSize = m_on.GetBmpSize();
	SetSize(wxSize(bmpSize.x + FromDIP(8), bmpSize.y + FromDIP(8)));
	SetMinSize(wxSize(bmpSize.x + FromDIP(8), bmpSize.y + FromDIP(8)));

    rebuild_bitmaps();
	update();
}

void CheckBox::SetValue(bool value)
{
    if (wxBitmapToggleButton::GetValue() != value) {
        wxBitmapToggleButton::SetValue(value);
        update();
    }
}

void CheckBox::SetHalfChecked(bool value)
{
	m_half_checked = value;
	update();
}

void CheckBox::Rescale()
{
    m_on.msw_rescale();
    m_half.msw_rescale();
    m_off.msw_rescale();
    m_on_disabled.msw_rescale();
    m_half_disabled.msw_rescale();
    m_off_disabled.msw_rescale();
    m_on_focused.msw_rescale();
    m_half_focused.msw_rescale();
    m_off_focused.msw_rescale();
    SetSize(m_on.GetBmpSize());
	update();
}

void CheckBox::update()
{
	rebuild_bitmaps();
	SetBitmapLabel((m_half_checked ? m_half : GetValue() ? m_on : m_off).bmp());
    SetBitmapDisabled((m_half_checked ? m_half_disabled : GetValue() ? m_on_disabled : m_off_disabled).bmp());
#ifdef __WXMSW__
    SetBitmapFocus((m_half_checked ? m_half_focused : GetValue() ? m_on_focused : m_off_focused).bmp());
#endif
    SetBitmapCurrent((m_half_checked ? m_half_focused : GetValue() ? m_on_focused : m_off_focused).bmp());

    // Always use parent background color - no themed highlight on checkbox
    const wxColour bg_normal = GetParent() ? GetParent()->GetBackgroundColour() : wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

    if (bg_normal != GetBackgroundColour()) {
        SetBackgroundColour(bg_normal);
        Refresh();
    }
#ifdef __WXOSX__
    wxCommandEvent e(wxEVT_UPDATE_UI);
    updateBitmap(e);
#endif
}

#ifdef __WXMSW__

CheckBox::State CheckBox::GetNormalState() const { return State_Normal; }

#endif

void CheckBox::rebuild_bitmaps()
{
    const std::string accent_hex = to_hex(Slic3r::GUI::Theme::getThemeColor("button.bg.checked"));
    if (accent_hex == m_color_hex)
        return;

    m_color_hex = accent_hex;

    auto make = [&](const char *name) {
        return ScalableBitmap(this, name, 18, false, false, false, std::vector<std::string>{m_color_hex});
    };

    m_on            = make("check_on");
    m_half          = make("check_half");
    m_off           = make("check_off");
    m_on_disabled   = make("check_on_disabled");
    m_half_disabled = make("check_half_disabled");
    m_off_disabled  = make("check_off_disabled");
    m_on_focused    = make("check_on_focused");
    m_half_focused  = make("check_half_focused");
    m_off_focused   = make("check_off_focused");

    auto bmpSize = m_on.GetBmpSize();
    SetSize(wxSize(bmpSize.x + FromDIP(8), bmpSize.y + FromDIP(8)));
    SetMinSize(wxSize(bmpSize.x + FromDIP(8), bmpSize.y + FromDIP(8)));
}


#ifdef __WXOSX__

bool CheckBox::Enable(bool enable)
{
    bool result = wxBitmapToggleButton::Enable(enable);
    if (result) {
        m_disable = !enable;
        wxCommandEvent e(wxEVT_ACTIVATE);
        updateBitmap(e);
    }
    return result;
}

wxBitmap CheckBox::DoGetBitmap(State which) const
{
    if (m_disable) {
        return wxBitmapToggleButton::DoGetBitmap(State_Disabled);
    }
    if (m_focus) {
        return wxBitmapToggleButton::DoGetBitmap(State_Current);
    }
    return wxBitmapToggleButton::DoGetBitmap(which);
}

void CheckBox::updateBitmap(wxEvent & evt)
{
    evt.Skip();
    if (evt.GetEventType() == wxEVT_ENTER_WINDOW) {
        m_hover = true;
    } else if (evt.GetEventType() == wxEVT_LEAVE_WINDOW) {
        m_hover = false;
    } else {
        if (evt.GetEventType() == wxEVT_SET_FOCUS) {
            m_focus = true;
        } else if (evt.GetEventType() == wxEVT_KILL_FOCUS) {
            m_focus = false;
        }
        wxMouseEvent e;
        if (m_hover)	
            OnEnterWindow(e);
        else
            OnLeaveWindow(e);
    }
}
	
#endif
