# BetterAltTab_Unnamed10110 v3

A modern, ultra-fast, and highly customizable Alt+Tab replacement for Windows, written in C++ with pure Win32 API. **Version 3** introduces advanced number input overlay, comprehensive Spanish documentation, and enhanced user experience features.

---

## 🚀 What's New in v3

### 🎯 Advanced Number Input System
- **Ctrl+Number Overlay**: When the main overlay is open, press and hold Ctrl to activate a minimalistic number input overlay
- **Precise Window Selection**: Type any number (1 to N, where N is the number of windows) and press Enter to instantly focus that window
- **OLED Black Design**: The number input overlay features a sleek black background with white text and rounded corners
- **Escape to Cancel**: Press Escape to close the number input overlay without selecting a window
- **Standalone Window**: The input overlay operates as an independent window for better focus management

### 🌍 Comprehensive Spanish Documentation
- **Paraguayan Spanish Comments**: All configuration variables, defines, pragmas, includes, and functions are documented in Spanish
- **Detailed Function Documentation**: Every function, method, and class includes explanatory comments about its purpose and usage
- **Organized Code Structure**: Clear sections for overlay configuration, state variables, and utility functions
- **Enhanced Maintainability**: Code is now more accessible to Spanish-speaking developers

### ⚡ Enhanced User Experience
- **Dual Input Methods**: 
  - Traditional numpad keys (1-9, 0 for 10th) for quick selection
  - New Ctrl+number overlay for precise input of any window number
- **Improved Visual Feedback**: Better hover states and visual indicators
- **Optimized Performance**: Enhanced timer-based input detection for reliable numpad key handling
- **Better Error Handling**: Robust input validation and graceful fallbacks

### 🔧 Technical Improvements
- **Subclassed Edit Controls**: Direct message handling for Enter/Escape keys in the number input overlay
- **Enhanced Timer System**: Improved WM_TIMER handling for both numpad detection and Ctrl overlay management
- **Better Resource Management**: Proper cleanup of overlay windows and edit controls
- **Debug Logging**: Enhanced debugging capabilities with detailed logging of user interactions

---

## 🎮 How to Use

### Main Overlay (Alt+Q)
- **Open Overlay**: Press `Alt+Q` to open the main window switcher
- **Navigate**: While holding Alt, use arrow keys to move selection
- **Quick Selection**: Press numpad keys `1`-`9` or `0` (for 10th window) for instant selection
- **Close**: Release Alt to focus selected window, or press `Esc`

### Advanced Number Input (Ctrl)
- **Activate**: With main overlay open, press and hold `Ctrl`
- **Input Number**: Type any number from 1 to N (where N is total windows)
- **Confirm**: Press `Enter` to focus the selected window
- **Cancel**: Press `Escape` to close without selecting
- **Visual**: Minimalistic black overlay with white text and rounded corners

### Quick Actions
- **Close Window**: Click the `X` icon on any window thumbnail
- **Pin/Unpin**: Click the pin icon to pin/unpin a window
- **Pin to Position**: Click the `#` icon to assign a specific position number

### Mode Switching
- **Toggle Modes**: Press `F2` to switch between Dynamic Order and Persistent Z-Order modes
- **Visual Indicator**: The button appearance changes to show current mode

---

## 🖥️ Features

### Core Functionality
- **Ultra-Fast Overlay**: Instant display with smooth animations
- **Live Window Previews**: Real-time thumbnails using DWM API
- **Virtual Desktop Support**: Automatically updates when switching desktops
- **Persistent Layout**: Window order and pin states saved between sessions
- **Circular Navigation**: Arrow keys wrap around window list

### Advanced Features
- **Dual Input Systems**: Both numpad and precise number input
- **OLED-Optimized Theme**: True black backgrounds for OLED displays
- **Rounded Corners**: Modern UI with custom window regions
- **Transparency Effects**: Layered windows with alpha blending
- **Tray Integration**: Persistent system tray icon with context menu

