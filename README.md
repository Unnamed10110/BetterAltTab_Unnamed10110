# BetterAltTab_Unnamed10110 v6

A modern, ultra-fast, and highly customizable Alt+Tab replacement for Windows, written in C++ with pure Win32 API. **Version 6** introduces advanced filtering system, dual hotkey modes, intelligent performance optimization, comprehensive settings management, and enhanced user experience features.

---

## 🚀 What's New in v6

### 🎯 Advanced Filtering System
- **Alt+A Hotkey**: Dedicated hotkey for persistent filtering mode with search box
- **Real-time Filtering**: Dynamic thumbnail updates as you type in the search box
- **Multi-criteria Filtering**: Filter by window title, class name, and process name
- **Smart Search Box**: Centered, OLED-themed search input with automatic clearing
- **Persistent Mode**: Overlay stays open while filtering for extended search sessions
- **Arrow Key Navigation**: Navigate through filtered results with arrow keys
- **Dynamic Thumbnail Display**: Thumbnails appear/disappear based on filter criteria

### ⚡ Dual Hotkey System
- **Alt+Q**: Classic window switching mode (original functionality)
- **Alt+A**: Advanced filtering mode with persistent overlay
- **Independent Operation**: Each mode operates independently without interference
- **Toggle Control**: Enable/disable Alt+A functionality from tray menu
- **Persistent State**: Alt+A state saved and restored between sessions

### 🚀 Intelligent Performance Optimization
- **Full Performance Mode**: Optimized for 16GB+ RAM systems with maximum responsiveness
- **Full Low Resources Mode**: Adaptive configuration based on current system resources
- **Automatic Resource Analysis**: Detects RAM, CPU, and disk usage for optimal settings
- **One-Click Optimization**: Apply performance profiles from tray menu
- **Real-time Adaptation**: Settings adjust based on system performance metrics
- **Battery Life Optimization**: Low resource mode extends laptop battery life

### 🎛️ Enhanced Settings Management
- **Dark OLED Settings Overlay**: Modern, themed settings dialog with custom tabs
- **Comprehensive Configuration**: All settings accessible through organized interface
- **Real-time Preview**: See changes immediately without restarting
- **INI File Integration**: All settings automatically saved to configuration file
- **Performance Profiles**: Quick access to optimization presets
- **Custom Tab System**: Organized settings into logical categories

### 🔧 Advanced Window Management
- **Automatic Window Detection**: Enhanced window enumeration for better application coverage
- **Real-time Thumbnail Updates**: Thumbnails update dynamically as windows change
- **Automatic Cleanup**: Closed windows automatically removed from overlay
- **Window Hook System**: System-wide monitoring for window state changes
- **Memory Management**: Intelligent thumbnail cache management
- **Performance Monitoring**: Real-time resource usage tracking

### 🎨 Enhanced User Interface
- **OLED Black Theme**: True black backgrounds for OLED displays
- **Custom Tab Buttons**: Modern tab interface without default Windows controls
- **Responsive Layout**: Adaptive grid system with dynamic column sizing
- **Visual Feedback**: Enhanced hover states and selection indicators
- **Professional Appearance**: Clean, modern design with consistent theming

---

## 🎮 How to Use

### Classic Mode (Alt+Q)
- **Open Overlay**: Press `Alt+Q` to open the classic window switcher
- **Navigate**: While holding Alt, use arrow keys to move selection
- **Quick Selection**: Press numpad keys `1`-`9` or `0` (for 10th window) for instant selection
- **Close**: Release Alt to focus selected window, or press `Esc`

### Advanced Filtering Mode (Alt+A)
- **Activate Filtering**: Press `Alt+A` to open overlay with filtering enabled
- **Search**: Type in the search box to filter windows in real-time
- **Navigate Results**: Use arrow keys to move through filtered results
- **Select Window**: Press Enter or Alt to focus the selected window
- **Persistent Mode**: Overlay remains open until you select a window or press Escape

### Performance Optimization
- **Full Performance**: Right-click tray icon → "Full Performance" for maximum speed
- **Low Resources**: Right-click tray icon → "Full Low Resources" for battery life
- **Automatic Analysis**: System resources analyzed and optimized automatically
- **Profile Switching**: Change performance profiles anytime from tray menu

