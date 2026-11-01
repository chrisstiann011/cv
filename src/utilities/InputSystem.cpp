#include "../Precompiled.h"

bool Input::Setup( )
{
	while (m_hWindow == nullptr)
	{
		EnumWindows(EnumWindowsCallback, reinterpret_cast< LPARAM >(&m_hWindow));
		LI_FN(Sleep)( 200U );
	}

	m_pOldWndProc = reinterpret_cast< WNDPROC >(LI_FN(SetWindowLongPtrW)( m_hWindow, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( Hooks::hkWndProc ) ) );

	if (m_pOldWndProc == nullptr)
		return false;

	return true;
}

void Input::Restore( )
{
	if (m_pOldWndProc != nullptr)
	{
		LI_FN(SetWindowLongPtrW)( m_hWindow, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( m_pOldWndProc ) );
		m_pOldWndProc = nullptr;
	}
}

bool Input::Process(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    if (Gui::m_bOpen && wParam != Config::i(g_Variables.m_Gui.m_iMenuKey) && wParam != Config::i(g_Variables.m_Gui.m_iUnloadKey))
        return false;

    WPARAM nKey = 0;

    EKeyState state = EKeyState::NONE;

    switch (uMsg)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (wParam < 256U)
        {
            nKey = wParam;
            state = EKeyState::DOWN;

            if (wParam == VK_SHIFT || wParam == VK_CONTROL || wParam == VK_MENU)
            {
                WPARAM extended = MapVirtualKeyW((lParam >> 16) & 0xFF, MAPVK_VSC_TO_VK_EX);
                if (extended)
                {
                    m_arrKeyState.at(extended) = state;
                }
            }
        }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (wParam < 256U)
        {
            nKey = wParam;
            state = EKeyState::UP;

            if (wParam == VK_SHIFT || wParam == VK_CONTROL || wParam == VK_MENU)
            {
                WPARAM extended = MapVirtualKeyW((lParam >> 16) & 0xFF, MAPVK_VSC_TO_VK_EX);
                if (extended)
                {
                    if (state == EKeyState::UP && m_arrKeyState.at(extended) == EKeyState::DOWN)
                        m_arrKeyState.at(extended) = EKeyState::RELEASED;
                    else
                        m_arrKeyState.at(extended) = state;
                }
            }
        }
        break;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        nKey = VK_LBUTTON;
        state = EKeyState::DOWN;
        break;
    case WM_LBUTTONUP:
        nKey = VK_LBUTTON;
        state = EKeyState::UP;
        break;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
        nKey = VK_RBUTTON;
        state = EKeyState::DOWN;
        break;
    case WM_RBUTTONUP:
        nKey = VK_RBUTTON;
        state = EKeyState::UP;
        break;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
        nKey = VK_MBUTTON;
        state = EKeyState::DOWN;
        break;
    case WM_MBUTTONUP:
        nKey = VK_MBUTTON;
        state = EKeyState::UP;
        break;
    case WM_XBUTTONDOWN:
    case WM_XBUTTONDBLCLK:
    {
        uint16_t xButton = ( (WORD)( ( ( (DWORD_PTR)( ( wParam ) ) >> 16 ) & 0xffff ) ) );
        nKey = ( xButton == XBUTTON1 ) ? VK_XBUTTON1 : VK_XBUTTON2;
        state = EKeyState::DOWN;
        break;
    }
    case WM_XBUTTONUP:
    {
        uint16_t xButton = ( (WORD)( ( ( (DWORD_PTR)( ( wParam ) ) >> 16 ) & 0xffff ) ) );
        nKey = ( xButton == XBUTTON1 ) ? VK_XBUTTON1 : VK_XBUTTON2;
        state = EKeyState::UP;
        break;
    }
    default:
        return false;
    }

    if (nKey != 0 && state != EKeyState::NONE)
    {

        if (state == EKeyState::UP && m_arrKeyState.at(nKey) == EKeyState::DOWN)
            m_arrKeyState.at(nKey) = EKeyState::RELEASED;
        else
            m_arrKeyState.at(nKey) = state;

        return true;
    }

    return false;
}