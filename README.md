# BetterAltTab_Unnamed10110 v3

A modern, ultra-fast, and highly customizable Alt+Tab replacement for Windows, written in C++ with pure Win32 API. **Version 3** introduces advanced number input overlay, comprehensive Spanish documentation, INI-based configuration system, settings dialog, dynamic thumbnail sizing, and enhanced user experience features.

---

## 🚀 What's New in v3

### 🎯 Advanced Number Input System
- **Ctrl+Number Overlay**: When the main overlay is open, press and hold Ctrl to activate a minimalistic number input overlay
- **Precise Window Selection**: Type any number (1 to N, where N is the number of windows) and press Enter to instantly focus that window
- **OLED Black Design**: The number input overlay features a sleek black background with white text and rounded corners
- **Escape to Cancel**: Press Escape to close the number input overlay without selecting a window
- **Standalone Window**: The input overlay operates as an independent window for better focus management

### ⚙️ INI Configuration System
- **Dynamic Configuration**: All application settings can be modified via `BetterAltTab.ini` without recompilation
- **Configurable Parameters**: 
  - Number of columns in the grid (1-19)
  - Overlay opacity (0-255)
  - Hotkey definitions
  - Color customization (borders, highlights, etc.)
  - Overlay size percentages
- **Runtime Updates**: Settings are loaded on startup and can be modified through the settings dialog
- **Backward Compatibility**: Default values are used if configuration file is missing

### 🎛️ Settings Dialog
- **Tray Menu Integration**: Right-click the tray icon and select "Settings..." to open the configuration dialog
- **Real-time Updates**: Changes are applied immediately and saved to the INI file
- **User-friendly Interface**: Simple dialog with input validation and error handling
- **Persistent Storage**: All settings are automatically saved to `BetterAltTab.ini`

### 📐 Dynamic Thumbnail Sizing
- **Automatic Scaling**: Thumbnail sizes automatically adjust based on the configured number of columns
- **Aspect Ratio Preservation**: Thumbnails maintain their original aspect ratio while scaling
- **No Horizontal Scrolling**: All thumbnails fit within the overlay width without requiring horizontal scrolling
- **Responsive Layout**: Grid layout adapts to different screen sizes and column configurations

### 🖱️ Enhanced Mouse Support
- **Wheel Scrolling**: 
  - Normal mouse wheel: Vertical scrolling through windows
  - Shift + mouse wheel: Horizontal scrolling through windows
- **Hover Effects**: Visual feedback when hovering over window thumbnails and buttons
- **Click Actions**: Direct interaction with pin, close, and pin-to-position buttons
- **Drag and Drop**: Visual feedback during drag operations (prepared for future implementation)

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

### Mouse Navigation
- **Vertical Scroll**: Use mouse wheel to scroll through windows vertically
- **Horizontal Scroll**: Hold Shift and use mouse wheel to scroll horizontally
- **Hover Selection**: Move mouse over windows to highlight them
- **Click to Select**: Click on any window thumbnail to select it

### Quick Actions
- **Close Window**: Click the `X` icon on any window thumbnail
- **Pin/Unpin**: Click the pin icon to pin/unpin a window
- **Pin to Position**: Click the `#` icon to assign a specific position number (automatically pins the window)

### Mode Switching
- **Toggle Modes**: Press `F2` to switch between Dynamic Order and Persistent Z-Order modes
- **Visual Indicator**: The button appearance changes to show current mode
aaaaasas
### Configuration
- **Open Settings**: Right-click the tray icon and select "Settings..."
- **Modify Columns**: Change the number of columns displayed in the grid
- **Apply Changes**: Click "Apply" to save changes immediately

### Visual Demostration