### Advanced Navigation
- **Arrow Keys**: Navigate through windows with full wrap-around support
- **Mouse Wheel**: Vertical scrolling through window list
- **Hover Selection**: Move mouse over windows to highlight them
- **Click to Select**: Click on any window thumbnail to select it

### Quick Actions
- **Close Window**: Click the `X` icon on any window thumbnail
- **Pin/Unpin**: Click the pin icon to pin/unpin a window
- **Pin to Position**: Click the `#` icon to assign a specific position number

### Configuration
- **Open Settings**: Right-click the tray icon and select "Settings..."
- **Performance Modes**: Access optimization profiles from tray menu
- **Toggle Alt+A**: Enable/disable filtering mode from tray menu
- **Dynamic Mode**: Switch between dynamic and fixed window ordering

---

## 🖥️ Features

### Core Functionality
- **Ultra-Fast Overlay**: Instant display with smooth animations
- **Live Window Previews**: Real-time thumbnails using DWM API
- **Virtual Desktop Support**: Automatically updates when switching desktops
- **Persistent Layout**: Window order and pin states saved between sessions
- **Circular Navigation**: Arrow keys wrap around window list

### Advanced Features
- **Dual Hotkey System**: Alt+Q for classic mode, Alt+A for filtering
- **Real-time Filtering**: Dynamic search with instant results
- **Performance Optimization**: Intelligent resource management
- **Comprehensive Settings**: Dark OLED themed configuration interface
- **Window Monitoring**: Automatic detection of window state changes
- **Memory Management**: Intelligent thumbnail caching and cleanup
- **INI Configuration**: External configuration file for all settings
- **Dynamic Thumbnail Sizing**: Automatic scaling based on column count

### Technical Excellence
- **Pure Win32 API**: No external dependencies or frameworks
- **Memory Efficient**: Optimized for handling large numbers of windows
- **Thread Safe**: Proper synchronization for multi-threaded operations
- **Error Resilient**: Graceful handling of window state changes
- **Cross-Desktop**: Works seamlessly across virtual desktops
- **Configuration Driven**: Runtime configuration without recompilation
- **Performance Monitoring**: Real-time resource usage tracking
- **Automatic Optimization**: Self-adjusting performance parameters

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
   build/Release/BetterAltTab_Unnamed10110_Optimized.exe
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
Columns=6
; Overlay opacity (0 = fully transparent, 255 = fully opaque)
OverlayAlpha=255
; Enable/disable Alt+A filtering functionality
AltAEnabled=1

[Performance]
; Low resource mode (auto/on/off)
LowResourceMode=auto
; Maximum thumbnail cache size
MaxThumbnailCache=20
; Normal refresh rate in milliseconds
NormalRefreshRate=25
; Low resource refresh rate in milliseconds
LowResourceRefreshRate=100
; Minimum thumbnail size in pixels
MinThumbnailSize=200
; Memory threshold for low resource mode (MB)
MemoryThreshold=512
; CPU threshold for low resource mode (percentage)
CPUThreshold=80
; Reserve memory for critical operations
ReserveMemory=true
; Optimize process priority
OptimizePriority=true
; Reduce visual effects in low resource mode
ReduceVisualEffects=true
; Automatic cleanup interval (milliseconds)
AutoCleanupInterval=30000
; Maximum thumbnail memory usage (MB)
MaxThumbnailMemory=100

[UI]
; Show system tray icon
ShowTrayIcon=1
; Show system information
ShowSystemInfo=1
; Show window count
ShowWindowCount=1
; Show memory usage
ShowMemoryUsage=1
; Show performance mode indicator
ShowPerformanceMode=1

[Hotkeys]
; Alt+Q key (classic mode)
AltQKey=Q
; Alt+A key (filtering mode)
AltAKey=A
; Numpad modifier key
NumpadModifier=0

