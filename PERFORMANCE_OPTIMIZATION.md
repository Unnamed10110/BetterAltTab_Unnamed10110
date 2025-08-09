# BetterAltTab Performance Optimization Guide

## Overview

BetterAltTab has been enhanced with comprehensive performance optimizations to ensure smooth operation even under low resource conditions. The application now automatically detects system resource levels and adjusts its behavior accordingly.

## Key Performance Features

### 1. **Automatic Resource Detection**
- **Memory Monitoring**: Continuously monitors available system memory
- **CPU Usage Tracking**: Monitors CPU utilization to detect high load
- **Dynamic Mode Switching**: Automatically switches between normal and low-resource modes

### 2. **Memory Management**
- **Thumbnail Caching**: Intelligent caching system for window thumbnails
- **Memory Reservation**: Reserves critical memory for essential operations
- **Garbage Collection**: Automatic cleanup of unused resources

### 3. **Process Priority Management**
- **Dynamic Priority**: Adjusts process priority based on resource conditions
- **Thread Optimization**: Optimizes UI thread priority for responsiveness
- **Resource Reservation**: Ensures the application gets adequate CPU time

### 4. **Visual Performance**
- **Reduced Refresh Rates**: Lower refresh rates in low-resource mode
- **Optimized Rendering**: Efficient drawing with reduced visual effects
- **Thumbnail Size Adjustment**: Smaller thumbnails when resources are limited

## Configuration Options

### Performance Settings in `BetterAltTab.ini`

```ini
[Performance]
; Low resource mode: auto/on/off
LowResourceMode=auto

; Maximum thumbnail cache size
MaxThumbnailCache=20

; Refresh rates (milliseconds)
NormalRefreshRate=50
LowResourceRefreshRate=100

; Minimum thumbnail size in low resource mode
MinThumbnailSize=200

; Memory threshold for low resource mode (MB)
MemoryThreshold=512

; CPU threshold for low resource mode (%)
CPUThreshold=80

; Reserve memory for critical operations
ReserveMemory=true

; Optimize process priority
OptimizePriority=true

; Reduce visual effects in low resource mode
ReduceVisualEffects=true
```

### Configuration Options Explained

#### `LowResourceMode`
- **`auto`**: Automatically detect and switch modes (recommended)
- **`on`**: Force low resource mode
- **`off`**: Disable low resource mode

#### `MaxThumbnailCache`
- Controls how many thumbnails to keep in memory
- Lower values use less memory but may cause more thumbnail regeneration
- Recommended: 15-25 for most systems

#### `MemoryThreshold`
- Memory level (in MB) that triggers low resource mode
- Lower values make the app more aggressive about resource conservation
- Recommended: 512-1024 MB

#### `CPUThreshold`
- CPU usage percentage that triggers low resource mode
- Lower values make the app more sensitive to CPU load
- Recommended: 70-85%

## Performance Modes

### Normal Mode
- Full visual effects and animations
- Higher refresh rates for smooth interaction
- Larger thumbnail sizes
- Standard process priority

### Low Resource Mode
- Reduced visual effects
- Lower refresh rates to conserve CPU
- Smaller thumbnail sizes
- Higher process priority to ensure responsiveness
- Aggressive memory management

## Manual Resource Management

### Windows Task Manager Integration
1. Right-click the BetterAltTab system tray icon
2. Select "Settings" to access performance options
3. Adjust settings based on your system's capabilities

### System Resource Monitoring
The application automatically monitors:
- Available physical memory
- CPU utilization
- System responsiveness
- Thumbnail generation performance

## Troubleshooting Performance Issues

### If the application feels sluggish:
1. Check if you're in low resource mode (red border around overlay)
2. Reduce `MaxThumbnailCache` in the configuration
3. Increase `MemoryThreshold` if you have sufficient RAM
4. Set `LowResourceMode=on` to force performance mode

### If thumbnails are slow to load:
1. Increase `MaxThumbnailCache` if you have sufficient memory
2. Reduce `LowResourceRefreshRate` for more frequent updates
3. Check if other applications are consuming system resources

### If the application uses too much memory:
1. Reduce `MaxThumbnailCache`
2. Set `LowResourceMode=on`
3. Close unnecessary applications to free up system memory

## Advanced Performance Tips

### For Systems with Limited RAM (< 4GB):
```ini
[Performance]
LowResourceMode=on
MaxThumbnailCache=10
MemoryThreshold=256
CPUThreshold=60
```

### For High-Performance Systems (> 8GB RAM):
```ini
[Performance]
LowResourceMode=off
MaxThumbnailCache=30
MemoryThreshold=1024
CPUThreshold=90
```

### For Gaming Systems:
```ini
[Performance]
LowResourceMode=auto
MaxThumbnailCache=15
MemoryThreshold=512
CPUThreshold=70
ReduceVisualEffects=true
```

## Monitoring Performance

The application provides several indicators of its performance state:

1. **Visual Indicators**:
   - Red border around overlay = Low resource mode active
   - Smooth animations = Normal mode
   - Reduced animations = Low resource mode

2. **System Tray**:
   - Right-click for performance status
   - Settings dialog shows current resource usage

3. **Configuration File**:
   - Settings are automatically saved
   - Changes take effect on next application restart

## Technical Details

### Memory Management
- Uses Windows Virtual Memory API for efficient memory allocation
- Implements reference counting for thumbnail objects
- Automatic cleanup of unused resources

### CPU Optimization
- Thread priority management for UI responsiveness
- Efficient painting algorithms with frame rate limiting
- Background resource monitoring with minimal impact

### Thumbnail Optimization
- Lazy loading of thumbnails (only when visible)
- Caching system to avoid regeneration
- Size adjustment based on available resources

## Support

If you experience performance issues:
1. Check the configuration settings
2. Monitor system resource usage
3. Try different performance mode settings
4. Report issues with system specifications

The application is designed to work efficiently on systems ranging from low-end laptops to high-performance workstations.