![Demo](https://github.com/Unnamed10110/BetterAltTab_Unnamed10110/raw/master/Carnac_27_07_2025_08_23_56.gif)


![Carnac_27_07_2025_03_17_11](https://github.com/user-attachments/assets/2dfc02ba-87be-4296-9f64-fe90e6d58d55)


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
- **INI Configuration**: External configuration file for all settings
- **Settings Dialog**: User-friendly interface for configuration changes
- **Dynamic Thumbnail Sizing**: Automatic scaling based on column count
- **Enhanced Mouse Support**: Wheel scrolling and hover effects
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
- **Configuration Driven**: Runtime configuration without recompilation

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

### INI File Configuration
The application uses `BetterAltTab.ini` for all configuration settings:

```ini
[General]
; Number of columns shown in the grid (1-19 recommended)
Columns=9

; Overlay opacity (0 = fully transparent, 255 = fully opaque)
OverlayAlpha=230

; Hot-key definitions – virtual-key codes or characters
; Leave blank to keep compiled defaults
;CtrlHotkey=VK_DECIMAL   ; decimal on numpad
;AltQHotkey=Q            ; letter Q combined with Alt

; Colour customisation (RGB hex, e.g. 00FF44 is bright green)
;BorderColor=202030
;HoverBorderColor=50B4FF
;HighlightColor=00FF44

; Per-monitor overlay size – expressed as percentage of work-area
;OverlayWidthPct=88
;OverlayHeightPct=80
```

### Overlay Settings
- **Size**: Defaults to 88% screen width, 80% height (configurable)
- **Colors**: OLED black theme with customizable accent colors
- **Transparency**: Adjustable alpha blending for background
- **Grid Layout**: Dynamic column layout with customizable spacing

### Input Configuration
- **Numpad Keys**: Direct 1-9, 0 for 10th window selection
- **Ctrl Overlay**: Customizable size and position
- **Hotkeys**: Configurable global shortcuts in main.cpp
- **Mouse Wheel**: Vertical and horizontal scrolling support

### Persistence
- **Window Order**: Saved in `grid_order.bin`
- **Pin States**: Preserved across sessions
- **Settings**: Runtime configuration changes persist in INI file

---

## 🧩 Technical Architecture

### Core Components
- **Window Manager**: Handles window enumeration and state management
- **Overlay System**: Layered window with custom painting
- **Input Handler**: Dual input system for keyboard and mouse
- **Configuration System**: INI file parsing and settings management
- **Settings Dialog**: Win32 common controls for user configuration
- **Persistence Layer**: Binary file storage for user preferences
- **Notification System**: Virtual desktop and system event handling

### Key Technologies
- **Win32 API**: Core Windows functionality
- **DWM API**: Desktop Window Manager for thumbnails
- **COM Interfaces**: Virtual desktop notifications
- **GDI+**: Custom drawing and UI elements
- **Shell API**: Tray icon and system integration
- **INI File API**: Configuration file management

### Performance Optimizations
- **Double Buffering**: Flicker-free rendering
- **Lazy Loading**: Thumbnails loaded on-demand
- **Efficient Sorting**: Optimized window ordering algorithms
- **Memory Management**: Proper resource cleanup and reuse
- **Dynamic Sizing**: Runtime thumbnail size calculations

---

## 🌟 Key Advantages

### Performance
- **Instant Response**: No lag or delay in overlay display
- **Low Resource Usage**: Minimal CPU and memory footprint
- **Smooth Animations**: Hardware-accelerated rendering
- **Scalable**: Handles 50+ windows efficiently
- **Dynamic Layout**: Automatic adaptation to different configurations

### User Experience
- **Intuitive Interface**: Familiar Alt+Tab workflow with enhancements
- **Flexible Input**: Multiple ways to select windows (keyboard, mouse, number input)
- **Visual Feedback**: Clear indicators for all actions
- **Accessibility**: Keyboard and mouse support
- **Customizable**: Extensive configuration options

### Developer Experience
- **Well Documented**: Comprehensive Spanish comments
- **Modular Design**: Clean separation of concerns
- **Extensible**: Easy to add new features
- **Maintainable**: Clear code structure and organization
- **Configuration Driven**: Runtime changes without recompilation

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
- Added INI configuration system
- Implemented settings dialog with tray menu integration
- Added dynamic thumbnail sizing
- Enhanced mouse wheel scrolling (vertical and horizontal)
- Improved pin and number pin button functionality
- Added comprehensive Spanish documentation
- Enhanced input handling and visual feedback
- Better error handling and resource management

### v2.0
- Added Ctrl+number input overlay
- Persistent tray icon
- Virtual desktop support
- Enhanced navigation
- OLED theme
- Performance improvements

### v1.0
- Basic Alt+Tab replacement
- Window thumbnails
- Hotkey support 