[Behavior]
; Dynamic window ordering
DynamicOrder=1
; Save window order between sessions
SaveWindowOrder=1
; Show closed windows
ShowClosedWindows=0
; Auto-hide overlay
AutoHideOverlay=1
; Show filter indicator
ShowFilterIndicator=1
; Enable sounds
EnableSounds=0
```

### Performance Profiles

#### Full Performance Mode
- **Target**: 16GB+ RAM systems
- **Settings**: Maximum responsiveness, high quality thumbnails, extended caching
- **Use Case**: Gaming, development, high-performance workstations

#### Full Low Resources Mode
- **Target**: Resource-constrained systems
- **Settings**: Battery optimization, minimal resource usage, adaptive configuration
- **Use Case**: Laptops, older systems, battery life optimization

---

## 🧩 Technical Architecture

### Core Components
- **Window Manager**: Enhanced window enumeration and state management
- **Filtering Engine**: Real-time search and filtering system
- **Performance Monitor**: Resource usage tracking and optimization
- **Settings Manager**: Comprehensive configuration management
- **Overlay System**: Layered window with custom painting
- **Input Handler**: Dual hotkey system for different modes
- **Configuration System**: INI file parsing and settings management
- **Window Hook System**: System-wide event monitoring

### Key Technologies
- **Win32 API**: Core Windows functionality
- **DWM API**: Desktop Window Manager for thumbnails
- **COM Interfaces**: Virtual desktop notifications
- **GDI+**: Custom drawing and UI elements
- **Shell API**: Tray icon and system integration
- **INI File API**: Configuration file management
- **Windows Hooks**: System-wide event monitoring
- **Performance Counters**: Resource usage tracking

### Performance Optimizations
- **Intelligent Caching**: Adaptive thumbnail cache management
- **Resource Monitoring**: Real-time system resource analysis
- **Dynamic Optimization**: Automatic performance parameter adjustment
- **Memory Management**: Efficient thumbnail memory usage
- **Cleanup Automation**: Intelligent cache cleanup scheduling

---

## 🌟 Key Advantages

### Performance
- **Instant Response**: No lag or delay in overlay display
- **Intelligent Resource Management**: Automatic optimization based on system state
- **Adaptive Performance**: Self-adjusting parameters for optimal operation
- **Efficient Caching**: Smart thumbnail management for better performance
- **Battery Optimization**: Low resource mode for extended laptop usage

### User Experience
- **Dual Mode Operation**: Classic Alt+Q and advanced Alt+A filtering
- **Real-time Search**: Instant filtering as you type
- **Persistent Filtering**: Extended search sessions without overlay closing
- **Performance Profiles**: One-click optimization for different use cases
- **Professional Interface**: Modern, themed settings and controls

### Developer Experience
- **Comprehensive Documentation**: Detailed code comments and explanations
- **Modular Design**: Clean separation of concerns
- **Extensible Architecture**: Easy to add new features
- **Configuration Driven**: Runtime changes without recompilation
- **Performance Monitoring**: Built-in resource tracking and optimization

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

### v6.0 (Current)
- **Advanced Filtering System**: Real-time search with Alt+A hotkey
- **Dual Hotkey Modes**: Alt+Q (classic) and Alt+A (filtering)
- **Intelligent Performance Optimization**: Full Performance and Full Low Resources modes
- **Enhanced Settings Interface**: Dark OLED themed settings dialog with custom tabs
- **Automatic Window Management**: Real-time thumbnail updates and cleanup
- **Performance Monitoring**: Resource usage tracking and automatic optimization
- **Enhanced User Experience**: Persistent filtering mode with search box
- **Professional UI**: Modern interface design with consistent theming

### v5.0
- **INI Configuration System**: External configuration file for all settings
- **Settings Dialog**: User-friendly interface with tray menu integration
- **Dynamic Thumbnail Sizing**: Automatic scaling based on column count
- **Enhanced Mouse Support**: Wheel scrolling and hover effects
- **Comprehensive Spanish Documentation**: Detailed code comments and explanations

### v4.0
- **Advanced Number Input System**: Ctrl+number overlay for precise selection
- **Enhanced Navigation**: Improved arrow key and mouse wheel support
- **OLED Theme**: True black backgrounds for OLED displays
- **Performance Improvements**: Better memory management and responsiveness

### v3.0
- **Basic Alt+Tab Replacement**: Window thumbnails and hotkey support
- **Core Functionality**: Essential window switching capabilities
- **Foundation**: Basic architecture and user interface 
