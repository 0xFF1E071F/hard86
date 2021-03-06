/*  Hard86 - An 8086 Emulator with support for virtual hardware
	
    Copyright (C) 2014 Stephen Zhang

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.	
*/

#ifndef HARD86_MEM_WATCHER_H
#define HARD86_MEM_WATCHER_H

#include <Windows.h>

#include "../global.h"
#include "toolwindow.h"
#include "hexgrid.h"

#include "../../../objwin32/src/gui/scrollbar.h"
#include "../../../objwin32/src/gui/editbox.h"
#include "../../../objwin32/src/gui/button.h"

#include "../../../../../system/multithreading/mutex/cpp/mutex.h"

namespace nsHard86Win32{

using namespace nsObjWin32::nsGUI;

class MemoryWatcher : public Hard86ToolWindow{
public:
	MemoryWatcher();

	virtual ~MemoryWatcher(){
		DestroyWindow(m_hWnd);
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND Create(int x, int y, int w, int h, HWND hwndParent, HMENU hMenu=NULL){
		Window::Create(m_exStyle, L"Memory", m_style, x, y, w, h, hwndParent, hMenu);
		Show();
		return m_hWnd;
	}
private:

	// Message handlers
#define MSGHANDLER(name) void On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Create);
	MSGHANDLER(Size);
	MSGHANDLER(Command);
	MSGHANDLER(VScroll);

#undef MSGHANDLER

	void CreateChildren(HWND hWnd);

	enum Children{
		MEMGRID,
		SCROLLBAR,
		ADDRBOX,
		SEGBOX,
		GOTO_BUTTON
	};

	WinManager m_children;

	template<typename T>
	T*& Child(int id){
		return (T*&)m_children[id];
	}

	static bool m_registered;

	uint32 GetAddr();
	uint32 GetSeg();
	void SetAddr(uint32 i);
	void SetSeg(uint32 i);

	uint32 m_lastBaseAddr;

	bool m_enabled;

};

}

#endif