#pragma once

enum class EKeyState : int
{
	NONE,
	DOWN,
	UP,
	RELEASED
};
static BOOL CALLBACK EnumWindowsCallback( HWND handle, LPARAM lParam )
{
	const auto MainWindow = [ handle ]( )
	{
		return GetWindow( handle, GW_OWNER ) == nullptr &&
			IsWindowVisible( handle ) && handle != GetConsoleWindow( );
	};

	DWORD nPID = 0;
	GetWindowThreadProcessId( handle, &nPID );

	if (GetCurrentProcessId( ) != nPID || !MainWindow( ))
		return TRUE;

	*reinterpret_cast< HWND* >( lParam ) = handle;
	return FALSE;
}

namespace Input
{

	inline HWND m_hWindow = nullptr;

	inline WNDPROC m_pOldWndProc = nullptr;

	inline std::array<EKeyState, 256U + 5U> m_arrKeyState = {};
	inline bool arrKeyBooleans[256U + 5U];

	bool Setup( );
	void Restore( );
	bool Process( UINT uMsg, WPARAM wParam, LPARAM lParam );

	inline bool IsKeyDown( const std::uint32_t uButtonCode )
	{
		return m_arrKeyState.at( uButtonCode ) == EKeyState::DOWN;
	}

	inline bool IsKeyReleased( const std::uint32_t uButtonCode )
	{
		if (m_arrKeyState.at( uButtonCode ) == EKeyState::RELEASED)
		{
			m_arrKeyState.at( uButtonCode ) = EKeyState::UP;
			return true;
		}

		return false;
	}

	inline bool HandleInput( KeyBind_t& pKeyBind )
	{
		if (pKeyBind.m_iKey == 0)
			return false;

		if (pKeyBind.m_iMode == HOLD)
		{
			return IsKeyDown( pKeyBind.m_iKey );
		}
		else if (pKeyBind.m_iMode == TOGGLE)
		{
			if (IsKeyReleased( pKeyBind.m_iKey ))
				arrKeyBooleans[ pKeyBind.m_iKey ] = !arrKeyBooleans[ pKeyBind.m_iKey ];

			return arrKeyBooleans[ pKeyBind.m_iKey ];
		}
		else if (pKeyBind.m_iMode == ALWAYS_ON)
		{
			return true;
		}

		return false;
	}
}