### Technical Excellence
- **Pure Win32 API**: No external dependencies or frameworks
- **Memory Efficient**: Optimized for handling large numbers of windows
- **Thread Safe**: Proper synchronization for multi-threaded operations
- **Error Resilient**: Graceful handling of window state changes
- **Cross-Desktop**: Works seamlessly across virtual desktops

---

## 🛠️ Build Instructions

### Prerequisites
- Windows 10/11 (x64)
- Visual Studio 2019/2022 or MSVC toolchain
- CMake 3.10+

### Build Steps
1. **Clone Repository**:
   ```sh
   git clone <your-repo-url>
   cd BetterAltTab_Unnamed10110
   ```

2. **Configure and Build**:
   ```sh
   cmake -S . -B build
   cmake --build build --config Release
   ```

3. **Run Application**:
   ```sh
   build/Release/BetterAltTab_Unnamed10110.exe
   ```

### Auto-Start Setup
The application automatically registers itself to run as administrator on login for optimal functionality.

---

## ⚙️ Configuration

### Overlay Settings
- **Size**: Defaults to 88% screen width, 80% height
- **Colors**: OLED black theme with customizable accent colors
- **Transparency**: Adjustable alpha blending for background
- **Grid Layout**: Fixed 5-column layout with customizable spacing

### Input Configuration
- **Numpad Keys**: Direct 1-9, 0 for 10th window selection
- **Ctrl Overlay**: Customizable size and position
- **Hotkeys**: Configurable global shortcuts in main.cpp

### Persistence
- **Window Order**: Saved in `grid_order.bin`
- **Pin States**: Preserved across sessions
- **Settings**: Runtime configuration changes persist

---

## 🧩 Technical Architecture

### Core Components
- **Window Manager**: Handles window enumeration and state management
- **Overlay System**: Layered window with custom painting
- **Input Handler**: Dual input system for keyboard and mouse
- **Persistence Layer**: Binary file storage for user preferences
- **Notification System**: Virtual desktop and system event handling

### Key Technologies
- **Win32 API**: Core Windows functionality
- **DWM API**: Desktop Window Manager for thumbnails
- **COM Interfaces**: Virtual desktop notifications
- **GDI+**: Custom drawing and UI elements
- **Shell API**: Tray icon and system integration

### Performance Optimizations
- **Double Buffering**: Flicker-free rendering
- **Lazy Loading**: Thumbnails loaded on-demand
- **Efficient Sorting**: Optimized window ordering algorithms
- **Memory Management**: Proper resource cleanup and reuse

---

## 🌟 Key Advantages

### Performance
- **Instant Response**: No lag or delay in overlay display
- **Low Resource Usage**: Minimal CPU and memory footprint
- **Smooth Animations**: Hardware-accelerated rendering
- **Scalable**: Handles 50+ windows efficiently

### User Experience
- **Intuitive Interface**: Familiar Alt+Tab workflow with enhancements
- **Flexible Input**: Multiple ways to select windows
- **Visual Feedback**: Clear indicators for all actions
- **Accessibility**: Keyboard and mouse support

### Developer Experience
- **Well Documented**: Comprehensive Spanish comments
- **Modular Design**: Clean separation of concerns
- **Extensible**: Easy to add new features
- **Maintainable**: Clear code structure and organization

---

## 📝 License
MIT License - See LICENSE file for details

---

## 🙏 Credits & Contact
- **Developer**: Unnamed10110
- **Email**: trojan.v6@gmail.com / sergiobritos10110@gmail.com
- **Inspiration**: Windows 11 Alt+Tab, PowerToys, and open-source community

---

## 💬 Contributing
We welcome contributions! Please feel free to:
- Submit bug reports and feature requests
- Contribute code improvements
- Suggest enhancements
- Help with documentation

---

## 🔄 Version History

### v3.0 (Current)
- Added Ctrl+number input overlay
- Comprehensive Spanish documentation
- Enhanced input handling
- Improved visual design
- Better error handling

### v2.0
- Persistent tray icon
- Virtual desktop support
- Enhanced navigation
- OLED theme
- Performance improvements

### v1.0
- Basic Alt+Tab replacement
- Window thumbnails
- Hotkey support 
