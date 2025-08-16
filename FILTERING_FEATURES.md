# BetterAltTab Filtering Features

## Overview
BetterAltTab now includes powerful filtering capabilities that allow you to control which windows are displayed in the Alt+Tab interface. This feature helps you focus on relevant applications and hide unwanted windows.

## Quick Start

### Enable Filtering
- **F3 Key**: Toggle filtering on/off
- **Ctrl+F**: Activate quick filter input (type and press Enter)
- **Tray Menu**: Right-click tray icon → "Filters..."

### Basic Usage
1. Press **F3** to enable filtering
2. Use **Ctrl+F** to quickly type a filter term
3. Press **Enter** to apply the filter
4. Press **Escape** to close quick filter or disable filtering

## Filter Types

### Text-Based Filtering
- **Title**: Filter by window title text
- **Class**: Filter by window class name
- **Process**: Filter by executable name

### Exclusion Rules
- **Process Exclusion**: Hide all windows from specific applications
- **Class Exclusion**: Hide all windows of specific types
- **Hidden Windows**: Option to exclude invisible windows
- **Minimized Windows**: Option to exclude minimized windows

### Inclusion Rules (Whitelist)
- **Process Whitelist**: Show only windows from specific applications
- **Class Whitelist**: Show only windows of specific types

## Configuration

### INI File Settings
All filter settings are stored in `BetterAltTab.ini` under the `[Filters]` section:

```ini
[Filters]
Enabled=1                    ; Enable/disable filtering
FilterByTitle=1             ; Filter by window title
FilterByClassName=1         ; Filter by window class
FilterByProcessName=1       ; Filter by process name
CaseSensitive=0             ; Case-sensitive search
ExcludeHidden=1             ; Exclude hidden windows
ExcludeMinimized=0          ; Exclude minimized windows
ExcludedProcesses=          ; Processes to exclude (separated by |)
ExcludedClasses=            ; Classes to exclude (separated by |)
IncludedProcesses=          ; Processes to include (whitelist)
IncludedClasses=            ; Classes to include (whitelist)
```

### Examples

#### Show Only Web Browsers
```ini
[Filters]
Enabled=1
FilterByTitle=1
FilterByClassName=0
FilterByProcessName=1
IncludedProcesses=chrome.exe|firefox.exe|edge.exe|opera.exe
```

#### Exclude System Windows
```ini
[Filters]
Enabled=1
ExcludedClasses=Shell_TrayWnd|Shell_SecondaryTrayWnd|Shell_NotifyWnd
```

#### Show Only Work Applications
```ini
[Filters]
Enabled=1
IncludedProcesses=excel.exe|word.exe|outlook.exe|teams.exe
```

## Keyboard Shortcuts

| Shortcut | Action |
|----------|---------|
| **F3** | Toggle filtering on/off |
| **Ctrl+F** | Activate quick filter input |
| **Enter** | Apply quick filter |
| **Escape** | Close quick filter or disable filtering |

## Context Menu Options

Right-click on any window thumbnail to access additional filtering options:

- **Pin/Unpin**: Fix window position
- **Exclude Process**: Hide all windows from this application
- **Exclude Class**: Hide all windows of this type

## Visual Indicators

When filtering is active, you'll see:
- **Yellow text** in the top-left corner showing filter status
- **Window count** (filtered/total)
- **Filter text** if a text filter is active

## Performance Considerations

- Filtering is applied in real-time as you type
- Filter settings are automatically saved
- Window enumeration is cached for better performance
- Filters are applied before window ordering

## Troubleshooting

### Filter Not Working
1. Check if filtering is enabled (F3 key)
2. Verify filter settings in the INI file
3. Ensure the filter text matches window properties
4. Check exclusion/inclusion lists

### Too Many Windows Hidden
1. Clear all filters (Filters dialog → Clear Filters)
2. Check inclusion lists aren't too restrictive
3. Verify exclusion lists aren't too broad

### Performance Issues
1. Reduce filter complexity
2. Limit exclusion/inclusion lists
3. Disable filtering for heavy workloads

## Advanced Usage

### Regular Expressions
Enable regex support for complex pattern matching:
```ini
[Filters]
UseRegex=1
```

### Case-Sensitive Search
For exact text matching:
```ini
[Filters]
CaseSensitive=1
```

### Process-Specific Filtering
Create different filter profiles for different use cases by modifying the INI file.

## Integration with Existing Features

- **Dynamic Order**: Filters work with both dynamic and persistent ordering
- **Window Pinning**: Pinned windows respect filter settings
- **Grid Layout**: Filtered windows maintain grid positioning
- **Hotkeys**: All existing shortcuts work with filtered windows

## Future Enhancements

Planned improvements include:
- Filter presets/profiles
- Advanced regex support
- Filter history
- Drag-and-drop filter management
- Filter import/export
