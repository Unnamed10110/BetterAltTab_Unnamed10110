# BetterAltTab_Unnamed10110 v2

A modern, ultra-fast, and highly customizable Alt+Tab replacement for Windows, written in C++ with pure Win32 API. **Version 2** brings even more power-user features, robust tray integration, seamless virtual desktop support, and a beautiful, efficient overlay.

---

## 🚀 What's New in v2

- **Persistent Tray Icon**: The tray icon is always visible and auto-recovers after Explorer restarts or crashes.
- **Robust Alt+Q Navigation**: Open the overlay with Alt+Q, keep holding Alt, and use arrow keys to navigate. Release Alt to focus the selected window. Navigation only works while Alt is held, for a true power-user workflow.
- **Circular Navigation**: Arrow navigation wraps around—pressing right on the last window jumps to the first, and left on the first jumps to the last.
- **Efficient Overlay**: Uses a layered, double-buffered, rounded window with true transparency for smooth, flicker-free visuals and minimal CPU usage.
- **Virtual Desktop Awareness**: Instantly updates when you switch virtual desktops, always showing the correct windows.
- **Persistent Z-Order & Pinning**: Pin windows, reorder them, and your layout persists across sessions and even after Explorer crashes.
- **Quick Actions**: Close, pin, and move-to-position actions for each window, accessible via mouse or keyboard.
- **OLED Black Theme**: Overlay and popups use a true black background for perfect OLED displays.
- **Minimal Dependencies**: Pure Win32 API, no Qt/.NET/third-party frameworks.
- **Super Fast**: Instant overlay, no lag, optimized for large numbers of windows.

---

## Demostration
```
# Quick commented guide:
- Press atl+q to open the overlay, with F2 you can change between dynamic mode and persistence z-order mode (you will notice the difference on the button 
right to the thumbnails)
- When cycling with alt+q, and you decide what window focus, just press again alt and it will focus that window (old behavior).
- Ctrl+Numpaddot has the same behavior as alt+tab (alt+q is an upgraded behavior)
```

- Dynamic Mode (Classic Mode)
  <img width="1689" height="832" alt="image" src="https://github.com/user-attachments/assets/04c8c545-55e2-4e63-b75d-eb8fc92b1970" />
- Z-order Persistence Mode:
-  You can pin multiple windows at the beginning and order then, also it mantains the z-order, independently from the process explorer.exe crashing or not.
  <img width="1689" height="832" alt="image" src="https://github.com/user-attachments/assets/a6f18638-2851-4243-b41b-93dd0c2c8e3a" />
- You can pin as many as you want, they always keep they position unless you changed, you can temrinate them, and give then a pinned gerarquy order:
- You also have the option to use shortcuts to acceso to the most importants windows pinned as important with jus the number in the window position
  <img width="1689" height="832" alt="image" src="https://github.com/user-attachments/assets/23789cc3-7a79-4c8c-a394-902621cb7a9e" />

---

## 🖥️ How to Use

### Hotkeys
- **Open Overlay**: `Ctrl+NumpadDot` or `Alt+Q`
- **Navigate**: While holding Alt, use arrow keys or Tab to move selection (navigation only works while Alt is held)
- **Cycle Selection**: Keep holding the modifier and press the hotkey again
- **Select by Number**: Press `1`-`9` or `0` (for 10th) to instantly select/focus a window
- **Close Overlay**: Release the modifier, press `Esc`, or click outside

### Quick Actions (on hover)
- **Close**: Click the `X` icon
- **Pin/Unpin**: Click the pin icon
- **Pin to Position**: Click the `#` icon, enter a number, press Enter

---

## 🛠️ Build Instructions

### Prerequisites
- Windows 10/11 (x64)
- Visual Studio 2019/2022 or MSVC toolchain
- CMake 3.10+

### Steps
1. Clone the repository:
   ```sh
   git clone <your-repo-url>
   cd BetterAltTab_Unnamed10110
   ```
2. Configure and build:
   ```sh
   cmake -S . -B build
   cmake --build build --config Release
   ```
3. Run:
   ```sh
   build/Release/BetterAltTab_Unnamed10110.exe
   ```

---

## ⚙️ Configuration & Customization
- **Persistent Z-Order**: Window order and pin state are saved in `grid_order.bin`.
- **Overlay Size**: Defaults to 88% screen width, 80% height; resizable at runtime.
- **Theme**: OLED black by default; easily customizable in `main.cpp`.
- **Hotkeys**: Change in `main.cpp` (`RegisterHotKey` calls).

---

## 🧩 Technical Details & Libraries Used
- **Language**: C++17, pure Win32 API
- **Libraries/Headers**:
  - `<windows.h>`: Core Win32 API
  - `<dwmapi.h>`: DWM thumbnails (live previews)
  - `<shobjidl.h>`, `<shellapi.h>`: Virtual desktop and tray icon management
  - `<commctrl.h>`, `<uxtheme.h>`: Modern UI, theme, and controls
  - `<objbase.h>`: COM for virtual desktop notifications
  - `<psapi.h>`: Process info for window icons
  - `<shlwapi.h>`, `<strsafe.h>`: Utility APIs
- **Techniques**:
  - **Layered, Rounded Overlay**: Uses `WS_EX_LAYERED`, `SetLayeredWindowAttributes`, and custom region for a modern, transparent overlay
  - **Double Buffering**: Off-screen drawing for flicker-free UI
  - **DWM Thumbnails**: `DwmRegisterThumbnail` for live window previews
  - **Persistent Tray Icon**: Handles `WM_TASKBARCREATED` to restore the icon after Explorer restarts
  - **Virtual Desktop Notifications**: Implements `IVirtualDesktopNotification` via COM to update overlay on desktop switch
  - **Hotkey Registration**: Uses `RegisterHotKey` for global shortcuts
  - **Custom Input Handling**: Full keyboard and mouse support, including Alt+arrow navigation, number selection, and quick actions
  - **Persistence**: Custom binary format for window order and pin state

---

## 🌟 Strong Points
- **Blazing Fast**: Instant overlay, no lag, even with 50+ windows
- **No Bloat**: No Qt, .NET, or heavy dependencies
- **Modern Look**: Rounded corners, transparency, OLED black, smooth UI
- **Power User Features**: Pinning, persistent order, search, keyboard shortcuts, quick actions
- **Portable**: Single EXE, no installer needed
- **Open Source**: Easily hackable and extensible

---

## 📝 License
MIT

---

## 🙏 Credits
- Inspired by Windows 11 Alt+Tab, PowerToys, and the open-source community.
- Contact: Unnamed10110 - trojan.v6@gmail.com / sergiobritos10110@gmail.com

---

## 💬 Feedback & Contributions
Pull requests, issues, and suggestions are welcome! 
