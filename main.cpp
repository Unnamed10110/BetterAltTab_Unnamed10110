// Activamos Unicode para que los textos se vean bien en todos lados, así no salen caracteres raros.
#define UNICODE
#define _UNICODE

// Acá traemos todo lo que necesitamos de Windows y C++.
// Sin esto, no podés hacer nada en Windows.
#include <windows.h>      // Todo lo básico de ventanas, mensajes, etc.
// Esto es para las miniaturas y efectos visuales de las ventanas (DWM = Desktop Window Manager).
#include <dwmapi.h>
// Macros útiles para trabajar con ventanas, te ahorran escribir código repetido.
#include <windowsx.h>
// El famoso vector de C++, es como una lista que crece sola.
#include <vector>
// Para usar strings de C++ (¡no más char* feos!).
#include <string>
// Un diccionario rápido, para buscar cosas por clave.
#include <unordered_map>
// Para leer y escribir archivos fácil.
#include <fstream>
// Controles comunes de Windows (botones, listas, etc).
#include <commctrl.h>
// Para que la app se vea más linda, con temas modernos.
#include <uxtheme.h>
// Otro tipo de diccionario, pero ordenado.
#include <map>
// Esto es para manejar escritorios virtuales (Windows 10+).
#include <shobjidl.h>
// Cosas de usuario de Windows, como teclas y ventanas.
#include <winuser.h>
// Para sacar info de procesos (como el nombre del exe de una ventana).
#include <psapi.h>
// Para manejar strings de forma más segura (no te pasás de largo).
#include <strsafe.h>
// Para que no se mezclen los hilos cuando escribimos logs.
#include <mutex>
// Funciones útiles como sort, min, max, etc.
#include <algorithm>
// Para transformaciones de strings
#include <cctype>
// Para cosas de la bandeja del sistema y ejecutar programas.
#include <shellapi.h>
// Utilidades varias de Windows.
#include <shlwapi.h>
// Para inicializar COM, que es como el pegamento de Windows.
#include <objbase.h>
// Estos pragmas le dicen al compilador: "che, agregá tal librería cuando compiles".
#pragma comment(lib, "Shlwapi.lib")      // Utilidades de shell
#pragma comment(lib, "dwmapi.lib")       // Miniaturas y efectos visuales
#pragma comment(lib, "comctl32.lib")     // Controles comunes
#pragma comment(lib, "uxtheme.lib")      // Temas visuales
#pragma comment(lib, "ole32.lib")        // COM
#pragma comment(lib, "psapi.lib")        // Info de procesos
#pragma comment(lib, "msimg32.lib")      // AlphaBlend (transparencias)
#pragma comment(lib, "msimg32.lib")      // De nuevo, por si acaso (a veces hace falta)

// ===============================
// Definiciones y constantes globales
// ===============================

// Acá van los defines, que son como etiquetas o valores fijos que usamos en todo el código.
// HOTKEY_ID es el identificador para el atajo de teclado principal (Ctrl+Numpad).
#define HOTKEY_ID 0xBEEF      // Esto es solo un número raro para que no choque con otros IDs.
#define HOTKEY_ID_ALTQ 0xBEEE // Otro ID para el atajo Alt+Q.
#define HOTKEY_ID_ALTA 0xBEED // ID para el atajo Alt+A (filtro persistente).
#define GRID_ORDER_FILE L"grid_order.bin" // Archivo donde guardamos el orden de las ventanas.

#define ID_TRAY_SETTINGS 2004
#define ID_TRAY_FILTERS 2006
#define ID_TRAY_CLEANUP 2005
#define ID_TRAY_TOGGLE_ALTA 2007
#define ID_TRAY_TOGGLE_DYNAMIC_MODE 2008
#define ID_TRAY_FULL_PERFORMANCE 2009
#define ID_TRAY_FULL_LOW_RESOURCES 2010
#define IDC_COLUMNS_LABEL 3001
#define IDC_COLUMNS_EDIT 3002
#define IDC_APPLY_BUTTON 3003
#define IDC_OVERLAY_ALPHA_LABEL 3004
#define IDC_OVERLAY_ALPHA_EDIT 3005
#define IDC_FIXED_COLS_LABEL 3006
#define IDC_FIXED_COLS_EDIT 3007
#define IDC_DYNAMIC_ORDER_CHECK 3008
#define IDC_SAVE_WINDOW_ORDER_CHECK 3009
#define IDC_SHOW_CLOSED_WINDOWS_CHECK 3010
#define IDC_CLOSED_WINDOW_DELAY_LABEL 3011
#define IDC_CLOSED_WINDOW_DELAY_EDIT 3012
#define IDC_AUTO_HIDE_OVERLAY_CHECK 3013
#define IDC_SHOW_FILTER_INDICATOR_CHECK 3014
#define IDC_ENABLE_SOUNDS_CHECK 3015
#define IDC_LOW_RESOURCE_MODE_LABEL 3016
#define IDC_LOW_RESOURCE_MODE_COMBO 3017
#define IDC_MAX_THUMBNAIL_CACHE_LABEL 3018
#define IDC_MAX_THUMBNAIL_CACHE_EDIT 3019
#define IDC_NORMAL_REFRESH_RATE_LABEL 3020
#define IDC_NORMAL_REFRESH_RATE_EDIT 3021
#define IDC_AUTO_CLEANUP_INTERVAL_LABEL 3022
#define IDC_AUTO_CLEANUP_INTERVAL_EDIT 3023
#define IDC_MAX_THUMBNAIL_MEMORY_LABEL 3024
#define IDC_MAX_THUMBNAIL_MEMORY_EDIT 3025
#define IDC_WINDOW_CACHE_TIMEOUT_LABEL 3026
#define IDC_WINDOW_CACHE_TIMEOUT_EDIT 3027
#define IDC_SHOW_TRAY_ICON_CHECK 3028
#define IDC_SHOW_SYSTEM_INFO_CHECK 3029
#define IDC_SHOW_WINDOW_COUNT_CHECK 3030
#define IDC_SHOW_MEMORY_USAGE_CHECK 3031
#define IDC_SHOW_PERFORMANCE_MODE_CHECK 3032
#define IDC_TRAY_ICON_SIZE_LABEL 3033
#define IDC_TRAY_ICON_SIZE_COMBO 3034
#define IDC_TRAY_ICON_COLOR_LABEL 3035
#define IDC_TRAY_ICON_COLOR_EDIT 3036
#define IDC_ALTQ_KEY_LABEL 3037
#define IDC_ALTQ_KEY_EDIT 3038
#define IDC_ALTA_KEY_LABEL 3039
#define IDC_ALTA_KEY_EDIT 3040
#define IDC_NUMPAD_MODIFIER_LABEL 3041
#define IDC_NUMPAD_MODIFIER_COMBO 3042
#define IDC_DEBUG_MODE_CHECK 3043
#define IDC_EVENT_LOGGING_CHECK 3044
#define IDC_LOG_LEVEL_LABEL 3045
#define IDC_LOG_LEVEL_COMBO 3046
#define IDC_CANCEL_BUTTON 3047

// Acá configuramos cómo se ve la interfaz: márgenes, tamaños, colores, etc.
const int GRID_COLS = 4; // Cuántas columnas tiene la grilla (no se usa, ver FIXED_COLS más abajo).
const int GRID_PADDING_X = 48; // Espacio a los costados de la grilla.
const int GRID_PADDING_Y = 48; // Espacio arriba y abajo de la grilla.
const int OVERLAY_WIDTH = 1692; // Ancho de la superposición (el overlay), casi toda la pantalla.
const int OVERLAY_HEIGHT = 1000; // Alto del overlay.
const int PREVIEW_MARGIN = 32; // Espacio entre miniaturas.
// ---------- Dynamic thumbnail sizing support ----------
const int BASE_PREVIEW_WIDTH = 299;
const int BASE_PREVIEW_HEIGHT = 207;
#ifndef PREVIEW_WIDTH
#define PREVIEW_WIDTH BASE_PREVIEW_WIDTH
#define PREVIEW_HEIGHT BASE_PREVIEW_HEIGHT
#endif
extern int g_previewW;
extern int g_previewH;
void UpdatePreviewSize();
const COLORREF OVERLAY_BG_COLOR = RGB(0, 0, 0); // Fondo negro, bien oscuro.
const BYTE OVERLAY_BG_ALPHA = 255; // Transparencia del overlay (255 = completamente opaco, sin transparencia).
const COLORREF BORDER_COLOR = RGB(32, 32, 48); // Color del borde de las miniaturas.
const COLORREF HOVER_BORDER_COLOR = RGB(80, 180, 255); // Borde azul cuando pasás el mouse.
const COLORREF DRAG_BORDER_COLOR = RGB(180, 140, 255); // Borde violeta cuando arrastrás.
const COLORREF TITLE_COLOR = RGB(255,255,255); // Color del texto (blanco).
const COLORREF HIGHLIGHT_COLOR = RGB(0, 255, 68); // Verde para resaltar.
const int ICON_SIZE = 27; // Tamaño de los íconos de las ventanas.
const int SCROLLBAR_HEIGHT = 12; // Alto de la barra de scroll (invisible, pero existe).
const int FIXED_COLS = 5; // Siempre mostramos 5 columnas, así queda prolijo.
const int PIN_CLOSE_VERTICAL_GAP = 12; // Espacio entre el pin y el botón de cerrar.
const int PIN_TO_POS_BUTTON_SIZE = 26; // Tamaño del botón para fijar a una posición.

// Añadimos variables configurables cargadas desde INI
int g_fixedCols = 5;               // columnas visibles
BYTE g_overlayAlpha = 230;         // opacidad del overlay

void CreateDefaultConfiguration()
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    wcscat_s(exePath, L"\\BetterAltTab.ini");

    // Verificar si el archivo ya existe
    if (GetFileAttributesW(exePath) != INVALID_FILE_ATTRIBUTES) {
        return; // El archivo ya existe, no crear uno nuevo
    }

    // Crear configuración por defecto
    WritePrivateProfileStringW(L"General", L"Columns", L"5", exePath);
    WritePrivateProfileStringW(L"General", L"OverlayAlpha", L"230", exePath);
    WritePrivateProfileStringW(L"General", L"AltAEnabled", L"1", exePath);
    WritePrivateProfileStringW(L"General", L"DynamicOrder", L"1", exePath);
    
    // Configuración de rendimiento
    WritePrivateProfileStringW(L"Performance", L"AutoCleanupInterval", L"30000", exePath);
    WritePrivateProfileStringW(L"Performance", L"MaxThumbnailMemory", L"100", exePath);
    WritePrivateProfileStringW(L"Performance", L"WindowCacheTimeout", L"5000", exePath);
    WritePrivateProfileStringW(L"Performance", L"NormalRefreshRate", L"50", exePath);
    WritePrivateProfileStringW(L"Performance", L"LowResourceMode", L"auto", exePath);
    WritePrivateProfileStringW(L"Performance", L"MaxThumbnailCache", L"50", exePath);
    WritePrivateProfileStringW(L"Performance", L"MinThumbnailSize", L"300", exePath);
    WritePrivateProfileStringW(L"Performance", L"MemoryThreshold", L"2048", exePath);
    WritePrivateProfileStringW(L"Performance", L"CPUThreshold", L"95", exePath);
    WritePrivateProfileStringW(L"Performance", L"ReserveMemory", L"true", exePath);
    WritePrivateProfileStringW(L"Performance", L"OptimizePriority", L"true", exePath);
    WritePrivateProfileStringW(L"Performance", L"ReduceVisualEffects", L"false", exePath);
    
    // Configuración de filtros
    WritePrivateProfileStringW(L"Filters", L"Enabled", L"0", exePath);
    WritePrivateProfileStringW(L"Filters", L"FilterByTitle", L"1", exePath);
    WritePrivateProfileStringW(L"Filters", L"FilterByClassName", L"1", exePath);
    WritePrivateProfileStringW(L"Filters", L"FilterByProcessName", L"1", exePath);
    WritePrivateProfileStringW(L"Filters", L"CaseSensitive", L"0", exePath);
    WritePrivateProfileStringW(L"Filters", L"UseRegex", L"0", exePath);
    WritePrivateProfileStringW(L"Filters", L"ExcludeHidden", L"1", exePath);
    WritePrivateProfileStringW(L"Filters", L"ExcludeMinimized", L"0", exePath);
    WritePrivateProfileStringW(L"Filters", L"ExcludedProcesses", L"", exePath);
    WritePrivateProfileStringW(L"Filters", L"ExcludedClasses", L"", exePath);
    WritePrivateProfileStringW(L"Filters", L"IncludedProcesses", L"", exePath);
    WritePrivateProfileStringW(L"Filters", L"IncludedClasses", L"", exePath);
    
    // Configuración de UI
    WritePrivateProfileStringW(L"UI", L"ShowTrayIcon", L"1", exePath);
    WritePrivateProfileStringW(L"UI", L"ShowSystemInfo", L"0", exePath);
    WritePrivateProfileStringW(L"UI", L"ShowWindowCount", L"0", exePath);
    WritePrivateProfileStringW(L"UI", L"ShowMemoryUsage", L"0", exePath);
    WritePrivateProfileStringW(L"UI", L"ShowPerformanceMode", L"0", exePath);
    WritePrivateProfileStringW(L"UI", L"TrayIconSize", L"32", exePath);
    WritePrivateProfileStringW(L"UI", L"TrayIconColor", L"#FFFFFF", exePath);
    
    // Configuración de hotkeys
    WritePrivateProfileStringW(L"Hotkeys", L"AltQKey", L"Q", exePath);
    WritePrivateProfileStringW(L"Hotkeys", L"AltAKey", L"A", exePath);
    WritePrivateProfileStringW(L"Hotkeys", L"NumpadModifier", L"VK_CONTROL", exePath);
    
    // Configuración avanzada
    WritePrivateProfileStringW(L"Advanced", L"DebugMode", L"0", exePath);
    WritePrivateProfileStringW(L"Advanced", L"EventLogging", L"0", exePath);
    WritePrivateProfileStringW(L"Advanced", L"LogLevel", L"info", exePath);
    WritePrivateProfileStringW(L"Advanced", L"LogToFile", L"0", exePath);
    WritePrivateProfileStringW(L"Advanced", L"LogToConsole", L"0", exePath);
}

void LoadConfiguration()
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    wcscat_s(exePath, L"\\BetterAltTab.ini");

    // Crear configuración por defecto si no existe
    CreateDefaultConfiguration();

    // Sección general
    g_fixedCols = GetPrivateProfileIntW(L"General", L"Columns", g_fixedCols, exePath);
    int alpha   = GetPrivateProfileIntW(L"General", L"OverlayAlpha", g_overlayAlpha, exePath);
    g_overlayAlpha = (BYTE)max(0, min(alpha, 255));
}

// ===============================
// Variables globales para el manejo de estado
// ===============================

// Acá están todas las variables que usamos para saber en qué parte de la grilla estamos, si estamos scrolleando, cuál ventana está seleccionada, etc.
static int g_scrollX = 0; // Cuánto nos movimos a la derecha en la grilla.
static int g_scrollMax = 0; // Hasta dónde se puede scrollear a la derecha.
static bool g_scrolling = false; // Si estamos scrolleando con el mouse.
static int g_scrollStartX = 0; // Dónde empezó el scroll (X).
static int g_scrollStartOffset = 0; // Cuánto llevábamos scrolleado cuando empezamos.
static int g_scrollY = 0; // Cuánto nos movimos para abajo.
static int g_scrollMaxY = 0; // Hasta dónde se puede scrollear para abajo.
static bool g_vscrolling = false; // Si estamos scrolleando vertical.
static int g_scrollStartY = 0; // Dónde empezó el scroll (Y).
static int g_scrollStartOffsetY = 0; // Cuánto llevábamos scrolleado cuando empezamos (Y).
static int g_selectedIndex = 0; // Qué ventana está seleccionada ahora.
// Estado de arrastrar y soltar (no usado actualmente, pero preparado para futuras mejoras).
static bool g_dragging = false; // Si estamos arrastrando una miniatura (todavía no se usa, pero está listo).
static int g_dragIndex = -1; // Qué ventana estamos arrastrando.
static int g_hoverIndex = -1; // Sobre qué ventana está el mouse.
static int g_dragMouseX = 0; // Dónde está el mouse cuando arrastramos.
static int g_dragMouseY = 0; // Dónde está el mouse cuando arrastramos (Y).
// Variables globales para el hover de los íconos.
static int g_pinHoverIndex = -1; // Sobre qué botón de pin está el mouse.
static int g_closeHoverIndex = -1; // Sobre qué botón de cerrar está el mouse.
static int g_pinToPosHoverIndex = -1; // Sobre qué botón de pin a posición está el mouse.
static int g_lastHotkey = 0; // Último atajo usado: 0 = ninguno, 1 = ctrl+numpad, 2 = alt+q.



// ===============================
// Superposición extra para entrada Ctrl+número
// ===============================

// Estas variables manejan el overlay para escribir números cuando apretás Ctrl.
static bool g_ctrlNumberInputActive = false; // Si está activo el overlay de números.
static std::wstring g_ctrlNumberBuffer; // Acá guardamos el número que escribís.
static HWND g_ctrlNumberOverlayHwnd = nullptr; // La ventana del overlay de números.

// Declaraciones anticipadas para la superposición de entrada Ctrl+número.
LRESULT CALLBACK CtrlNumberOverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ShowCtrlNumberOverlay(HWND parent);
void HideCtrlNumberOverlay();

// Tamaños del overlay principal (se pueden cambiar dinámicamente).
int g_overlayWidth = OVERLAY_WIDTH; // Ancho del overlay principal.
int g_overlayHeight = OVERLAY_HEIGHT; // Alto del overlay principal.

// ===============================
// Estructuras para representar ventanas y persistencia
// ===============================

// Esta estructura guarda toda la info de una ventana: su handle, título, clase, miniatura, etc.
// Es como una ficha de cada ventana que tenemos abierta.
struct WindowInfo {
    HWND hwnd; // Handle de la ventana (es como el DNI de la ventana en Windows).
    std::wstring title; // Título de la ventana (lo que aparece en la barra de título).
    std::wstring className; // Nombre de la clase de la ventana (tipo de ventana).
    HTHUMBNAIL thumbnail = nullptr; // Miniatura DWM (la imagen chiquita de la ventana).
    bool pinned = false; // Si está fijada en la grilla (no se mueve cuando cambias el orden).
};

// Esta estructura es para guardar el orden de las ventanas en un archivo.
// Así cuando reiniciás la compu, las ventanas quedan en el mismo lugar que las dejaste.
struct PersistedWindow {
    HWND hwnd; // Handle de la ventana.
    wchar_t title[256]; // Título (máximo 256 caracteres).
    wchar_t className[128]; // Clase (máximo 128 caracteres).
    bool pinned = false; // Si estaba fijada.
};

// ===============================
// Vectores y mapas globales
// ===============================

// Acá están todos los contenedores que usamos para manejar las ventanas y sus miniaturas.
// Son como cajas donde guardamos toda la info que necesitamos.
std::vector<WindowInfo> g_windows; // Lista de todas las ventanas que tenemos abiertas.
std::map<HWND, HTHUMBNAIL> g_thumbnailMap; // Diccionario que relaciona cada ventana con su miniatura.
std::vector<PersistedWindow> g_gridOrder; // Orden guardado de las ventanas (para cuando reiniciás).
HWND g_tooltip = nullptr; // Ventana del tooltip (cuando pasás el mouse sobre algo).
IVirtualDesktopManager* g_vdm = nullptr; // Para manejar escritorios virtuales.
bool g_dynamicOrder = false; // Si el orden cambia dinámicamente o no.

// ===============================
// Variables de filtrado
// ===============================
static bool g_filteringEnabled = false; // Si el filtrado está habilitado
static std::wstring g_filterText; // Texto del filtro actual
static std::vector<WindowInfo> g_filteredWindows; // Ventanas filtradas
static std::vector<WindowInfo> g_allWindows; // Todas las ventanas (sin filtrar)
static bool g_filterByTitle = true; // Filtrar por título
static bool g_filterByClassName = true; // Filtrar por nombre de clase
static bool g_filterByProcessName = true; // Filtrar por nombre del proceso
static bool g_filterCaseSensitive = false; // Si el filtro es sensible a mayúsculas
static bool g_filterRegex = false; // Si usar expresiones regulares
static bool g_filterExcludeHidden = true; // Excluir ventanas ocultas
static bool g_filterExcludeMinimized = false; // Excluir ventanas minimizadas
static std::vector<std::wstring> g_excludedProcesses; // Procesos excluidos
static std::vector<std::wstring> g_excludedClasses; // Clases de ventana excluidas
static std::vector<std::wstring> g_includedProcesses; // Procesos incluidos (whitelist)
static std::vector<std::wstring> g_includedClasses; // Clases de ventana incluidas (whitelist)
static bool g_quickFilterActive = false; // Si el filtro rápido está activo
static HWND g_quickFilterEdit = nullptr; // Control de entrada del filtro rápido
static bool g_persistentFilterMode = false; // Si el overlay está en modo filtro persistente (Alt+A)
static bool g_placeholderActive = false; // Si el placeholder está activo

// ===============================
// Variables de optimización de rendimiento
// ===============================
DWORD g_lastCleanupTime = 0; // Última vez que se limpió el cache
DWORD g_lastWindowEnumTime = 0; // Última vez que se enumeraron las ventanas
DWORD g_lastRedrawTime = 0; // Última vez que se redibujó
ULONGLONG g_thumbnailMemoryUsage = 0; // Uso de memoria de miniaturas en bytes
bool g_performanceMode = false; // Modo de rendimiento optimizado
bool g_lowResourceMode = false; // Modo de bajo recursos activo
int g_cleanupInterval = 30000; // Intervalo de limpieza automática (ms)
int g_maxThumbnailMemory = 100 * 1024 * 1024; // Memoria máxima para miniaturas (100MB)
int g_windowCacheTimeout = 5000; // Timeout del cache de ventanas (ms)
int g_redrawThrottle = 50; // Throttle para redibujado (ms)

// ===============================
// Variables para detección de ventanas cerradas
// ===============================
HHOOK g_windowHook = nullptr; // Hook para detectar cuando se cierran ventanas
bool g_windowClosed = false; // Flag para indicar que una ventana se cerró

// ===============================
// Funciones principales
// ===============================

// Acá están las declaraciones de todas las funciones que usamos.
// Es como un índice de un libro: le decimos al compilador "che, estas funciones existen".
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // La función principal que maneja todos los mensajes de la ventana.
std::vector<WindowInfo> EnumerateWindows(HWND excludeHwnd); // Busca todas las ventanas abiertas.
void RegisterThumbnails(HWND host, std::vector<WindowInfo>& windows); // Crea las miniaturas de las ventanas.
void UnregisterThumbnails(std::vector<WindowInfo>& windows); // Borra las miniaturas cuando no las necesitamos.
void SaveGridOrder(const std::vector<WindowInfo>& windows); // Guarda el orden de las ventanas en un archivo.
void LoadGridOrder(std::vector<PersistedWindow>& order); // Carga el orden guardado del archivo.
void ApplyGridOrder(std::vector<WindowInfo>& windows, const std::vector<PersistedWindow>& order); // Aplica el orden guardado.
void CreateDefaultConfiguration(); // Crea el archivo de configuración por defecto.
void LoadConfiguration(); // Carga la configuración desde el archivo INI.

// Forward declarations for filter functions
void LoadFilterSettings(); // Carga configuración de filtros desde INI
void SaveFilterSettings(); // Guarda configuración de filtros en INI
void ApplyFilters(); // Aplica los filtros actuales a las ventanas
bool WindowMatchesFilter(const WindowInfo& window); // Verifica si una ventana coincide con el filtro
std::wstring GetProcessName(HWND hwnd); // Obtiene el nombre del proceso de una ventana
void ToggleFiltering(); // Alterna el estado del filtrado
void ClearFilters(); // Limpia todos los filtros
void SetFilterText(const std::wstring& text); // Establece el texto del filtro
void AddExcludedProcess(const std::wstring& processName); // Agrega un proceso a la lista de excluidos
void RemoveExcludedProcess(const std::wstring& processName); // Remueve un proceso de la lista de excluidos
void AddExcludedClass(const std::wstring& className); // Agrega una clase a la lista de excluidos
void RemoveExcludedClass(const std::wstring& className); // Remueve una clase de la lista de excluidos
void AddIncludedProcess(const std::wstring& processName); // Agrega un proceso a la lista de incluidos
void RemoveIncludedProcess(const std::wstring& processName); // Remueve un proceso de la lista de incluidos
void AddIncludedClass(const std::wstring& className); // Agrega una clase a la lista de incluidos
void RemoveIncludedClass(const std::wstring& className); // Remueve una clase de la lista de incluidos
void ShowFilterDialog(HWND parent); // Muestra el diálogo de configuración de filtros
void ClearSearchBox(); // Limpia la caja de búsqueda
void DrawWindowIcon(HDC hdc, HWND hwnd, int x, int y, int size); // Dibuja el ícono de una ventana.
void DrawTextWithShadow(HDC hdc, LPCWSTR text, RECT* rc, COLORREF color, int glowSize); // Dibuja texto con sombra.
void CenterOverlayWindow(HWND hwnd, int width, int height); // Centra la ventana del overlay en la pantalla.
void InvalidateGrid(HWND hwnd); // Le dice a Windows que redibuje la grilla.
void UnregisterAllThumbnails(); // Borra todas las miniaturas de una vez.
void ShowSettingsDialog(HWND parent); // <-- Forward declaration
void ShowFilterDialog(HWND parent); // <-- Forward declaration
void ShowTabControls(HWND hwnd, int tabIndex);
void SaveAllSettings(HWND hwnd);
void ApplyDarkThemeToControls(HWND hwnd, HFONT hTitleFont, HFONT hLabelFont, HFONT hControlFont);
void ConfigureFullPerformance(); // Configura para máximo rendimiento
void ConfigureFullLowResources(); // Configura para mínimo consumo



// ===============================
// Funciones de optimización de rendimiento
// ===============================
void LoadPerformanceSettings(); // Carga configuración de rendimiento desde INI
void CheckPerformanceMode(); // Verifica y ajusta el modo de rendimiento
void CleanupThumbnailCache(); // Limpia cache de miniaturas automáticamente
void MonitorMemoryUsage(); // Monitorea el uso de memoria
bool ShouldRedraw(); // Determina si se debe redibujar basado en throttling
void OptimizeProcessPriority(); // Optimiza la prioridad del proceso
void ResetPerformanceCounters(); // Resetea contadores de rendimiento

// ===============================
// Funciones para detección de ventanas cerradas
// ===============================
LRESULT CALLBACK WindowHookProc(int nCode, WPARAM wParam, LPARAM lParam); // Hook para detectar cierre de ventanas
void InstallWindowHook(); // Instala el hook para detectar ventanas cerradas
void UninstallWindowHook(); // Desinstala el hook

// Función SetWindowsZOrder removida - no se utilizaba

// Esta función es como un filtro: Windows la llama para cada ventana que encuentra.
// Nosotros decidimos cuáles queremos mostrar en nuestra grilla.
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    HWND excludeHwnd = (HWND)lParam; // La ventana que no queremos mostrar (nuestro overlay).
    if (hwnd == excludeHwnd) return TRUE; // Si es nuestro overlay, la saltamos.
    if (!IsWindowVisible(hwnd)) return TRUE; // Si no se ve, la saltamos.
    
    // Solo mostrar ventanas del escritorio virtual actual (Windows 10+).
    if (g_vdm) {
        BOOL onCurrent = FALSE;
        if (FAILED(g_vdm->IsWindowOnCurrentVirtualDesktop(hwnd, &onCurrent)) || !onCurrent)
            return TRUE; // Si no está en este escritorio, la saltamos.
    }
    
    // Obtener información de la ventana
    wchar_t title[256];
    GetWindowTextW(hwnd, title, 256);
    wchar_t className[128];
    GetClassNameW(hwnd, className, 128);
    
    // Si no tiene título, la saltamos
    if (wcslen(title) == 0) return TRUE;
    
    // Obtener estilos de la ventana
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    HWND owner = GetWindow(hwnd, GW_OWNER);
    
    // Criterios más inclusivos para capturar más aplicaciones
    bool shouldInclude = false;
    
    // Incluir si es una ventana de aplicación principal
    if (exStyle & WS_EX_APPWINDOW) {
        shouldInclude = true;
    }
    // Incluir si es una ventana principal sin dueño
    else if (!owner && (style & WS_OVERLAPPEDWINDOW)) {
        shouldInclude = true;
    }
    // Incluir si tiene un dueño pero es una ventana principal
    else if (owner && (style & WS_OVERLAPPEDWINDOW)) {
        shouldInclude = true;
    }
    // Incluir si es una ventana de diálogo modal
    else if (style & WS_DLGFRAME) {
        shouldInclude = true;
    }
    // Incluir si es una ventana de aplicación con título significativo
    else if (wcslen(title) > 0 && !(exStyle & WS_EX_TOOLWINDOW)) {
        // Verificar si parece ser una aplicación principal por su título
        if (wcsstr(title, L" - ") || wcsstr(title, L" | ") || wcsstr(title, L" — ")) {
            shouldInclude = true;
        }
        // Incluir si el título es suficientemente largo (probablemente una aplicación)
        else if (wcslen(title) > 10) {
            shouldInclude = true;
        }
    }
    
    // Excluir solo tool windows muy específicas
    if (exStyle & WS_EX_TOOLWINDOW) {
        // Pero incluir algunas tool windows que pueden ser aplicaciones útiles
        if (wcsstr(className, L"ConsoleWindowClass") || 
            wcsstr(className, L"Console") ||
            wcsstr(className, L"Terminal")) {
            shouldInclude = true;
        } else {
            shouldInclude = false;
        }
    }
    
    if (shouldInclude) {
        g_allWindows.push_back({hwnd, title, className});
    }
    
    return TRUE; // Seguimos buscando más ventanas.
}

// Esta función busca todas las ventanas abiertas y las devuelve en una lista.
// También aplica el orden guardado si existe.
std::vector<WindowInfo> EnumerateWindows(HWND excludeHwnd) {
    DWORD currentTime = GetTickCount();
    
    // Optimización: cache de ventanas para evitar enumeración excesiva
    // Solo usar cache si estamos en modo persistente (Alt+A) y no ha pasado mucho tiempo
    if (currentTime - g_lastWindowEnumTime < g_windowCacheTimeout && !g_allWindows.empty() && g_persistentFilterMode) {
        // Solo usar cache si estamos en modo persistente (Alt+A)
        if (g_filteringEnabled) {
            ApplyFilters();
            return g_filteredWindows;
        } else {
            return g_allWindows;
        }
    }
    
    g_allWindows.clear(); // Limpiamos la lista anterior.
    EnumWindows(EnumWindowsProc, (LPARAM)excludeHwnd); // Buscamos todas las ventanas.
    
    // Siempre usar todas las ventanas como base
    g_windows = g_allWindows;
    
    // Aplicar filtros solo si estamos en modo persistente (Alt+A)
    if (g_filteringEnabled && g_persistentFilterMode) {
        ApplyFilters();
    }
    
    if (!g_dynamicOrder && !g_gridOrder.empty()) { // Si tenemos un orden guardado y no es dinámico.
        ApplyGridOrder(g_windows, g_gridOrder); // Aplicamos el orden guardado.
    }
    
    g_lastWindowEnumTime = currentTime; // Actualizar timestamp de enumeración
    return g_windows; // Devolvemos la lista de ventanas.
}

// Esta función crea las miniaturas (imágenes chiquitas) de todas las ventanas.
// Es como sacar una foto de cada ventana para mostrarla en nuestra grilla.
void RegisterThumbnails(HWND host, std::vector<WindowInfo>& windows) {
    for (auto& win : windows) { // Recorremos cada ventana.
        if (!win.thumbnail) { // Si no tiene miniatura todavía.
            // Verificar que la ventana existe y es válida antes de registrar
            if (!IsWindow(win.hwnd) || !IsWindowVisible(win.hwnd)) {
                continue; // Saltar ventanas inválidas o invisibles
            }
            
            // Intentar registrar con timeout implícito usando PeekMessage
            HTHUMBNAIL tempThumbnail = nullptr;
            HRESULT hr = DwmRegisterThumbnail(host, win.hwnd, &tempThumbnail);
            
            if (hr == S_OK && tempThumbnail) { // Creamos la miniatura.
                win.thumbnail = tempThumbnail;
                // Actualizar uso de memoria estimado
                g_thumbnailMemoryUsage += (g_previewW * g_previewH * 4); // 4 bytes por píxel (RGBA)
                g_thumbnailMap[win.hwnd] = win.thumbnail; // Guardar en el mapa global
            } else {
                // Log del error para debugging
                wchar_t debugMsg[256];
                wsprintfW(debugMsg, L"Failed to register thumbnail for window 0x%p, HRESULT: 0x%08X\n", win.hwnd, hr);
                OutputDebugStringW(debugMsg);
            }
        }
    }
}

// Esta función borra todas las miniaturas cuando no las necesitamos más.
// Es como limpiar la memoria para que no se llene.
void UnregisterThumbnails(std::vector<WindowInfo>& windows) {
    for (auto& win : windows) { // Recorremos cada ventana.
        if (win.thumbnail) { // Si tiene miniatura.
            // Verificar que la miniatura es válida antes de desregistrar
            HRESULT hr = DwmUnregisterThumbnail(win.thumbnail);
            if (FAILED(hr)) {
                // Log del error para debugging
                wchar_t debugMsg[256];
                wsprintfW(debugMsg, L"Failed to unregister thumbnail 0x%p, HRESULT: 0x%08X\n", win.thumbnail, hr);
                OutputDebugStringW(debugMsg);
            }
            // Actualizar uso de memoria estimado
            g_thumbnailMemoryUsage -= (g_previewW * g_previewH * 4); // 4 bytes por píxel (RGBA)
            win.thumbnail = nullptr; // La marcamos como que no existe.
        }
    }
    g_thumbnailMap.clear(); // Limpiamos el mapa de miniaturas también.
}

// Esta función guarda el orden de las ventanas en un archivo.
// Así cuando reiniciás la compu, las ventanas quedan en el mismo lugar.
void SaveGridOrder(const std::vector<WindowInfo>& windows) {
    std::ofstream ofs(GRID_ORDER_FILE, std::ios::binary); // Abrimos el archivo para escribir.
    if (!ofs) return; // Si no se pudo abrir, nos vamos.
    
    for (const auto& win : windows) { // Recorremos cada ventana.
        PersistedWindow pw; // Creamos una estructura para guardar.
        pw.hwnd = win.hwnd; // Guardamos el handle.
        wcsncpy_s(pw.title, win.title.c_str(), 255); // Guardamos el título (máximo 255 caracteres).
        wcsncpy_s(pw.className, win.className.c_str(), 127); // Guardamos la clase (máximo 127 caracteres).
        pw.pinned = win.pinned; // Guardamos si estaba fijada.
        ofs.write(reinterpret_cast<const char*>(&pw), sizeof(PersistedWindow)); // Escribimos al archivo.
    }
}

// Esta función lee el orden guardado de las ventanas desde el archivo.
// Es como cargar una partida guardada en un juego.
void LoadGridOrder(std::vector<PersistedWindow>& order) {
    order.clear(); // Limpiamos la lista anterior.
    std::ifstream ifs(GRID_ORDER_FILE, std::ios::binary); // Abrimos el archivo para leer.
    if (!ifs) return; // Si no se pudo abrir, nos vamos.
    
    PersistedWindow pw; // Variable para leer cada ventana.
    while (ifs.read(reinterpret_cast<char*>(&pw), sizeof(PersistedWindow))) { // Leemos mientras haya datos.
        order.push_back(pw); // Agregamos la ventana a la lista.
    }
}

// Esta función ordena las ventanas según el orden guardado.
// Es como arreglar las cartas en el mismo orden que las tenías antes.
// Esta función aplica el orden guardado a la lista actual de ventanas.
// Es como reorganizar las cartas según un patrón que tenías guardado.
void ApplyGridOrder(std::vector<WindowInfo>& windows, const std::vector<PersistedWindow>& order) {
    std::vector<WindowInfo> sorted; // Lista ordenada que vamos a crear.
    std::vector<bool> used(windows.size(), false); // Marcamos cuáles ventanas ya usamos.
    
    for (const auto& pw : order) { // Recorremos el orden guardado.
        for (size_t i = 0; i < windows.size(); ++i) { // Buscamos la ventana en la lista actual.
            if (!used[i] && windows[i].hwnd == pw.hwnd) { // Si encontramos la ventana y no la usamos.
                windows[i].pinned = pw.pinned; // Restauramos si estaba fijada.
                sorted.push_back(windows[i]); // La agregamos a la lista ordenada.
                used[i] = true; // La marcamos como usada.
                break; // Dejamos de buscar esta ventana.
            }
        }
    }
    
    for (size_t i = 0; i < windows.size(); ++i) { // Agregamos las ventanas que no estaban guardadas.
        if (!used[i]) sorted.push_back(windows[i]); // Las ponemos al final.
    }
    
    windows = std::move(sorted); // Reemplazamos la lista original con la ordenada.
}

// Esta función dibuja el ícono de una ventana.
// Es como sacar la foto del programa que está corriendo en esa ventana.
void DrawWindowIcon(HDC hdc, HWND hwnd, int x, int y, int size) {
    HICON hIcon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0); // Intentamos sacar el ícono chico.
    if (!hIcon) hIcon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_BIG, 0); // Si no hay, probamos el grande.
    if (!hIcon) hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM); // Si no hay, probamos el de la clase chico.
    if (!hIcon) hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICON); // Si no hay, probamos el de la clase grande.
    bool extracted = false; // Marcamos si tuvimos que extraer el ícono del exe.
    if (!hIcon) { // Si todavía no tenemos ícono.
        DWORD pid = 0; // ID del proceso.
        GetWindowThreadProcessId(hwnd, &pid); // Sacamos el ID del proceso de la ventana.
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid); // Abrimos el proceso.
        if (hProcess) { // Si pudimos abrir el proceso.
            wchar_t exePath[MAX_PATH] = {0}; // Buffer para la ruta del exe.
            if (GetModuleFileNameExW(hProcess, nullptr, exePath, MAX_PATH)) { // Sacamos la ruta del exe.
                HICON hExtractedSmall = nullptr; // Variable para el ícono extraído.
                UINT count = ExtractIconExW(exePath, 0, nullptr, &hExtractedSmall, 1); // Extraemos el ícono del exe.
                if (count > 0 && hExtractedSmall) { // Si pudimos extraer el ícono.
                    hIcon = hExtractedSmall; // Lo usamos.
                    extracted = true; // Marcamos que lo extrajimos.
                }
            }
            CloseHandle(hProcess); // Cerramos el proceso.
        }
    }
    if (hIcon) { // Si tenemos un ícono.
        DrawIconEx(hdc, x, y, hIcon, size, size, 0, nullptr, DI_NORMAL); // Lo dibujamos.
        if (extracted) { // Si lo extrajimos del exe.
            DestroyIcon(hIcon); // Lo borramos para liberar memoria.
        }
    }
}

// Esta función dibuja texto con un efecto de resplandor (glow).
// Es como cuando escribís con marcador fluorescente: se ve bien hasta en fondos oscuros.
void DrawTextWithShadow(HDC hdc, LPCWSTR text, RECT* rc, COLORREF color, int glowSize) {
    // Versión simplificada sin efectos visuales para mejor rendimiento
    SetTextColor(hdc, color); // Ponemos el color del texto.
    SetBkMode(hdc, TRANSPARENT); // Fondo transparente.
    DrawTextW(hdc, text, -1, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS); // Dibujamos el texto normal.
}

// Esta función centra la ventana del overlay en la pantalla.
// Es como poner un cuadro en el medio de la pared: queda perfecto en el centro.
void CenterOverlayWindow(HWND hwnd, int width, int height) {
    RECT rc; // Rectángulo para guardar el área de trabajo.
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0); // Sacamos el área de trabajo (sin la barra de tareas).
    int x = rc.left + (rc.right - rc.left - width) / 2; // Calculamos X para centrar.
    int y = rc.top + (rc.bottom - rc.top - height) / 2; // Calculamos Y para centrar.
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height, SWP_SHOWWINDOW); // Movemos la ventana y la mostramos.
}

// Esta función le dice a Windows que redibuje toda la grilla.
// Es como decirle "che, volvé a pintar todo porque algo cambió".
void InvalidateGrid(HWND hwnd) {
    RECT rc; // Rectángulo para guardar el área de la ventana.
    GetClientRect(hwnd, &rc); // Sacamos el área de la ventana (sin bordes).
    InvalidateRect(hwnd, &rc, TRUE); // Le decimos a Windows que redibuje todo el área.
}

// Esta función maneja las miniaturas solo de las ventanas que se ven en la grilla.
// Es como un sistema inteligente: solo saca fotos de las ventanas que realmente necesitamos mostrar.
void RegisterVisibleThumbnails(HWND host, std::vector<WindowInfo>& windows, const std::vector<bool>& visible) {
    for (size_t i = 0; i < windows.size(); ++i) { // Recorremos cada ventana.
        HWND hwnd = windows[i].hwnd; // Handle de la ventana.
        if (visible[i]) { // Si la ventana se ve en la grilla.
            if (!g_thumbnailMap[hwnd]) { // Si no tiene miniatura todavía.
                HTHUMBNAIL thumb = nullptr; // Variable para la nueva miniatura.
                if (SUCCEEDED(DwmRegisterThumbnail(host, hwnd, &thumb))) { // Si pudimos crear la miniatura.
                    g_thumbnailMap[hwnd] = thumb; // La guardamos en el mapa.
                    windows[i].thumbnail = thumb; // La asignamos a la ventana.
                }
            } else { // Si ya tiene miniatura.
                windows[i].thumbnail = g_thumbnailMap[hwnd]; // La reusamos.
            }
        } else { // Si la ventana no se ve en la grilla.
            if (g_thumbnailMap[hwnd]) { // Si tiene miniatura.
                DwmUnregisterThumbnail(g_thumbnailMap[hwnd]); // La borramos.
                g_thumbnailMap[hwnd] = nullptr; // La marcamos como que no existe.
                windows[i].thumbnail = nullptr; // La quitamos de la ventana.
            }
        }
    }
}

// Esta función era para debuggear el orden de las ventanas.
// Ahora está vacía porque ya no necesitamos hacer debug.
void DebugOrder(const std::vector<WindowInfo>& windows, int dragIdx, int hoverIdx, int insertIdx) {
    // (Eliminado: logging de depuración innecesario).
}

// Esta función dibuja un ícono de pin (como un alfiler).
// Es dorado si está fijado, gris si no está fijado.
void DrawPinIcon(HDC hdc, int x, int y, bool pinned) {
    int w = 22, h = 22; // Tamaño del ícono.
    HPEN pen = CreatePen(PS_SOLID, 2, pinned ? RGB(255, 215, 0) : RGB(180, 180, 180)); // Lápiz: dorado si fijado, gris si no.
    HBRUSH brush = CreateSolidBrush(pinned ? RGB(255, 215, 0) : RGB(220, 220, 220)); // Pincel: dorado si fijado, gris claro si no.
    HPEN oldPen = (HPEN)SelectObject(hdc, pen); // Guardamos el lápiz anterior.
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush); // Guardamos el pincel anterior.
    // Cabeza del pin (círculo).
    Ellipse(hdc, x, y, x + w/2, y + h/2);
    // Cuerpo del pin (línea).
    MoveToEx(hdc, x + w/4, y + h/4, NULL); // Movemos al centro de la cabeza.
    LineTo(hdc, x + w/4, y + h); // Dibujamos una línea hasta abajo.
    // Base del pin (rectángulo chico).
    Rectangle(hdc, x + w/4 - 3, y + h - 4, x + w/4 + 3, y + h); // Dibujamos la base.
    SelectObject(hdc, oldPen); // Restauramos el lápiz anterior.
    SelectObject(hdc, oldBrush); // Restauramos el pincel anterior.
    DeleteObject(pen); // Borramos el lápiz que creamos.
    DeleteObject(brush); // Borramos el pincel que creamos.
}

// Esta función dibuja un ícono de cerrar (X roja).
// Es como el botón de cerrar de las ventanas de Windows.
void DrawCloseIcon(HDC hdc, int x, int y, int size) {
    int w = size, h = size; // Ancho y alto del ícono.
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(220, 80, 80)); // Lápiz rojo para la X.
    HPEN oldPen = (HPEN)SelectObject(hdc, pen); // Guardamos el lápiz anterior.
    // Dibuja la X (dos líneas que se cruzan).
    MoveToEx(hdc, x, y, NULL); // Movemos a la esquina superior izquierda.
    LineTo(hdc, x + w, y + h); // Dibujamos una línea diagonal.
    MoveToEx(hdc, x + w, y, NULL); // Movemos a la esquina superior derecha.
    LineTo(hdc, x, y + h); // Dibujamos la otra línea diagonal.
    SelectObject(hdc, oldPen); // Restauramos el lápiz anterior.
    DeleteObject(pen); // Borramos el lápiz que creamos.
}

// Esta función dibuja un ícono de '#' para el botón de fijar a posición.
// Es azul y en negrita para que se vea bien.
void DrawPinToPosIcon(HDC hdc, int x, int y, int size) {
    SetBkMode(hdc, TRANSPARENT); // Fondo transparente.
    HFONT hFont = CreateFontW(size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, // Creamos una fuente en negrita.
        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont); // Guardamos la fuente anterior.
    SetTextColor(hdc, RGB(80, 180, 255)); // Color azul para el texto.
    TextOutW(hdc, x, y, L"#", 1); // Dibujamos el símbolo #.
    SelectObject(hdc, oldFont); // Restauramos la fuente anterior.
    DeleteObject(hFont); // Borramos la fuente que creamos.
}

// Esta función dibuja el número de orden sobre cada miniatura.
// Es como poner etiquetas numeradas en las cajas: 1, 2, 3, etc.
void DrawNumberOverlay(HDC hdc, int x, int y, int number) {
    wchar_t buf[4]; // Buffer para convertir el número a texto.
    wsprintfW(buf, L"%d", number); // Convertimos el número a texto (maneja 1-999).

    // Ajustamos el tamaño de fuente y posicionamiento según cuántos dígitos tiene el número.
    int fontSize; // Tamaño de la fuente.
    int xOffset; // Cuánto nos movemos a la izquierda para centrar.
    
    if (number < 10) { // Si es un solo dígito (1-9).
        fontSize = 18; // Fuente más grande.
        xOffset = 6; // Centramos el dígito único.
    } 
    else if (number < 100) { // Si son dos dígitos (10-99).
        fontSize = 16; // Fuente mediana.
        xOffset = 9; // Centramos dos dígitos.
    }
    else { // Si son tres dígitos (100-999) - aunque es poco probable necesitar tantas ventanas.
        fontSize = 14; // Fuente más chica.
        xOffset = 12; // Centramos tres dígitos.
    }

    // Creamos la fuente con el tamaño apropiado.
    HFONT hFont = CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, // Fuente en negrita.
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    
    if (hFont) { // Si pudimos crear la fuente.
        HFONT oldFont = (HFONT)SelectObject(hdc, hFont); // Guardamos la fuente anterior.
        SetBkMode(hdc, TRANSPARENT); // Fondo transparente.
        
        // Versión simplificada sin efectos visuales para mejor rendimiento
        SetTextColor(hdc, RGB(80, 180, 255)); // Color azulado.
        TextOutW(hdc, x - xOffset, y, buf, lstrlenW(buf)); // Dibujamos el texto normal.

        SelectObject(hdc, oldFont); // Restauramos la fuente anterior.
        DeleteObject(hFont); // Borramos la fuente que creamos.
    }
}

// Esta función ordena las ventanas: primero las fijadas, luego las no fijadas.
// Es como separar las cartas importantes de las normales.
void SortWindowsForGrid(std::vector<WindowInfo>& windows) {
    std::stable_sort(windows.begin(), windows.end(), [](const WindowInfo& a, const WindowInfo& b) { // Ordenamos de forma estable.
        return a.pinned > b.pinned; // Las fijadas van primero (true > false).
    });
}

// Estas constantes definen cómo se ve el borde cuando una ventana está fijada.
const int PERSISTENT_CONTOUR_THICKNESS = 1; // Grosor del contorno rojo en modo persistente.
const int PERSISTENT_CONTOUR_COLOR = 0x00FF44; // Color del contorno en modo persistente (hexadecimal, 0xRRGGBB).

// Esta función maneja las teclas Enter y Escape en el campo de entrada numérica.
// Es como interceptar las teclas antes de que lleguen al control normal.
LRESULT CALLBACK NumberEditProc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN) { // Si se presionó una tecla.
        if (wParam == VK_RETURN) { // Si es Enter.
            HWND hParent = GetParent(hEdit); // Agarramos la ventana padre.
            SendMessageW(hParent, WM_COMMAND, 12, 0); // Enviamos mensaje de OK.
            return 0; // No procesamos más la tecla.
        } else if (wParam == VK_ESCAPE) { // Si es Escape.
            HWND hParent = GetParent(hEdit); // Agarramos la ventana padre.
            SendMessageW(hParent, WM_COMMAND, 13, 0); // Enviamos mensaje de Cancelar.
            return 0; // No procesamos más la tecla.
        }
    }
    return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hEdit, GWLP_USERDATA), hEdit, msg, wParam, lParam); // Pasamos el mensaje al control original.
}

// Esta función crea una ventana modal para ingresar un número.
// Es como un popup que te pide que escribas un número, con tema negro OLED.
int ShowNumberInputDialog(HWND parent, const wchar_t* prompt) {
    static int result = 0; // Variable estática para guardar el resultado.
    static HWND sEdit = nullptr; // Variable estática para el campo de entrada.
    WNDCLASSW wc = {0}; // Estructura para definir la clase de ventana.
    wc.lpfnWndProc = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT { // Función lambda que maneja los mensajes de la ventana.
        static HWND hEdit, hOK, hCancel; // Variables estáticas para los controles.
        static WNDPROC oldEditProc = nullptr; // Variable estática para guardar el procedimiento original del edit.
        switch (msg) { // Switch para manejar diferentes mensajes.
        case WM_CREATE: { // Cuando se crea la ventana.
            // Creamos los controles de la ventana modal: texto, campo de entrada, botones.
            CreateWindowW(L"STATIC", (LPCWSTR)((CREATESTRUCT*)lParam)->lpCreateParams, WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, // Texto estático.
                10, 10, 200, 20, hWnd, (HMENU)10, nullptr, nullptr);
            hEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, // Campo de entrada solo para números.
                10, 35, 200, 24, hWnd, (HMENU)11, nullptr, nullptr);
            sEdit = hEdit; // Guardamos la referencia al edit.
            hOK = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, // Botón OK.
                30, 70, 60, 28, hWnd, (HMENU)12, nullptr, nullptr);
            hCancel = CreateWindowW(L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE, // Botón Cancelar.
                120, 70, 60, 28, hWnd, (HMENU)13, nullptr, nullptr);
            // Subclaseamos el campo de entrada para manejar Enter/Esc.
            oldEditProc = (WNDPROC)SetWindowLongPtrW(hEdit, GWLP_WNDPROC, (LONG_PTR)NumberEditProc); // Cambiamos el procedimiento de ventana.
            SetWindowLongPtrW(hEdit, GWLP_USERDATA, (LONG_PTR)oldEditProc); // Guardamos el procedimiento original.
            break;
        }
        case WM_SHOWWINDOW: // Cuando se muestra la ventana.
            if (wParam && sEdit) { // Si se está mostrando y tenemos el edit.
                SetTimer(hWnd, 1, 10, NULL); // Timer corto para enfocar el campo de entrada.
            }
            break;
        case WM_TIMER: // Cuando se activa el timer.
            if (wParam == 1 && sEdit) { // Si es nuestro timer y tenemos el edit.
                SetFocus(sEdit); // Enfocamos el campo de entrada.
                SendMessageW(sEdit, EM_SETSEL, 0, -1); // Seleccionamos todo el texto.
                KillTimer(hWnd, 1); // Matamos el timer.
            }
            break;
        case WM_CTLCOLORSTATIC: // Color de los controles estáticos.
        case WM_CTLCOLOREDIT: // Color de los campos de entrada.
        case WM_CTLCOLORBTN: { // Color de los botones.
            // Aplicamos fondo negro y texto blanco a todos los controles.
            HDC hdc = (HDC)wParam; // Contexto de dispositivo.
            SetBkColor(hdc, RGB(0,0,0)); // Fondo negro.
            SetTextColor(hdc, RGB(255,255,255)); // Texto blanco.
            static HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0)); // Pincel negro estático.
            return (LRESULT)hBrush; // Devolvemos el pincel.
        }
        case WM_COMMAND: // Cuando se hace clic en un botón.
            if (LOWORD(wParam) == 12) { // Si es el botón OK.
                wchar_t buf[32] = {0}; // Buffer para el texto.
                GetWindowTextW(hEdit, buf, 31); // Sacamos el texto del edit.
                SetWindowLongPtrW(hWnd, GWLP_USERDATA, _wtoi(buf)); // Convertimos a número y lo guardamos.
                PostMessageW(hWnd, WM_APP + 1, 0, 0); // Enviamos mensaje para cerrar.
                return 0; // No procesamos más.
            } else if (LOWORD(wParam) == 13) { // Si es el botón Cancelar.
                SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0); // Guardamos 0 como resultado.
                PostMessageW(hWnd, WM_APP + 1, 0, 0); // Enviamos mensaje para cerrar.
                return 0; // No procesamos más.
            }
            break;
        case WM_CLOSE: // Cuando se cierra la ventana.
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0); // Guardamos 0 como resultado.
            PostMessageW(hWnd, WM_APP + 1, 0, 0); // Enviamos mensaje para cerrar.
            return 0; // No procesamos más.
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam); // Procesamiento por defecto.
    };
    wc.hInstance = GetModuleHandleW(nullptr); // Instancia de la aplicación.
    wc.lpszClassName = L"NumInputDlgClass"; // Nombre de la clase.
    wc.hbrBackground = CreateSolidBrush(RGB(0,0,0)); // Fondo negro.
    RegisterClassW(&wc); // Registramos la clase.
    HWND hDialog = CreateWindowExW(WS_EX_DLGMODALFRAME, wc.lpszClassName, L"Pin a position", // Creamos la ventana.
        WS_POPUP | WS_VISIBLE | WS_SYSMENU, // Estilos de la ventana.
        (GetSystemMetrics(SM_CXSCREEN)-220)/2, (GetSystemMetrics(SM_CYSCREEN)-110)/2, 220, 120, parent, nullptr, wc.hInstance, (LPVOID)prompt); // Posición y tamaño centrados.
    SetWindowLongPtrW(hDialog, GWLP_USERDATA, 0); // Inicializamos el resultado en 0.
    MSG msg; // Variable para los mensajes.
    BOOL done = FALSE; // Variable para controlar el bucle.
    while (!done && GetMessageW(&msg, nullptr, 0, 0)) { // Bucle de mensajes.
        if (msg.message == WM_APP + 1 && msg.hwnd == hDialog) { // Si es nuestro mensaje de cierre.
            done = TRUE; // Terminamos el bucle.
        } else { // Si no es nuestro mensaje.
            TranslateMessage(&msg); // Traducimos el mensaje.
            DispatchMessageW(&msg); // Lo enviamos a la ventana.
        }
    }
    int ret = (int)GetWindowLongPtrW(hDialog, GWLP_USERDATA); // Sacamos el resultado.
    DestroyWindow(hDialog); // Destruimos la ventana.
    UnregisterClassW(wc.lpszClassName, wc.hInstance); // Desregistramos la clase.
    return ret; // Devolvemos el resultado.
}

// Estas constantes definen los mensajes y IDs para el ícono de la bandeja del sistema.
#define WM_TRAYICON (WM_USER + 100) // Mensaje personalizado para el ícono de la bandeja.
#define ID_TRAY_SUSPEND 2001 // ID para suspender la aplicación.
#define ID_TRAY_EXIT 2002 // ID para salir de la aplicación.
#define ID_TRAY_RESTART 2003 // ID para reiniciar la aplicación.
#define ID_TRAY_SETTINGS 2004 // ID para abrir el diálogo de configuración.

// Variables para manejar el estado de la aplicación y el ícono de la bandeja.
static bool g_suspended = false; // Estado de suspensión (si está pausada o no).
static bool g_altAEnabled = true; // Estado de habilitación de Alt+A (si está activo o no).
static NOTIFYICONDATA nid = {0}; // Estructura para el ícono de la bandeja del sistema.

// Esta función crea un ícono cuadrado rojo en memoria.
// Es como dibujar un cuadrado rojo y convertirlo en un ícono para la bandeja del sistema.
HICON CreateRedSquareIcon() {
    HDC hdc = GetDC(NULL); // Obtenemos el contexto de dispositivo de la pantalla.
    HDC memDC = CreateCompatibleDC(hdc); // Creamos un contexto de memoria compatible.
    HBITMAP bmp = CreateCompatibleBitmap(hdc, 32, 32); // Creamos un bitmap de 32x32 píxeles.
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, bmp); // Seleccionamos el bitmap en el contexto de memoria.
    HBRUSH brush = CreateSolidBrush(RGB(220, 0, 0)); // Creamos un pincel rojo.
    RECT rc = {0,0,32,32}; // Rectángulo de 32x32 píxeles.
    FillRect(memDC, &rc, brush); // Llenamos el rectángulo con el pincel rojo.
    DeleteObject(brush); // Borramos el pincel.
    ICONINFO ii = {0}; // Estructura para la información del ícono.
    ii.fIcon = TRUE; // Es un ícono (no un cursor).
    ii.hbmMask = bmp; // El bitmap también sirve como máscara.
    ii.hbmColor = bmp; // El bitmap es el color del ícono.
    HICON hIcon = CreateIconIndirect(&ii); // Creamos el ícono.
    SelectObject(memDC, oldBmp); // Restauramos el bitmap original.
    DeleteObject(bmp); // Borramos el bitmap.
    DeleteDC(memDC); // Borramos el contexto de memoria.
    ReleaseDC(NULL, hdc); // Liberamos el contexto de la pantalla.
    return hIcon; // Devolvemos el ícono creado.
}

// Esta función registra el programa como una tarea programada para que se ejecute como administrador al iniciar sesión.
// Es como decirle a Windows: "che, ejecutá este programa cada vez que me conecte".
void RegisterRunAsAdminOnLogin() {
    char exePath[MAX_PATH]; // Buffer para la ruta del ejecutable.
    GetModuleFileNameA(NULL, exePath, MAX_PATH); // Sacamos la ruta del ejecutable actual.
    std::string taskName = "BetterAltTab_RunAsAdmin"; // Nombre de la tarea programada.
    std::string checkCmd = "schtasks /Query /TN \"" + taskName + "\" >nul 2>&1"; // Comando para verificar si la tarea ya existe.
    if (system(checkCmd.c_str()) == 0) { // Si el comando devuelve 0, la tarea ya existe.
        return; // La tarea ya existe, nos vamos.
    }
    std::string cmd = "schtasks /Create /F /RL HIGHEST /SC ONLOGON /TN \"" + taskName + // Comando para crear la tarea.
                      "\" /TR \"\\\"" + exePath + "\\\"\"";
    int result = system(cmd.c_str()); // Ejecutamos el comando.
    if (result != 0) { // Si falló.
        MessageBoxA(NULL, "Failed to create scheduled task for auto-run as admin.", "Error", MB_OK | MB_ICONERROR); // Mostramos un error.
    }
}

// Esta es una definición de interfaz para el proveedor de servicios de Windows.
// Es como un intermediario que nos permite acceder a diferentes servicios del sistema.
#ifndef __IServiceProvider_INTERFACE_DEFINED__
#define __IServiceProvider_INTERFACE_DEFINED__
EXTERN_C const IID IID_IServiceProvider; // ID único de la interfaz.
MIDL_INTERFACE("6d5140c1-7436-11ce-8034-00aa006009fa") // Identificador único de la interfaz.
IServiceProvider : public IUnknown // Interfaz que hereda de IUnknown (interfaz base de COM).
{
public:
    virtual HRESULT STDMETHODCALLTYPE QueryService( // Función virtual para consultar servicios.
        /* [in] */ REFGUID guidService, // ID del servicio que queremos.
        /* [in] */ REFIID riid, // ID de la interfaz que queremos.
        /* [out] */ void **ppvObject) = 0; // Puntero donde se guarda el resultado.
};
#endif

// Estas son constantes que identifican diferentes servicios y interfaces de Windows.
// Son como los números de teléfono de diferentes servicios del sistema.
const CLSID CLSID_ImmersiveShell = {0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}}; // ID del shell inmersivo.

// Agregamos definiciones para las notificaciones de escritorios virtuales.
const CLSID CLSID_VirtualDesktopNotificationService = {0x0CD45E71, 0xD927, 0x4F15, {0x8B, 0x0A, 0x8F, 0xEF, 0x52, 0x53, 0x37, 0xBF}}; // ID del servicio de notificaciones.
const IID IID_IVirtualDesktopNotification = {0xC179334C, 0x4295, 0x40D3, {0xBE, 0xA1, 0xC6, 0x54, 0xD9, 0x65, 0x60, 0x5A}}; // ID de la interfaz de notificaciones.
const IID IID_IVirtualDesktopNotificationService = {0x0CD45E71, 0xD927, 0x4F15, {0x8B, 0x0A, 0x8F, 0xEF, 0x52, 0x53, 0x37, 0xBF}}; // ID del servicio de notificaciones.

// Mensaje personalizado para cuando cambia el escritorio virtual.
#define WM_VIRTUAL_DESKTOP_CHANGED (WM_USER + 200) // Nuestro mensaje personalizado.

// Variable global para guardar el handle de la ventana principal.
HWND g_mainHwnd = nullptr; // Handle de la ventana principal (para notificaciones).

// Estas estructuras definen las interfaces para recibir notificaciones de escritorios virtuales.
// Es como suscribirse a un canal de noticias sobre escritorios virtuales.
struct IVirtualDesktopNotification : public IUnknown { // Interfaz para recibir notificaciones.
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(void*) { return S_OK; } // Cuando se crea un escritorio.
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(void*, void*) { return S_OK; } // Cuando empieza a destruirse.
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(void*, void*) { return S_OK; } // Cuando falla la destrucción.
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(void*, void*) { return S_OK; } // Cuando se destruye.
    virtual HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(void*) { return S_OK; } // Cuando cambia la vista.
    virtual HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(void* pDesktopOld, void* pDesktopNew) = 0; // Cuando cambia el escritorio actual.
};

struct IVirtualDesktopNotificationService : public IUnknown { // Interfaz del servicio de notificaciones.
    virtual HRESULT STDMETHODCALLTYPE Register(IVirtualDesktopNotification* pNotification, DWORD* pdwCookie) = 0; // Para registrarse.
    virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD dwCookie) = 0; // Para desregistrarse.
};

// Esta clase implementa la interfaz de notificaciones de escritorios virtuales.
// Es como un receptor que escucha cuando cambias de escritorio virtual.
class VirtualDesktopNotificationImpl : public IVirtualDesktopNotification { // Hereda de la interfaz de notificaciones.
    LONG m_refCount = 1; // Contador de referencias (para COM).
public:
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_refCount); } // Incrementa el contador de referencias.
    ULONG STDMETHODCALLTYPE Release() override { // Decrementa el contador de referencias.
        ULONG res = InterlockedDecrement(&m_refCount); // Decrementa de forma segura.
        if (res == 0) delete this; // Si no hay más referencias, se borra a sí mismo.
        return res; // Devuelve el nuevo contador.
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override { // Consulta una interfaz específica.
        if (riid == IID_IUnknown || riid == IID_IVirtualDesktopNotification) { // Si piden una interfaz que soportamos.
            *ppvObject = static_cast<IVirtualDesktopNotification*>(this); // Devolvemos un puntero a nosotros mismos.
            AddRef(); // Incrementamos la referencia.
            return S_OK; // Todo bien.
        }
        *ppvObject = nullptr; // No soportamos esa interfaz.
        return E_NOINTERFACE; // Error: interfaz no soportada.
    }
    HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(void* /*pDesktopOld*/, void* /*pDesktopNew*/) override { // Cuando cambia el escritorio actual.
        if (g_mainHwnd) PostMessageW(g_mainHwnd, WM_VIRTUAL_DESKTOP_CHANGED, 0, 0); // Enviamos un mensaje a la ventana principal.
        return S_OK; // Todo bien.
    }
};

// Variable para el mensaje de creación de la barra de tareas.
UINT WM_TASKBARCREATED = 0; // Mensaje que se envía cuando se crea la barra de tareas.

// ===============================
// Implementación de funciones para detección de ventanas cerradas
// ===============================

// Hook para detectar cuando se cierran ventanas
LRESULT CALLBACK WindowHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    
    // Detectar cuando se cierra una ventana
    if (wParam == WM_DESTROY || wParam == WM_CLOSE) {
        HWND hwnd = (HWND)lParam;
        if (hwnd && hwnd != g_mainHwnd) {
            // Marcar que una ventana se cerró
            g_windowClosed = true;
            
            // Enviar mensaje personalizado a la ventana principal
            if (g_mainHwnd) {
                PostMessageW(g_mainHwnd, WM_TIMER, 300, 0);
            }
        }
    }
    
    // También detectar cuando se oculta una ventana (puede indicar que se está cerrando)
    if (wParam == WM_SHOWWINDOW && lParam == FALSE) {
        HWND hwnd = (HWND)lParam;
        if (hwnd && hwnd != g_mainHwnd) {
            // Verificar si la ventana realmente se cerró
            if (!IsWindow(hwnd)) {
                g_windowClosed = true;
                if (g_mainHwnd) {
                    PostMessageW(g_mainHwnd, WM_TIMER, 300, 0);
                }
            }
        }
    }
    
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Instala el hook para detectar ventanas cerradas
void InstallWindowHook() {
    if (!g_windowHook) {
        g_windowHook = SetWindowsHookEx(WH_CALLWNDPROC, WindowHookProc, 
                                       GetModuleHandle(nullptr), GetCurrentThreadId());
    }
}

// Desinstala el hook
void UninstallWindowHook() {
    if (g_windowHook) {
        UnhookWindowsHookEx(g_windowHook);
        g_windowHook = nullptr;
    }
}

// Esta es la función principal de la aplicación Windows.
// Es como el punto de entrada: acá empieza todo cuando ejecutás el programa.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    RegisterRunAsAdminOnLogin(); // Registramos la aplicación para que se ejecute como admin al iniciar sesión.
    const wchar_t CLASS_NAME[] = L"BetterAltTab_Unnamed10110Class"; // Nombre de la clase de ventana.
    
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) }; // Estructura para definir la clase de ventana.
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // Estilos de la clase: redibujar al cambiar tamaño, doble clic.
    wc.lpfnWndProc = WndProc; // Función que maneja los mensajes de la ventana.
    wc.hInstance = hInstance; // Instancia de la aplicación.
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // Cursor por defecto (flecha).
    wc.lpszClassName = CLASS_NAME; // Nombre de la clase.
    
    RegisterClassEx(&wc); // Registramos la clase de ventana.
    
    // Cargar configuración desde INI
    LoadConfiguration();
    
    // Cargar configuración de filtros
    LoadFilterSettings();
    
    // Inicializar optimizaciones de rendimiento
    LoadPerformanceSettings();
    ResetPerformanceCounters();
    
    // Inicializamos COM y el administrador de escritorios virtuales.
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); // Inicializamos COM en modo apartment-threaded.
    HRESULT hr = CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&g_vdm)); // Creamos el administrador de escritorios virtuales.
    
    // Establecemos el ancho de la superposición al 88% del ancho de la pantalla y el alto al 80% del alto de la pantalla.
    RECT rc; // Rectángulo para guardar el área de trabajo.
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0); // Obtenemos el área de trabajo (sin la barra de tareas).
    int screenW = rc.right - rc.left; // Ancho de la pantalla.
    int screenH = rc.bottom - rc.top; // Alto de la pantalla.
    g_overlayWidth = (int)(screenW * 0.88); // 88% del ancho de la pantalla.
    g_overlayHeight = (int)(screenH * 0.80); // 80% del alto de la pantalla.
    UpdatePreviewSize(); // Recalculate with final dimensions
    // Creamos la ventana como overlay para mayor eficiencia.
    HWND hwnd = CreateWindowEx( // Creamos la ventana.
        WS_EX_TOPMOST, // Estilos extendidos: siempre arriba, sin capas para mejor rendimiento.
        CLASS_NAME, L"BetterAltTab_Unnamed10110", // Clase y título de la ventana.
        WS_POPUP, // Estilo de ventana: popup (sin bordes).
        CW_USEDEFAULT, CW_USEDEFAULT, g_overlayWidth, g_overlayHeight, // Posición y tamaño.
        nullptr, nullptr, hInstance, nullptr); // Parámetros adicionales.
    
    if (!hwnd) return 0; // Si no se pudo crear la ventana, salimos.
    g_mainHwnd = hwnd; // Guardamos el handle para las notificaciones.
    
    // Configuramos la ventana sin efectos visuales para mejor rendimiento.
    // SetLayeredWindowAttributes(hwnd, 0, g_overlayAlpha, LWA_ALPHA); // Comentado: sin transparencia para mejor rendimiento.
    
    // Establecemos una región rectangular simple para mejor rendimiento.
    // HRGN rgn = CreateRoundRectRgn(0, 0, g_overlayWidth, g_overlayHeight, 60, 60); // Comentado: sin bordes redondeados para mejor rendimiento.
    // SetWindowRgn(hwnd, rgn, FALSE); // Comentado: sin región personalizada para mejor rendimiento.
    CenterOverlayWindow(hwnd, g_overlayWidth, g_overlayHeight); // Centramos la ventana en la pantalla.
    LoadGridOrder(g_gridOrder); // Cargamos el orden guardado de las ventanas.
    // RegisterHotKey(hwnd, HOTKEY_ID, MOD_CONTROL, VK_DECIMAL); // Removido: atajo Ctrl+Numpad.
    RegisterHotKey(hwnd, HOTKEY_ID_ALTQ, MOD_ALT, 'Q'); // Registramos el atajo Alt+Q.
    RegisterHotKey(hwnd, HOTKEY_ID_ALTA, MOD_ALT, 'A'); // Registramos el atajo Alt+A (filtro persistente).
    
    // Instalar hook para detectar ventanas cerradas
    InstallWindowHook();
    
    // Inicializar timer de limpieza automática de rendimiento
    SetTimer(hwnd, 300, g_cleanupInterval, NULL);
    ShowWindow(hwnd, SW_HIDE); // Ocultamos la ventana al inicio.
    
    // Inicializamos el ícono de la bandeja del sistema.
    HICON hIcon = CreateRedSquareIcon(); // Creamos el ícono rojo cuadrado.
    nid.cbSize = sizeof(nid); // Tamaño de la estructura.
    nid.hWnd = hwnd; // Handle de la ventana.
    nid.uID = 1; // ID del ícono.
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; // Flags: ícono, mensaje y tooltip.
    nid.uCallbackMessage = WM_TRAYICON; // Mensaje que se envía cuando se hace clic.
    nid.hIcon = hIcon; // El ícono que creamos.
                    // Actualizar tooltip con información de rendimiento
                wchar_t tooltipText[256];
                swprintf_s(tooltipText, L"BetterAltTab_Unnamed10110\nMemory: %d MB", (int)(g_thumbnailMemoryUsage / (1024 * 1024)));
                lstrcpyW(nid.szTip, tooltipText); // Texto del tooltip con info de memoria.
    Shell_NotifyIcon(NIM_ADD, &nid); // Agregamos el ícono a la bandeja del sistema.
    
    // Registramos las notificaciones de escritorios virtuales (forma correcta).
    IServiceProvider* pServiceProvider = nullptr; // Proveedor de servicios.
    IVirtualDesktopNotificationService* pNotificationService = nullptr; // Servicio de notificaciones.
    VirtualDesktopNotificationImpl* pNotification = nullptr; // Nuestra implementación de notificaciones.
    DWORD notificationCookie = 0; // Cookie para desregistrarse después.
    HRESULT hrSP = CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&pServiceProvider)); // Creamos el proveedor de servicios.
    if (SUCCEEDED(hrSP) && pServiceProvider) { // Si se creó exitosamente.
        HRESULT hrVS = pServiceProvider->QueryService(CLSID_VirtualDesktopNotificationService, IID_IVirtualDesktopNotificationService, (void**)&pNotificationService); // Obtenemos el servicio de notificaciones.
        if (SUCCEEDED(hrVS) && pNotificationService) { // Si se obtuvo exitosamente.
            pNotification = new VirtualDesktopNotificationImpl(); // Creamos nuestra implementación.
            if (SUCCEEDED(pNotificationService->Register(pNotification, &notificationCookie))) { // Nos registramos para recibir notificaciones.
                // Registrado exitosamente.
            } else { // Si falló el registro.
                delete pNotification; // Borramos nuestra implementación.
                pNotification = nullptr; // La marcamos como nula.
            }
        }
        pServiceProvider->Release(); // Liberamos el proveedor de servicios.
    }
    
    WM_TASKBARCREATED = RegisterWindowMessageW(L"TaskbarCreated"); // Registramos el mensaje de creación de la barra de tareas.
    
    // Bucle principal de mensajes de la aplicación.
    MSG msg = {}; // Variable para los mensajes.
    while (GetMessage(&msg, nullptr, 0, 0)) { // Mientras haya mensajes.
        TranslateMessage(&msg); // Traducimos el mensaje.
        DispatchMessage(&msg); // Lo enviamos a la ventana correspondiente.
    }
    
    // Limpieza al salir de la aplicación.
    if (g_vdm) g_vdm->Release(); // Liberamos el administrador de escritorios virtuales.
    CoUninitialize(); // Desinicializamos COM.
    // UnregisterHotKey(hwnd, HOTKEY_ID); // Removido: atajo Ctrl+Numpad.
    UnregisterHotKey(hwnd, HOTKEY_ID_ALTQ); // Desregistramos el atajo Alt+Q.
    UnregisterHotKey(hwnd, HOTKEY_ID_ALTA); // Desregistramos el atajo Alt+A.
    
    // Limpiar timer de rendimiento
    KillTimer(hwnd, 300);
    Shell_NotifyIcon(NIM_DELETE, &nid); // Borramos el ícono de la bandeja del sistema.
    if (hIcon) DestroyIcon(hIcon); // Destruimos el ícono que creamos.
    // Limpieza de las notificaciones.
    if (pNotificationService && notificationCookie) // Si tenemos el servicio y la cookie.
        pNotificationService->Unregister(notificationCookie); // Nos desregistramos.
    if (pNotificationService) // Si tenemos el servicio.
        pNotificationService->Release(); // Lo liberamos.
    if (pNotification) // Si tenemos nuestra implementación.
        pNotification->Release(); // La liberamos.
    return 0; // Salimos de la aplicación.
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static int lastTooltipIdx = -1; // Variable estática para recordar el último tooltip mostrado.
    
    // Manejo especial para restaurar el ícono de la bandeja si Explorer se reinicia.
    if (msg == WM_TASKBARCREATED) { // Si se recreó la barra de tareas.
        Shell_NotifyIcon(NIM_ADD, &nid); // Volvemos a agregar nuestro ícono.
        return 0; // No procesamos más.
    }
    switch (msg) { // Switch para manejar diferentes tipos de mensajes.
        case WM_TRAYICON: // Mensaje del ícono de la bandeja del sistema.
            if (lParam == WM_RBUTTONUP) { // Si se hizo clic derecho.
                // Mostramos el menú contextual.
                POINT pt; // Variable para la posición del cursor.
                GetCursorPos(&pt); // Obtenemos la posición del cursor.
                HMENU hMenu = CreatePopupMenu(); // Creamos un menú popup.
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_SETTINGS, L"Settings...");
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_FILTERS, L"Filters...");
                AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenuW(hMenu, MF_STRING | (g_altAEnabled ? MF_CHECKED : 0), ID_TRAY_TOGGLE_ALTA, g_altAEnabled ? L"Alt+A: ON" : L"Alt+A: OFF"); // Toggle para Alt+A
                AppendMenuW(hMenu, MF_STRING | (g_dynamicOrder ? MF_CHECKED : 0), ID_TRAY_TOGGLE_DYNAMIC_MODE, g_dynamicOrder ? L"Dynamic Mode: ON" : L"Fix Z-order(Not Dynamic): ON"); // Toggle para modo dinámico
                AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_FULL_PERFORMANCE, L"Full Performance"); // Configuración de máximo rendimiento
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_FULL_LOW_RESOURCES, L"Full Low Resources"); // Configuración de mínimo consumo
                AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenuW(hMenu, MF_STRING | (g_suspended ? MF_CHECKED : 0), ID_TRAY_SUSPEND, g_suspended ? L"Reanudar" : L"Suspend"); // Opción de suspender/reanudar.
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_RESTART, L"Restart"); // Opción de reiniciar.
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_CLEANUP, L"Cleanup Memory"); // Opción de limpieza de memoria.
                AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit"); // Opción de salir.
                SetForegroundWindow(hwnd); // Ponemos nuestra ventana al frente.
                int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL); // Mostramos el menú y obtenemos la selección.
                DestroyMenu(hMenu); // Destruimos el menú.
                if (cmd == ID_TRAY_SETTINGS) {
                    ShowSettingsDialog(hwnd);
                } else if (cmd == ID_TRAY_FILTERS) {
                    ShowFilterDialog(hwnd);
                } else if (cmd == ID_TRAY_TOGGLE_ALTA) { // Si eligieron toggle Alt+A
                    g_altAEnabled = !g_altAEnabled; // Cambiar estado de Alt+A
                    SaveFilterSettings(); // Guardar el nuevo estado en el INI
                    // Actualizar el texto del menú para la próxima vez
                } else if (cmd == ID_TRAY_TOGGLE_DYNAMIC_MODE) { // Si eligieron toggle modo dinámico
                    g_dynamicOrder = !g_dynamicOrder; // Cambiar estado del modo dinámico
                    SetWindowTextW(hwnd, g_dynamicOrder ? L"BetterAltTab_Unnamed10110 [Dynamic Order]" : L"BetterAltTab_Unnamed10110 [PERSISTENT Z-ORDER MODE]");
                    InvalidateGrid(hwnd); // Redibujar la grilla
                    SaveFilterSettings(); // Guardar el nuevo estado en el INI
                } else if (cmd == ID_TRAY_SUSPEND) { // Si eligieron suspender.
                    g_suspended = !g_suspended; // Cambiamos el estado de suspensión.
                } else if (cmd == ID_TRAY_EXIT) { // Si eligieron salir.
                    PostQuitMessage(0); // Enviamos mensaje para salir.
                } else if (cmd == ID_TRAY_RESTART) { // Si eligieron reiniciar.
                    wchar_t path[MAX_PATH]; // Buffer para la ruta del ejecutable.
                    GetModuleFileNameW(NULL, path, MAX_PATH); // Obtenemos la ruta del ejecutable actual.
                    ShellExecuteW(NULL, NULL, path, NULL, NULL, SW_SHOWNORMAL); // Ejecutamos una nueva instancia.
                    PostQuitMessage(0); // Salimos de la instancia actual.
                } else if (cmd == ID_TRAY_CLEANUP) { // Si eligieron limpiar memoria.
                    CleanupThumbnailCache(); // Limpiar cache de miniaturas
                    MonitorMemoryUsage(); // Verificar uso de memoria
                    CheckPerformanceMode(); // Ajustar modo de rendimiento
                    ResetPerformanceCounters(); // Resetear contadores
                } else if (cmd == ID_TRAY_FULL_PERFORMANCE) { // Si eligieron máximo rendimiento
                    ConfigureFullPerformance(); // Configurar para máximo rendimiento
                } else if (cmd == ID_TRAY_FULL_LOW_RESOURCES) { // Si eligieron mínimo consumo
                    ConfigureFullLowResources(); // Configurar para mínimo consumo
                }
            }
            break;
        case WM_HOTKEY: // Mensaje cuando se presiona un atajo de teclado registrado.
            if (g_suspended) return 0; // Si está suspendido, ignoramos los atajos.
            if (wParam == HOTKEY_ID_ALTQ || (wParam == HOTKEY_ID_ALTA && g_altAEnabled)) { // Si es nuestro atajo (Alt+Q, o Alt+A solo si está habilitado).
                if (IsWindowVisible(hwnd)) { // Si la superposición está abierta.
                    // Actúa como Tab: mueve la selección a la siguiente miniatura.
                    auto& windows = g_windows; // Referencia a la lista de ventanas.
                    int n = (int)windows.size(); // Cantidad de ventanas.
                    if (n > 0) { // Si hay ventanas.
                        g_selectedIndex = (g_selectedIndex + 1) % n; // Movemos al siguiente (cíclico).
                        g_hoverIndex = g_selectedIndex; // El hover sigue a la selección.
                        InvalidateGrid(hwnd); // Redibujamos la grilla.
                    }
                } else { // Si la superposición está cerrada.
                    g_scrollX = 0; // Reseteamos el scroll horizontal.
                    g_scrollY = 0; // Reseteamos el scroll vertical.
                    g_selectedIndex = 0; // Seleccionamos la primera ventana.
                    g_hoverIndex = 0; // El hover va a la primera ventana.
                    SetWindowTextW(hwnd, g_dynamicOrder ? L"BetterAltTab_Unnamed10110 [Dynamic Order]" : L"BetterAltTab_Unnamed10110 [PERSISTENT Z-ORDER MODE]"); // Cambiamos el título según el modo.
                    // Refrescamos las miniaturas cada vez que se muestra la superposición.
                    CleanupThumbnailCache(); // Limpieza automática del cache
                    MonitorMemoryUsage(); // Verificar uso de memoria
                    UnregisterAllThumbnails(); // Borramos todas las miniaturas anteriores.
                    g_windows = EnumerateWindows(hwnd); // Enumeramos las ventanas actuales.
                    
                    // Configurar comportamiento según el atajo usado
                    if (wParam == HOTKEY_ID_ALTQ) { // Si fue Alt+Q
                        g_lastHotkey = 2;
                        g_persistentFilterMode = false; // Desactivar modo persistente
                        g_filteringEnabled = false; // Desactivar filtros
                        g_filterText.clear(); // Limpiar filtro
                        // Para Alt+Q, asegurar que se usen todas las ventanas sin filtros
                        g_filteredWindows.clear(); // Limpiar ventanas filtradas
                        
                        // Restaurar configuración de filtros por defecto
                        g_filterByTitle = true;
                        g_filterByClassName = true;
                        g_filterByProcessName = true;
                        g_filterCaseSensitive = false;
                        g_filterExcludeHidden = true;
                        g_filterExcludeMinimized = false;
                        
                        // Forzar nueva enumeración para Alt+Q (evitar cache)
                        g_lastWindowEnumTime = 0;
                        
                        // Para Alt+Q, registrar miniaturas en g_windows
                        RegisterThumbnails(hwnd, g_windows);
                        
                        SetTimer(hwnd, 100, 50, NULL); // Timer cada 50ms para Alt+Q
                    } else if (wParam == HOTKEY_ID_ALTA) { // Si fue Alt+A
                        g_lastHotkey = 3;
                        g_persistentFilterMode = true;
                        g_filteringEnabled = true;
                        
                        // Asegurar que los filtros estén habilitados para Alt+A
                        g_filterByTitle = true;
                        g_filterByClassName = true;
                        g_filterByProcessName = true;
                        
                        // Si ya estamos en modo persistente, siempre comenzar en el primer thumbnail
                        if (g_quickFilterEdit) {
                            // Siempre comenzar en el primer thumbnail cuando se abre el overlay
                            g_selectedIndex = 0;
                            g_hoverIndex = 0;
                            InvalidateGrid(hwnd);
                        } else {
                            // Primera vez: crear el control y seleccionar el primer thumbnail
                            g_filterText.clear(); // Limpiar filtro anterior
                            
                            // Crear un control estático para mostrar el texto del filtro (sin tomar foco)
                            g_quickFilterEdit = CreateWindowW(L"STATIC", L"", 
                                WS_CHILD | WS_VISIBLE | SS_CENTER,
                                (OVERLAY_WIDTH - 400) / 2, 15, 400, 30, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
                            
                            // Configurar fuente y estilo
                            HFONT hFont = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
                            SendMessage(g_quickFilterEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
                            
                            // Inicializar selección en el primer thumbnail
                            g_selectedIndex = 0;
                            g_hoverIndex = 0;
                            
                            // Aplicar filtros después de que todo esté configurado
                            ApplyFilters();
                        }
                        
                        // Para Alt+A, registrar miniaturas en g_filteredWindows después de aplicar filtros
                        if (!g_filteredWindows.empty()) {
                            RegisterThumbnails(hwnd, g_filteredWindows);
                        } else {
                            RegisterThumbnails(hwnd, g_windows);
                        }
                        
                        // No enfocar el control - mantener el foco en la ventana principal
                        SetFocus(hwnd);
                    } else { // Si fue Ctrl+Numpad
                        g_lastHotkey = 1;
                        // Para Ctrl+Numpad, registrar miniaturas en g_windows
                        RegisterThumbnails(hwnd, g_windows);
                    }
                    
                    // Iniciamos timer para verificar teclas del numpad.
                    SetTimer(hwnd, 200, 100, NULL); // Timer cada 100ms para teclas del numpad (reducido de 50ms)
                    
                    // Timer único para verificar ventanas cerradas (consolidado)
                    SetTimer(hwnd, 300, 1000, NULL); // Timer cada 1000ms para verificar ventanas cerradas (reducido de múltiples timers)
                    
                    ShowWindow(hwnd, SW_SHOW); // Mostramos la superposición.
                    SetForegroundWindow(hwnd); // La ponemos al frente.
                    SetFocus(hwnd); // Aseguramos que el overlay reciba las teclas.
                    // Removido Sleep(10) que bloqueaba el thread principal
                    // En su lugar, usar PostMessage para procesar el foco de forma asíncrona
                    PostMessage(hwnd, WM_USER + 1, 0, 0); // Mensaje personalizado para procesar foco
                }
                return 0; // No procesamos más.
            }
            break;
        case WM_KEYUP: // Mensaje cuando se suelta una tecla.
            if (IsWindowVisible(hwnd) && g_lastHotkey == 1 && wParam == VK_CONTROL) { // Si la superposición está abierta y fue Ctrl+Numpad y se soltó Ctrl.
                // Enfocamos la ventana seleccionada antes de ocultarla.
                auto& windows = g_windows; // Referencia a la lista de ventanas.
                int n = (int)windows.size(); // Cantidad de ventanas.
                if (n > 0 && g_selectedIndex >= 0 && g_selectedIndex < n) { // Si hay ventanas y el índice es válido.
                    if (IsIconic(windows[g_selectedIndex].hwnd)) { // Si la ventana está minimizada.
                        ShowWindow(windows[g_selectedIndex].hwnd, SW_RESTORE); // La restauramos.
                    }
                    AllowSetForegroundWindow(ASFW_ANY); // Permitimos que cualquier ventana vaya al frente.
                    if (!SetForegroundWindow(windows[g_selectedIndex].hwnd)) { // Si no pudimos ponerla al frente.
                        SwitchToThisWindow(windows[g_selectedIndex].hwnd, TRUE); // Usamos el método alternativo.
                    }
                }
                ClearSearchBox();
                ShowWindow(hwnd, SW_HIDE); // Ocultamos la superposición.
                g_lastHotkey = 0; // Reseteamos el último atajo.
                KillTimer(hwnd, 100); // Limpiamos el timer de Alt+Q.
                return 0; // No procesamos más.
            }
            // Manejo adicional para Alt+Q: si se suelta Q mientras Alt está presionado.
            else if (IsWindowVisible(hwnd) && g_lastHotkey == 2 && wParam == 'Q') { // Si la superposición está abierta y fue Alt+Q y se soltó Q.
                // Verificamos si Alt sigue presionado.
                if (GetAsyncKeyState(VK_MENU) & 0x8000) { // Si Alt sigue presionado.
                    // Enfocamos la ventana seleccionada antes de ocultarla.
                    auto& windows = g_windows; // Referencia a la lista de ventanas.
                    int n = (int)windows.size(); // Cantidad de ventanas.
                    if (n > 0 && g_selectedIndex >= 0 && g_selectedIndex < n) { // Si hay ventanas y el índice es válido.
                        if (IsIconic(windows[g_selectedIndex].hwnd)) { // Si la ventana está minimizada.
                            ShowWindow(windows[g_selectedIndex].hwnd, SW_RESTORE); // La restauramos.
                        }
                        AllowSetForegroundWindow(ASFW_ANY); // Permitimos que cualquier ventana vaya al frente.
                        if (!SetForegroundWindow(windows[g_selectedIndex].hwnd)) { // Si no pudimos ponerla al frente.
                            SwitchToThisWindow(windows[g_selectedIndex].hwnd, TRUE); // Usamos el método alternativo.
                        }
                    }
                ClearSearchBox();
                    ShowWindow(hwnd, SW_HIDE); // Ocultamos la superposición.
                    g_lastHotkey = 0; // Reseteamos el último atajo.
                    KillTimer(hwnd, 100); // Limpiamos el timer de Alt+Q.
                    return 0; // No procesamos más.
                }
            }
            break;
        case WM_SYSKEYUP: // Mensaje cuando se suelta una tecla del sistema (como Alt).
            if (IsWindowVisible(hwnd) && g_lastHotkey == 2 && wParam == VK_MENU) { // Si la superposición está abierta y fue Alt+Q y se soltó Alt.
                // Enfocamos la ventana seleccionada antes de ocultarla.
                auto& windows = g_windows; // Referencia a la lista de ventanas.
                int n = (int)windows.size(); // Cantidad de ventanas.
                if (n > 0 && g_selectedIndex >= 0 && g_selectedIndex < n) { // Si hay ventanas y el índice es válido.
                    if (IsIconic(windows[g_selectedIndex].hwnd)) { // Si la ventana está minimizada.
                        ShowWindow(windows[g_selectedIndex].hwnd, SW_RESTORE); // La restauramos.
                    }
                    AllowSetForegroundWindow(ASFW_ANY); // Permitimos que cualquier ventana vaya al frente.
                    if (!SetForegroundWindow(windows[g_selectedIndex].hwnd)) { // Si no pudimos ponerla al frente.
                        SwitchToThisWindow(windows[g_selectedIndex].hwnd, TRUE); // Usamos el método alternativo.
                    }
                }
                ClearSearchBox();
                ShowWindow(hwnd, SW_HIDE); // Ocultamos la superposición.
                g_lastHotkey = 0; // Reseteamos el último atajo.
                KillTimer(hwnd, 100); // Limpiamos el timer de Alt+Q.
                return 0; // No procesamos más.
            }
            // Si fue Alt+A (modo persistente), no cerrar el overlay cuando se suelta Alt
            if (IsWindowVisible(hwnd) && g_lastHotkey == 3 && wParam == VK_MENU) {
                // En modo persistente, solo resetear el estado pero mantener el overlay abierto
                g_lastHotkey = 0;
                return 0;
            }
            break;
            
        case WM_MOUSEWHEEL: { // Mensaje cuando se mueve la rueda del mouse.
            int delta = GET_WHEEL_DELTA_WPARAM(wParam); // Obtenemos el delta de la rueda.
            if (g_scrollMaxY > 0) { // Si hay scroll vertical disponible.
                g_scrollY -= delta / 2; // Movemos el scroll vertical.
                g_scrollY = max(0, min(g_scrollY, g_scrollMaxY)); // Limitamos el scroll entre 0 y el máximo.
                InvalidateGrid(hwnd); // Redibujamos la grilla.
            } else if (g_scrollMax > 0) { // Si hay scroll horizontal disponible.
                g_scrollX -= delta / 2; // Movemos el scroll horizontal.
                g_scrollX = max(0, min(g_scrollX, g_scrollMax)); // Limitamos el scroll entre 0 y el máximo.
                InvalidateGrid(hwnd); // Redibujamos la grilla.
            }
            return 0; // No procesamos más.
        }
        
        case WM_LBUTTONDOWN: { // Mensaje cuando se presiona el botón izquierdo del mouse.
            int x = GET_X_LPARAM(lParam); // Coordenada X del clic.
            int y = GET_Y_LPARAM(lParam); // Coordenada Y del clic.
            RECT clientRect; // Rectángulo del área cliente.
            GetClientRect(hwnd, &clientRect); // Obtenemos el área cliente.
            
            auto& windows = g_windows; // Referencia a la lista de ventanas.
            SortWindowsForGrid(windows); // Ordenamos las ventanas para la grilla.
            int n = (int)windows.size(); // Cantidad de ventanas.
            // Calculamos el área de la cuadrícula (restamos el relleno de ambos lados).
            int gridAreaW = clientRect.right - 2 * GRID_PADDING_X; // Ancho del área de la grilla.
            int gridAreaH = clientRect.bottom - 2 * GRID_PADDING_Y; // Alto del área de la grilla.
            int cols = g_fixedCols; // Número fijo de columnas.
            int rows = (n + cols - 1) / cols; // Calculamos las filas necesarias.
            
            int gridW = cols * g_previewW + (cols - 1) * PREVIEW_MARGIN; // Ancho total de la grilla.
            int gridH = rows * g_previewH + (rows - 1) * PREVIEW_MARGIN; // Alto total de la grilla.
            
            int startX = GRID_PADDING_X - g_scrollX; // Posición X de inicio considerando el scroll.
            int startY = GRID_PADDING_Y - g_scrollY; // Posición Y de inicio considerando el scroll.
            
            g_scrollMax = (gridW > gridAreaW) ? (gridW - gridAreaW) : 0; // Máximo scroll horizontal.
            g_scrollMax = 0; // Force no horizontal scroll since we sized to fit
            g_scrollMaxY = (gridH > gridAreaH) ? (gridH - gridAreaH) : 0; // Máximo scroll vertical.
            
            if (g_scrollMax > 0 && y > clientRect.bottom - SCROLLBAR_HEIGHT) { // Si hay scroll horizontal y el clic está en la barra de scroll.
                g_scrolling = true; // Activamos el scroll.
                g_scrollStartX = x; // Guardamos la posición inicial del clic.
                g_scrollStartOffset = g_scrollX; // Guardamos el offset inicial del scroll.
                SetCapture(hwnd); // Capturamos el mouse para seguir el movimiento.
                return 0; // No procesamos más.
            }
            if (g_scrollMaxY > 0 && x > clientRect.right - SCROLLBAR_HEIGHT) { // Si hay scroll vertical y el clic está en la barra de scroll vertical.
                g_vscrolling = true; // Activamos el scroll vertical.
                g_scrollStartY = y; // Guardamos la posición inicial del clic.
                g_scrollStartOffsetY = g_scrollY; // Guardamos el offset inicial del scroll vertical.
                SetCapture(hwnd); // Capturamos el mouse para seguir el movimiento.
                return 0; // No procesamos más.
            }
            
            // Buscamos en qué miniatura se hizo clic.
            for (int i = 0; i < n; ++i) { // Recorremos todas las ventanas.
                int row = i / cols; // Fila de la ventana.
                int col = i % cols; // Columna de la ventana.
                int windowX = startX + col * (g_previewW + PREVIEW_MARGIN); // Posición X de la ventana.
                int windowY = startY + row * (g_previewH + PREVIEW_MARGIN); // Posición Y de la ventana.
                
                if (x >= windowX && x < windowX + g_previewW && // Si el clic está dentro del ancho de la ventana.
                    y >= windowY && y < windowY + g_previewH) { // Si el clic está dentro del alto de la ventana.
                    
                    // Debug: Log that we're inside a window cell
                    wchar_t cellDebugMsg[256];
                    wsprintfW(cellDebugMsg, L"Click inside window %d cell: windowX=%d, windowY=%d, clickX=%d, clickY=%d\n", 
                             i, windowX, windowY, x, y);
                    OutputDebugStringW(cellDebugMsg);
                    
                    // Verificamos si el clic fue en el botón de pin (toggle pin/unpin).
                    // Debug: Log dynamic order state
                    wchar_t orderDebugMsg[256];
                    wsprintfW(orderDebugMsg, L"g_dynamicOrder: %s\n", g_dynamicOrder ? L"true" : L"false");
                    OutputDebugStringW(orderDebugMsg);
                    if (!g_dynamicOrder) { // Solo en modo persistente
                        // Account for cell inflation (same as in drawing code)
                        int inflation = (i == g_hoverIndex || i == g_selectedIndex) ? 4 : 0;
                        int pinX = windowX + g_previewW - 28 + inflation; // Posición X del botón de pin.
                        int pinY = windowY + 8 + inflation; // Posición Y del botón de pin.
                        
                        // Debug: Log pin button coordinates and click position
                        wchar_t pinDebugMsg[256];
                        wsprintfW(pinDebugMsg, L"Pin button for window %d: pinX=%d, pinY=%d, clickX=%d, clickY=%d, inflation=%d\n", 
                                 i, pinX, pinY, x, y, inflation);
                        OutputDebugStringW(pinDebugMsg);
                        
                        if (x >= pinX && x < pinX + 22 && y >= pinY && y < pinY + 22) { // Si el clic fue en el botón de pin.
                            // Debug: Log successful pin button click
                            wchar_t successDebugMsg[256];
                            wsprintfW(successDebugMsg, L"Pin button clicked for window %d, pinned: %s\n", i, windows[i].pinned ? L"true" : L"false");
                            OutputDebugStringW(successDebugMsg);
                            // Cambiamos el estado de pin de la ventana.
                            windows[i].pinned = !windows[i].pinned; // Invertimos el estado de pin.
                            SaveGridOrder(windows); // Guardamos el nuevo orden.
                            LoadGridOrder(g_gridOrder); // Reload to in-memory list
                            ApplyGridOrder(windows, g_gridOrder); // Reapply ordering/pin states
                            InvalidateGrid(hwnd); // Redraw grid
                            return 0; // No procesamos más.
                        }
                    }
                    
                    // Verificamos si el clic fue en el botón de cerrar.
                    // Account for cell inflation (same as in drawing code)
                    int closeInflation = (i == g_hoverIndex || i == g_selectedIndex) ? 4 : 0;
                    int closeX = windowX + g_previewW - 28 + closeInflation; // Posición X del botón de cerrar.
                    int closeY;
                    if (!g_dynamicOrder) {
                        // En modo persistente, el botón de cerrar está debajo del pin
                        closeY = windowY + 8 + 22 + PIN_CLOSE_VERTICAL_GAP + closeInflation;
                    } else {
                        // En modo dinámico, el botón de cerrar está en la posición del pin
                        closeY = windowY + 8 + closeInflation;
                    }
                    if (x >= closeX && x < closeX + 14 && y >= closeY && y < closeY + 14) { // Si el clic fue en el botón de cerrar.
                        // Cerramos la ventana.
                        PostMessage(windows[i].hwnd, WM_CLOSE, 0, 0); // Enviamos mensaje para cerrar la ventana.
                        return 0; // No procesamos más.
                    }
                    
                    // Verificamos si el clic fue en el botón de pin a posición (#).
                    if (!g_dynamicOrder) { // Solo en modo persistente
                        // Account for cell inflation (same as in drawing code)
                        int pinToPosInflation = (i == g_hoverIndex || i == g_selectedIndex) ? 4 : 0;
                        int pinToPosX = windowX + g_previewW - 28 + pinToPosInflation; // Posición X del botón de pin a posición.
                        int pinToPosY = windowY + 8 + 22 + PIN_CLOSE_VERTICAL_GAP + 14 + PIN_CLOSE_VERTICAL_GAP + pinToPosInflation; // Posición Y del botón de pin a posición (debajo del close).
                        if (x >= pinToPosX && x < pinToPosX + PIN_TO_POS_BUTTON_SIZE && y >= pinToPosY && y < pinToPosY + PIN_TO_POS_BUTTON_SIZE) { // Si el clic fue en el botón de pin a posición.
                            // Mostramos el diálogo para ingresar la posición.
                            int newPos = ShowNumberInputDialog(hwnd, L"Enter position (1-999):"); // Mostramos el diálogo.
                            if (newPos > 0 && newPos <= n) { // Si se ingresó una posición válida.
                                // Movemos la ventana a la posición especificada.
                                auto windowToMove = windows[i]; // Guardamos la ventana a mover.
                                // Aseguramos que quede fijada en la posición elegida.
                                windowToMove.pinned = true;
                                windows.erase(windows.begin() + i); // La quitamos de su posición actual.
                                windows.insert(windows.begin() + newPos - 1, windowToMove); // La insertamos en la nueva posición.
                                g_selectedIndex = newPos - 1; // Actualizamos la selección.
                                g_hoverIndex = newPos - 1; // Actualizamos el hover.
                                SaveGridOrder(windows); // Guardamos el nuevo orden.
                                LoadGridOrder(g_gridOrder); // Reload into memory
                                ApplyGridOrder(windows, g_gridOrder); // Apply order
                                InvalidateGrid(hwnd); // Redraw grid
                                return 0; // No procesamos más.
                            }
                            return 0; // No procesamos más.
                        }
                    }
                    
                    // Si el clic fue en la miniatura, la seleccionamos y enfocamos.
                    g_selectedIndex = i; // Seleccionamos la ventana.
                    g_hoverIndex = i; // El hover va a la misma ventana.
                    
                    // Enfocamos la ventana seleccionada (incluso si está minimizada).
                    if (IsIconic(windows[i].hwnd)) { // Si la ventana está minimizada.
                        ShowWindow(windows[i].hwnd, SW_RESTORE); // La restauramos.
                    }
                    AllowSetForegroundWindow(ASFW_ANY); // Permitimos que cualquier ventana vaya al frente.
                    if (!SetForegroundWindow(windows[i].hwnd)) { // Si no pudimos ponerla al frente.
                        SwitchToThisWindow(windows[i].hwnd, TRUE); // Usamos el método alternativo.
                    }
                    
                    ShowWindow(hwnd, SW_HIDE); // Ocultamos la superposición.
                    InvalidateGrid(hwnd); // Redibujamos la grilla.
                    return 0; // No procesamos más.
                }
            }
            break;
        }
        
        case WM_MOUSEMOVE: { // Mensaje cuando se mueve el mouse.
            int x = GET_X_LPARAM(lParam); // Coordenada X del mouse.
            int y = GET_Y_LPARAM(lParam); // Coordenada Y del mouse.
            
            if (g_scrolling) { // Si estamos scrolleando horizontalmente.
                int dx = x - g_scrollStartX; // Diferencia en X desde el inicio.
                g_scrollX = g_scrollStartOffset - dx; // Calculamos el nuevo scroll.
                g_scrollX = max(0, min(g_scrollX, g_scrollMax)); // Limitamos el scroll.
                InvalidateGrid(hwnd); // Redibujamos la grilla.
                return 0; // No procesamos más.
            }
            
            if (g_vscrolling) { // Si estamos scrolleando verticalmente.
                int dy = y - g_scrollStartY; // Diferencia en Y desde el inicio.
                g_scrollY = g_scrollStartOffsetY - dy; // Calculamos el nuevo scroll.
                g_scrollY = max(0, min(g_scrollY, g_scrollMaxY)); // Limitamos el scroll.
                InvalidateGrid(hwnd); // Redibujamos la grilla.
                return 0; // No procesamos más.
            }
            
            // Detección de hover para los botones.
            auto& windows = g_windows; // Referencia a la lista de ventanas.
            SortWindowsForGrid(windows); // Ordenamos las ventanas para la grilla.
            int n = (int)windows.size(); // Cantidad de ventanas.
            int cols = g_fixedCols; // Número fijo de columnas.
            int gridAreaW = g_overlayWidth - 2 * GRID_PADDING_X; // Ancho del área de la grilla.
            int gridAreaH = g_overlayHeight - 2 * GRID_PADDING_Y; // Alto del área de la grilla.
            int rows = (n + cols - 1) / cols; // Calculamos las filas necesarias.
            int startX = GRID_PADDING_X - g_scrollX; // Posición X de inicio considerando el scroll.
            int startY = GRID_PADDING_Y - g_scrollY; // Posición Y de inicio considerando el scroll.
            int col = (x - startX) / (g_previewW + PREVIEW_MARGIN); // Columna donde está el mouse.
            int row = (y - startY) / (g_previewH + PREVIEW_MARGIN); // Fila donde está el mouse.
            int idx = row * cols + col; // Índice de la ventana.
            // Detección de hover para el botón de pin.
            int oldPinHover = g_pinHoverIndex; // Guardamos el hover anterior.
            g_pinHoverIndex = -1; // Reseteamos el hover.
            int oldCloseHover = g_closeHoverIndex; // Guardamos el hover anterior.
            g_closeHoverIndex = -1; // Reseteamos el hover.
            if (idx >= 0 && idx < n) { // Si el índice es válido.
                int cellX = startX + col * (g_previewW + PREVIEW_MARGIN); // Posición X de la celda.
                int cellY = startY + row * (g_previewH + PREVIEW_MARGIN); // Posición Y de la celda.
                const int pinSize = 22; // Tamaño del pin.
                const int closeSize = 14; // Tamaño del botón de cerrar.
                if (!g_dynamicOrder) { // Si no es orden dinámico.
                    // Detección de hover para el botón de pin.
                    RECT pinRect = { cellX + g_previewW - 28, cellY + 8, cellX + g_previewW - 28 + pinSize, cellY + 8 + pinSize }; // Rectángulo del botón de pin.
                    if (x >= pinRect.left && x < pinRect.right && y >= pinRect.top && y < pinRect.bottom) { // Si el mouse está sobre el botón.
                        g_pinHoverIndex = idx; // Activamos el hover.
                    }
                    
                    const int closeY = cellY + 8 + pinSize + PIN_CLOSE_VERTICAL_GAP; // Posición Y del botón de cerrar.
                    RECT closeRect = { cellX + g_previewW - 28, closeY, cellX + g_previewW - 28 + closeSize, closeY + closeSize }; // Rectángulo del botón de cerrar.
                    if (x >= closeRect.left && x < closeRect.right && y >= closeRect.top && y < closeRect.bottom) { // Si el mouse está sobre el botón.
                        g_closeHoverIndex = idx; // Activamos el hover.
                    }
                } else { // Si es orden dinámico.
                    RECT closeRect = { cellX + g_previewW - 28, cellY + 8, cellX + g_previewW - 28 + closeSize, cellY + 8 + closeSize }; // Rectángulo del botón de cerrar.
                    if (x >= closeRect.left && x < closeRect.right && y >= closeRect.top && y < closeRect.bottom) { // Si el mouse está sobre el botón.
                        g_closeHoverIndex = idx; // Activamos el hover.
                    }
                }
            }
            if (g_pinHoverIndex != oldPinHover || g_closeHoverIndex != oldCloseHover) InvalidateGrid(hwnd); // Si cambió el hover, redibujamos.

            // Detección de hover para el botón de fijar a posición.
            int oldPinToPosHover = g_pinToPosHoverIndex; // Guardamos el hover anterior.
            g_pinToPosHoverIndex = -1; // Reseteamos el hover.
            if (idx >= 0 && idx < n) { // Si el índice es válido.
                int cellX = startX + col * (g_previewW + PREVIEW_MARGIN); // Posición X de la celda.
                int cellY = startY + row * (g_previewH + PREVIEW_MARGIN); // Posición Y de la celda.
                const int pinSize = 22; // Tamaño del pin.
                const int closeSize = 14; // Tamaño del botón de cerrar.
                if (!g_dynamicOrder) { // Si no es orden dinámico.
                    int pinToPosY = cellY + 8 + pinSize + PIN_CLOSE_VERTICAL_GAP + closeSize + PIN_CLOSE_VERTICAL_GAP; // Posición Y del botón de pin a posición.
                    RECT pinToPosRect = { cellX + g_previewW - 28, pinToPosY, cellX + g_previewW - 28 + PIN_TO_POS_BUTTON_SIZE, pinToPosY + PIN_TO_POS_BUTTON_SIZE }; // Rectángulo del botón.
                    if (x >= pinToPosRect.left && x < pinToPosRect.right && y >= pinToPosRect.top && y < pinToPosRect.bottom) { // Si el mouse está sobre el botón.
                        g_pinToPosHoverIndex = idx; // Activamos el hover.
                    }
                }
            }
            if (g_pinToPosHoverIndex != oldPinToPosHover) InvalidateGrid(hwnd); // Si cambió el hover, redibujamos.
            return 0; // No procesamos más.
        }
        
        case WM_LBUTTONUP: // Mensaje cuando se suelta el botón izquierdo del mouse.
            if (g_scrolling || g_vscrolling) { // Si estábamos scrolleando.
                g_scrolling = false; // Desactivamos el scroll horizontal.
                g_vscrolling = false; // Desactivamos el scroll vertical.
                ReleaseCapture(); // Liberamos la captura del mouse.
                return 0; // No procesamos más.
            }
            return 0; // No procesamos más.
            
        case WM_RBUTTONDOWN: { // Mensaje cuando se presiona el botón derecho del mouse.
            int x = GET_X_LPARAM(lParam); // Coordenada X del clic.
            int y = GET_Y_LPARAM(lParam); // Coordenada Y del clic.
            RECT clientRect; // Rectángulo del área cliente.
            GetClientRect(hwnd, &clientRect); // Obtenemos el área cliente.
            auto& windows = g_windows; // Referencia a la lista de ventanas.
            SortWindowsForGrid(windows); // Ordenamos las ventanas para la grilla.
            int n = (int)windows.size(); // Cantidad de ventanas.
            int cols = g_fixedCols; // Número fijo de columnas.
            int gridAreaW = clientRect.right - 2 * GRID_PADDING_X; // Ancho del área de la grilla.
            int gridAreaH = clientRect.bottom - 2 * GRID_PADDING_Y; // Alto del área de la grilla.
            int rows = (n + cols - 1) / cols; // Calculamos las filas necesarias.
            int gridW = cols * g_previewW + (cols - 1) * PREVIEW_MARGIN;
            int gridH = rows * g_previewH + (rows - 1) * PREVIEW_MARGIN;
            int startX = GRID_PADDING_X - g_scrollX;
            int startY = GRID_PADDING_Y - g_scrollY;
            int col = (x - startX) / (g_previewW + PREVIEW_MARGIN);
            int row = (y - startY) / (g_previewH + PREVIEW_MARGIN);
            int idx = row * cols + col;
            if (idx >= 0 && idx < n) {
                int cellX = startX + col * (g_previewW + PREVIEW_MARGIN);
                int cellY = startY + row * (g_previewH + PREVIEW_MARGIN);
                RECT cellRect = { cellX, cellY, cellX + g_previewW, cellY + g_previewH };
                if (x >= cellRect.left && x < cellRect.right && y >= cellRect.top && y < cellRect.bottom) {
                    if (!g_dynamicOrder) {
                        HMENU hMenu = CreatePopupMenu();
                        AppendMenuW(hMenu, MF_STRING, 1, windows[idx].pinned ? L"Despin" : L"Pin");
                        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
                        AppendMenuW(hMenu, MF_STRING, 2, L"Exclude Process");
                        AppendMenuW(hMenu, MF_STRING, 3, L"Exclude Class");
                        POINT pt = { x, y };
                        ClientToScreen(hwnd, &pt);
                        int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);
                        DestroyMenu(hMenu);
                        if (cmd == 1) {
                            windows[idx].pinned = !windows[idx].pinned; // Solo cambia esta ventana
                            SaveGridOrder(windows);
                            LoadGridOrder(g_gridOrder);
                            ApplyGridOrder(windows, g_gridOrder);
                            InvalidateGrid(hwnd);
                        } else if (cmd == 2) {
                            // Excluir proceso
                            std::wstring processName = GetProcessName(windows[idx].hwnd);
                            if (!processName.empty()) {
                                AddExcludedProcess(processName);
                                InvalidateGrid(hwnd);
                            }
                        } else if (cmd == 3) {
                            // Excluir clase
                            AddExcludedClass(windows[idx].className);
                            InvalidateGrid(hwnd);
                        }
                    }
                }
            }
            return 0;
        }
        
        case WM_KEYDOWN: {
            // debug: log de todas las teclas presionadas cuando el overlay está visible
            if (IsWindowVisible(hwnd)) {
                wchar_t debugMsg[256];
                wsprintfW(debugMsg, L"WM_KEYDOWN received: wParam=%d, lParam=%08X\n", wParam, lParam);
                OutputDebugStringW(debugMsg);
            }
            
            // Manejo de numpad numbers para el foco de la ventana
            if (IsWindowVisible(hwnd) && wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9) {
                // debug: log de la tecla presionada
                wchar_t debugMsg[256];
                wsprintfW(debugMsg, L"Numpad key pressed: %d\n", wParam);
                OutputDebugStringW(debugMsg);
                
                int n = (int)g_windows.size();
                int digit = wParam - VK_NUMPAD0;
                int num = -1;
                if (digit == 0) {
                    num = 9;
                } else if (digit <= n) {
                    num = digit - 1;
                }
                if (num >= 0 && num < n) {
                    g_selectedIndex = num;
                    g_hoverIndex = num;
                    InvalidateGrid(hwnd);
                    AllowSetForegroundWindow(ASFW_ANY);
                    if (IsIconic(g_windows[num].hwnd)) {
                        ShowWindow(g_windows[num].hwnd, SW_RESTORE);
                    }
                    if (!SetForegroundWindow(g_windows[num].hwnd)) {
                        SwitchToThisWindow(g_windows[num].hwnd, TRUE);
                    }
                    ShowWindow(hwnd, SW_HIDE);
                    KillTimer(hwnd, 100);
                    return 0;
                }
            }
            // Solo permitir navegación con flechas si Alt está presionado
            if (IsWindowVisible(hwnd) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
                auto& windows = g_windows;
                SortWindowsForGrid(windows);
                int n = (int)windows.size();
                if (n > 0) {
                    int cols = g_fixedCols;
                    int rows = (n + cols - 1) / cols;
                    int row = g_selectedIndex / cols;
                    int col = g_selectedIndex % cols;
                    int gridAreaW = g_overlayWidth - 2 * GRID_PADDING_X;
                    int gridAreaH = g_overlayHeight - 2 * GRID_PADDING_Y;
                    int gridW = cols * g_previewW + (cols - 1) * PREVIEW_MARGIN;
                    int gridH = rows * g_previewH + (rows - 1) * PREVIEW_MARGIN;
                    int startX = GRID_PADDING_X - g_scrollX;
                    int startY = GRID_PADDING_Y - g_scrollY;
                    auto scrollToSelected = [&]() {
                        int selX = startX + col * (g_previewW + PREVIEW_MARGIN);
                        int selY = startY + row * (g_previewH + PREVIEW_MARGIN);
                        // Horizontal
                        if (selX < GRID_PADDING_X) g_scrollX = max(0, g_scrollX - (GRID_PADDING_X - selX));
                        else if (selX + g_previewW > g_overlayWidth - GRID_PADDING_X) g_scrollX = min(g_scrollMax, g_scrollX + (selX + g_previewW - (g_overlayWidth - GRID_PADDING_X)));
                        // Vertical
                        if (selY < GRID_PADDING_Y) g_scrollY = max(0, g_scrollY - (GRID_PADDING_Y - selY));
                        else if (selY + g_previewH > g_overlayHeight - GRID_PADDING_Y) g_scrollY = min(g_scrollMaxY, g_scrollY + (selY + g_previewH - (g_overlayHeight - GRID_PADDING_Y)));
                    };
                    if (wParam == VK_TAB) {
                        if (GetKeyState(VK_SHIFT) & 0x8000) {
                            g_selectedIndex = (g_selectedIndex - 1 + n) % n;
                        } else {
                            g_selectedIndex = (g_selectedIndex + 1) % n;
                        }
                        g_hoverIndex = g_selectedIndex;
                        scrollToSelected();
                        InvalidateGrid(hwnd);
                        return 0;
                    } else if (wParam == VK_RETURN) {
                        if (g_selectedIndex >= 0 && g_selectedIndex < n) {
                            SetForegroundWindow(windows[g_selectedIndex].hwnd);
                            ShowWindow(hwnd, SW_HIDE);
                            KillTimer(hwnd, 100);
                        }
                        return 0;
                    } else if (wParam == VK_LEFT) {
                        if (col > 0) g_selectedIndex--;
                        else if (row > 0) g_selectedIndex = (row - 1) * cols + (cols - 1);
                        else g_selectedIndex = n - 1; // Circular: si es el primero, va al último
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        scrollToSelected();
                        InvalidateGrid(hwnd);
                        return 0;
                    } else if (wParam == VK_RIGHT) {
                        if (col < cols - 1 && g_selectedIndex + 1 < n) g_selectedIndex++;
                        else if (row < rows - 1) g_selectedIndex = (row + 1) * cols;
                        else g_selectedIndex = 0; // Circular: si es el último, va al primero
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        scrollToSelected();
                        InvalidateGrid(hwnd);
                        return 0;
                    } else if (wParam == VK_UP) {
                        if (row > 0) g_selectedIndex -= cols;
                        else g_selectedIndex = (rows - 1) * cols + col; // Wrap-around: ir a la última fila, misma columna
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        scrollToSelected();
                        InvalidateGrid(hwnd);
                        return 0;
                    } else if (wParam == VK_DOWN) {
                        if (row < rows - 1 && g_selectedIndex + cols < n) g_selectedIndex += cols;
                        else g_selectedIndex = col; // Wrap-around: ir a la primera fila, misma columna
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        scrollToSelected();
                        InvalidateGrid(hwnd);
                        return 0;
                    }
                }
            }
            // Si no está Alt presionado, ignorar las flechas
            if (wParam == VK_ESCAPE) {
                // Si estamos en modo filtro persistente (Alt+A), cerrar el overlay
                if (g_persistentFilterMode) {
                    g_persistentFilterMode = false;
                    g_filteringEnabled = false;
                    g_filterText.clear();
                    ApplyFilters();
                ShowWindow(hwnd, SW_HIDE);
                    g_lastHotkey = 0;
                KillTimer(hwnd, 100);
                KillTimer(hwnd, 200);
                return 0;
            }
                ShowWindow(hwnd, SW_HIDE);
                KillTimer(hwnd, 100);
                KillTimer(hwnd, 200);
                return 0;
            }
            // Alterna el orden dinámico/persistente con F2 - REMOVIDO
            // if (wParam == VK_F2) {
            //     g_dynamicOrder = !g_dynamicOrder;
            //     SetWindowTextW(hwnd, g_dynamicOrder ? L"BetterAltTab_Unnamed10110 [Dynamic Order]" : L"BetterAltTab_Unnamed10110 [PERSISTENT Z-ORDER MODE]");
            //     InvalidateGrid(hwnd);
            //     return 0;
            // }
            // Alterna el filtrado con F3
            if (wParam == VK_F3) {
                ToggleFiltering();
                InvalidateGrid(hwnd);
                return 0;
            }
            // Activar filtro rápido con Ctrl+F
            if (wParam == 'F' && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                if (!g_quickFilterActive) {
                    g_quickFilterActive = true;
                    g_filteringEnabled = true;
                    
                    // Crear control de entrada del filtro rápido
                    g_quickFilterEdit = CreateWindowW(L"EDIT", g_filterText.c_str(), 
                        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
                        10, 40, 200, 25, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
                    
                    // Configurar fuente y estilo
                    HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
                    SendMessage(g_quickFilterEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
                    
                    SetFocus(g_quickFilterEdit);
                    SendMessage(g_quickFilterEdit, EM_SETSEL, 0, -1);
                }
                return 0;
            }
            
            // Navegación con flechas en modo filtro persistente
            if (g_persistentFilterMode && g_quickFilterEdit) {
                auto& windows = g_filteringEnabled && !g_filterText.empty() ? g_filteredWindows : g_windows;
                int n = (int)windows.size();
                if (n > 0) {
                    int cols = g_fixedCols;
                    int rows = (n + cols - 1) / cols;
                    int row = g_selectedIndex / cols;
                    int col = g_selectedIndex % cols;
                    
                    if (wParam == VK_LEFT) {
                        if (col > 0) g_selectedIndex--;
                        else if (row > 0) g_selectedIndex = (row - 1) * cols + (cols - 1);
                        else g_selectedIndex = n - 1; // Circular: si es el primero, va al último
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        InvalidateGrid(hwnd);
                        return 0;
                    } else if (wParam == VK_RIGHT) {
                        if (col < cols - 1 && g_selectedIndex + 1 < n) g_selectedIndex++;
                        else if (row < rows - 1) g_selectedIndex = (row + 1) * cols;
                        else g_selectedIndex = 0; // Circular: si es el último, va al primero
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        InvalidateGrid(hwnd);
                        return 0;
                    } else if (wParam == VK_UP) {
                        if (row > 0) g_selectedIndex -= cols;
                        else g_selectedIndex = (rows - 1) * cols + col; // Wrap-around: ir a la última fila, misma columna
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        InvalidateGrid(hwnd);
                        return 0;
                    } else if (wParam == VK_DOWN) {
                        if (row < rows - 1 && g_selectedIndex + cols < n) g_selectedIndex += cols;
                        else g_selectedIndex = col; // Wrap-around: ir a la primera fila, misma columna
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        InvalidateGrid(hwnd);
                        return 0;
                    }
                }
            }
            break;
        }
        
        case WM_SYSKEYDOWN: {
            // Permitir navegación con flechas si Alt está presionado (igual que en WM_KEYDOWN)
            if (IsWindowVisible(hwnd) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
                auto& windows = g_windows;
                SortWindowsForGrid(windows);
                int n = (int)windows.size();
                if (n > 0) {
                    int cols = g_fixedCols;
                    int rows = (n + cols - 1) / cols;
                    int row = g_selectedIndex / cols;
                    int col = g_selectedIndex % cols;
                    int gridAreaW = g_overlayWidth - 2 * GRID_PADDING_X;
                    int gridAreaH = g_overlayHeight - 2 * GRID_PADDING_Y;
                    int gridW = cols * g_previewW + (cols - 1) * PREVIEW_MARGIN;
                    int gridH = rows * g_previewH + (rows - 1) * PREVIEW_MARGIN;
                    int startX = GRID_PADDING_X - g_scrollX;
                    int startY = GRID_PADDING_Y - g_scrollY;
                    auto scrollToSelected = [&]() {
                        int selX = startX + col * (g_previewW + PREVIEW_MARGIN);
                        int selY = startY + row * (g_previewH + PREVIEW_MARGIN);
                        if (selX < GRID_PADDING_X) g_scrollX = max(0, g_scrollX - (GRID_PADDING_X - selX));
                        else if (selX + g_previewW > g_overlayWidth - GRID_PADDING_X) g_scrollX = min(g_scrollMax, g_scrollX + (selX + g_previewW - (g_overlayWidth - GRID_PADDING_X)));
                        if (selY < GRID_PADDING_Y) g_scrollY = max(0, g_scrollY - (GRID_PADDING_Y - selY));
                        else if (selY + g_previewH > g_overlayHeight - GRID_PADDING_Y) g_scrollY = min(g_scrollMaxY, g_scrollY + (selY + g_previewH - (g_overlayHeight - GRID_PADDING_Y)));
                    };
                    if (wParam == VK_LEFT) {
                        if (col > 0) g_selectedIndex--;
                        else if (row > 0) g_selectedIndex = (row - 1) * cols + (cols - 1);
                        else g_selectedIndex = n - 1; // Circular: si es el primero, va al último
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        scrollToSelected();
                        InvalidateGrid(hwnd);
                        return 0;
                    } else if (wParam == VK_RIGHT) {
                        if (col < cols - 1 && g_selectedIndex + 1 < n) g_selectedIndex++;
                        else if (row < rows - 1) g_selectedIndex = (row + 1) * cols;
                        else g_selectedIndex = 0; // Circular: si es el último, va al primero
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        scrollToSelected();
                        InvalidateGrid(hwnd);
                        return 0;
                    } else if (wParam == VK_UP) {
                        if (row > 0) g_selectedIndex -= cols;
                        else g_selectedIndex = (rows - 1) * cols + col; // Wrap-around: ir a la última fila, misma columna
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        scrollToSelected();
                        InvalidateGrid(hwnd);
                        return 0;
                    } else if (wParam == VK_DOWN) {
                        if (row < rows - 1 && g_selectedIndex + cols < n) g_selectedIndex += cols;
                        else g_selectedIndex = col;
                        g_selectedIndex = (g_selectedIndex + n) % n;
                        g_hoverIndex = g_selectedIndex;
                        scrollToSelected();
                        InvalidateGrid(hwnd);
                        return 0;
                    }
                }
            }
            
            // Si se presiona Alt en modo filtro persistente, activar la ventana seleccionada
            if (g_persistentFilterMode && g_quickFilterEdit && wParam == VK_MENU) {
                // Activar la ventana seleccionada (usar ventanas filtradas si hay filtro activo)
                auto& windows = g_filteringEnabled && !g_filterText.empty() ? g_filteredWindows : g_windows;
                int n = (int)windows.size();
                
                // Si no hay ventanas filtradas, no hacer nada
                if (n == 0) {
                    return 0;
                }
                
                // Asegurar que el índice seleccionado esté dentro de los límites
                int selectedIndex = (g_selectedIndex >= 0 && g_selectedIndex < n) ? g_selectedIndex : 0;
                
                if (IsIconic(windows[selectedIndex].hwnd)) {
                    ShowWindow(windows[selectedIndex].hwnd, SW_RESTORE);
                }
                AllowSetForegroundWindow(ASFW_ANY);
                if (!SetForegroundWindow(windows[selectedIndex].hwnd)) {
                    SwitchToThisWindow(windows[selectedIndex].hwnd, TRUE);
                }
                
                // Cerrar el overlay
                ClearSearchBox();
                ShowWindow(hwnd, SW_HIDE);
                g_lastHotkey = 0;
                KillTimer(hwnd, 100);
                KillTimer(hwnd, 200);
                return 0;
            }
            break;
        }
        
     
        case WM_CHAR:
    if (IsWindowVisible(hwnd)) {
        int n = (int)g_windows.size();
        // Remover el manejo de numpad number desde aquí
        if (wParam == VK_ESCAPE) {
            // Si el filtro rápido está activo, cerrarlo primero
            if (g_quickFilterActive) {
                DestroyWindow(g_quickFilterEdit);
                g_quickFilterEdit = nullptr;
                g_quickFilterActive = false;
                InvalidateGrid(hwnd);
                return 0;
            }
            // Si estamos en modo filtro persistente, cerrar el overlay
            if (g_persistentFilterMode) {
                g_persistentFilterMode = false;
                g_filteringEnabled = false;
                g_filterText.clear();
                g_placeholderActive = false;
                if (g_quickFilterEdit) {
                    DestroyWindow(g_quickFilterEdit);
                    g_quickFilterEdit = nullptr;
                }
                ApplyFilters();
            ShowWindow(hwnd, SW_HIDE);
                g_lastHotkey = 0;
            KillTimer(hwnd, 100);
            KillTimer(hwnd, 200);
            return 0;
        }
            ClearSearchBox();
            ShowWindow(hwnd, SW_HIDE);
            KillTimer(hwnd, 100);
            KillTimer(hwnd, 200);
            return 0;
        }
        // Si se presiona Enter en el filtro rápido, aplicar el filtro
        if (wParam == VK_RETURN && g_quickFilterActive) {
            wchar_t filterText[256];
            GetWindowTextW(g_quickFilterEdit, filterText, 256);
            g_filterText = filterText;
            SetFilterText(g_filterText);
            
            // Cerrar el filtro rápido
            DestroyWindow(g_quickFilterEdit);
            g_quickFilterEdit = nullptr;
            g_quickFilterActive = false;
            
            InvalidateGrid(hwnd);
            return 0;
        }
        // Si se presiona Enter en el filtro persistente, aplicar el filtro y activar la ventana seleccionada
        if (wParam == VK_RETURN && g_persistentFilterMode && g_quickFilterEdit) {
            wchar_t filterText[256];
            GetWindowTextW(g_quickFilterEdit, filterText, 256);
            g_filterText = filterText;
            SetFilterText(g_filterText);
            
            // Re-registrar miniaturas solo para las ventanas filtradas
            if (g_filteringEnabled && !g_filterText.empty()) {
                // Limpiar miniaturas anteriores
                UnregisterAllThumbnails();
                // Registrar miniaturas solo para las ventanas filtradas
                if (!g_filteredWindows.empty()) {
                    RegisterThumbnails(hwnd, g_filteredWindows);
                }
            }
            
            InvalidateGrid(hwnd);
            
            // Activar la ventana seleccionada (usar ventanas filtradas si hay filtro activo)
            auto& windows = g_filteringEnabled && !g_filterText.empty() ? g_filteredWindows : g_windows;
            int n = (int)windows.size();
            
            // Si no hay ventanas filtradas, no hacer nada
            if (n == 0) {
                return 0;
            }
            
            // Asegurar que el índice seleccionado esté dentro de los límites
            int selectedIndex = (g_selectedIndex >= 0 && g_selectedIndex < n) ? g_selectedIndex : 0;
            
            if (IsIconic(windows[selectedIndex].hwnd)) {
                ShowWindow(windows[selectedIndex].hwnd, SW_RESTORE);
            }
            AllowSetForegroundWindow(ASFW_ANY);
            if (!SetForegroundWindow(windows[selectedIndex].hwnd)) {
                SwitchToThisWindow(windows[selectedIndex].hwnd, TRUE);
            }
            
            // Cerrar el overlay
            ClearSearchBox();
            ShowWindow(hwnd, SW_HIDE);
            g_lastHotkey = 0;
            KillTimer(hwnd, 100);
            KillTimer(hwnd, 200);
            return 0;
        }
        
        // Capturar teclas para el filtro persistente (sin tomar foco)
        if (g_persistentFilterMode && g_quickFilterEdit && wParam >= 32 && wParam < 127) {
            // Agregar el carácter al texto del filtro
            g_filterText += (wchar_t)wParam;
            SetWindowTextW(g_quickFilterEdit, g_filterText.c_str());
            
            // Aplicar filtros
            ApplyFilters();
            
            // Re-registrar miniaturas solo para las ventanas filtradas
            if (g_filteringEnabled && !g_filterText.empty()) {
                // Limpiar miniaturas anteriores
                UnregisterAllThumbnails();
                // Registrar miniaturas solo para las ventanas filtradas
                if (!g_filteredWindows.empty()) {
                    RegisterThumbnails(hwnd, g_filteredWindows);
                }
            }
            
            // Forzar actualización de miniaturas
            InvalidateGrid(hwnd);
            UpdateWindow(hwnd);
            
            return 0;
        }
        
        // Manejar backspace para el filtro persistente
        if (g_persistentFilterMode && g_quickFilterEdit && wParam == VK_BACK) {
            if (!g_filterText.empty()) {
                g_filterText.pop_back();
                SetWindowTextW(g_quickFilterEdit, g_filterText.c_str());
                
                // Aplicar filtros
                ApplyFilters();
                
                // Re-registrar miniaturas solo para las ventanas filtradas
                if (g_filteringEnabled && !g_filterText.empty()) {
                    // Limpiar miniaturas anteriores
                    UnregisterAllThumbnails();
                    // Registrar miniaturas solo para las ventanas filtradas
                    if (!g_filteredWindows.empty()) {
                        RegisterThumbnails(hwnd, g_filteredWindows);
                    }
                } else {
                    // Si no hay filtro, mostrar todas las ventanas
                    UnregisterAllThumbnails();
                    RegisterThumbnails(hwnd, g_windows);
                }
                
                // Forzar actualización de miniaturas
                InvalidateGrid(hwnd);
                UpdateWindow(hwnd);
            }
            return 0;
        }

    }
    return 0;
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            
            // Crear DC simple sin transparencia para mejor rendimiento
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBM = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
            HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);
            
            // Fondo sólido sin transparencia para mejor rendimiento
            HBRUSH bgBrush = CreateSolidBrush(OVERLAY_BG_COLOR);
            FillRect(memDC, &clientRect, bgBrush);
            DeleteObject(bgBrush);
            
            // Usar la lista correcta según el modo
            auto& windows = (g_persistentFilterMode && g_filteringEnabled) ? g_filteredWindows : g_windows;
            SortWindowsForGrid(windows);
            int n = (int)windows.size();
            int cols = g_fixedCols;
            int gridAreaW = clientRect.right - 2 * GRID_PADDING_X;
            int gridAreaH = clientRect.bottom - 2 * GRID_PADDING_Y;
            int rows = (n + cols - 1) / cols;
            int gridW = cols * g_previewW + (cols - 1) * PREVIEW_MARGIN; // Ancho total de la grilla.
            int gridH = rows * g_previewH + (rows - 1) * PREVIEW_MARGIN; // Alto total de la grilla.
            int startX = GRID_PADDING_X - g_scrollX;
            int startY = GRID_PADDING_Y - g_scrollY;
            g_scrollMax = (gridW > gridAreaW) ? (gridW - gridAreaW) : 0;
            g_scrollMaxY = (gridH > gridAreaH) ? (gridH - gridAreaH) : 0;
            int firstCol = max(0, g_scrollX / (g_previewW + PREVIEW_MARGIN));
            int lastCol = min(cols - 1, (g_scrollX + gridAreaW) / (g_previewW + PREVIEW_MARGIN));
            int firstRow = max(0, g_scrollY / (g_previewH + PREVIEW_MARGIN));
            int lastRow = min(rows - 1, (g_scrollY + gridAreaH) / (g_previewH + PREVIEW_MARGIN));
            std::vector<bool> visible(n, false);
            for (int r = firstRow; r <= lastRow; ++r) {
                for (int c = firstCol; c <= lastCol; ++c) {
                    int idx = r * cols + c;
                    if (idx >= n) continue;
                    int x = startX + c * (g_previewW + PREVIEW_MARGIN);
                    int y = startY + r * (g_previewH + PREVIEW_MARGIN);
                    if (x + g_previewW < GRID_PADDING_X || x > clientRect.right - GRID_PADDING_X ||
                        y + g_previewH < GRID_PADDING_Y || y > clientRect.bottom - GRID_PADDING_Y)
                        continue;
                    visible[idx] = true;
                }
            }
            RegisterVisibleThumbnails(hwnd, windows, visible);
            // Dibuja todas las miniaturas excepto la que se está arrastrando
            for (int r = firstRow; r <= lastRow; ++r) {
                for (int c = firstCol; c <= lastCol; ++c) {
                    int idx = r * cols + c;
                    if (idx >= n) continue;
                    if (g_dragging && idx == g_dragIndex) continue; // salta la arrastrada
                    int x = startX + c * (g_previewW + PREVIEW_MARGIN);
                    int y = startY + r * (g_previewH + PREVIEW_MARGIN);
                    if (x + g_previewW < GRID_PADDING_X || x > clientRect.right - GRID_PADDING_X ||
                        y + g_previewH < GRID_PADDING_Y || y > clientRect.bottom - GRID_PADDING_Y)
                        continue;
                    // Dibuja el espacio/resaltado en el destino de soltado
                    if (g_dragging && idx == g_hoverIndex) {
                        RECT gapRect = { x-4, y-4, x + g_previewW+4, y + g_previewH+4 };
                        HBRUSH gapBrush = CreateSolidBrush(RGB(0,255,68));
                        FrameRect(memDC, &gapRect, gapBrush);
                        DeleteObject(gapBrush);
                    }
                    RECT cellRect = { x, y, x + g_previewW, y + g_previewH };
                    int border = (idx == g_hoverIndex || idx == g_selectedIndex) ? 6 : (idx == g_dragIndex) ? 4 : 2;
                    COLORREF borderColor = (idx == g_selectedIndex) ? HIGHLIGHT_COLOR : (idx == g_hoverIndex) ? HOVER_BORDER_COLOR : (idx == g_dragIndex) ? DRAG_BORDER_COLOR : BORDER_COLOR;
                    if (idx == g_hoverIndex || idx == g_selectedIndex) {
                        InflateRect(&cellRect, 4, 4);
                    }
                    HBRUSH cellBrush = CreateSolidBrush(
                        idx == g_dragIndex ? RGB(40, 60, 80) :
                        (idx == g_hoverIndex || idx == g_selectedIndex) ? RGB(32, 48, 64) : 
                        RGB(20, 30, 40));
                    FillRect(memDC, &cellRect, cellBrush);
                    DeleteObject(cellBrush);
                    HPEN borderPen = CreatePen(PS_SOLID, border, borderColor);
                    HPEN oldPen = (HPEN)SelectObject(memDC, borderPen);
                    SelectObject(memDC, GetStockObject(NULL_BRUSH));
                    Rectangle(memDC, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom); // Rectángulo simple en lugar de redondeado
                    SelectObject(memDC, oldPen);
                    DeleteObject(borderPen);
                    // Dibuja el ícono de la ventana, el texto con sombra y la miniatura...
                    int iconSize = 18;
                    int titleAreaTop = cellRect.bottom - 32;
                    int iconY = titleAreaTop + (32 - iconSize) / 2;
                    DrawWindowIcon(memDC, windows[idx].hwnd, cellRect.left + 12, iconY, iconSize);
                    RECT textRect = cellRect;
                    textRect.left = cellRect.left + 12 + iconSize + 8;
                    textRect.top = cellRect.bottom - 32;
                    DrawTextWithShadow(memDC, windows[idx].title.c_str(), &textRect, TITLE_COLOR, 12);
                    if (windows[idx].thumbnail) {
                        DWM_THUMBNAIL_PROPERTIES props = {};
                        props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
                        RECT thumbRect = cellRect;
                        thumbRect.bottom -= 36;
                        thumbRect.left += 10;
                        thumbRect.right -= 10;
                        thumbRect.top += 10;
                        // Deja espacio para el ícono de pin en la esquina superior derecha
                        thumbRect.right -= 32; // reduce el ancho para el margen del pin
                        props.rcDestination = thumbRect;
                        props.fVisible = TRUE;
                        props.opacity = 255;
                        DwmUpdateThumbnailProperties(windows[idx].thumbnail, &props);
                    }
                    // Dibuja el ícono de pin sobre último para que sea siempre visible
                    const int pinSize = 22;
                    const int pinX = cellRect.right - 28;
                    const int pinY = cellRect.top + 8;
                    // En WM_PAINT, solo muestra el ícono de pin y el de cerrar debajo de él si el modo de orden persistente está activo
                    if (!g_dynamicOrder) {
                        const int closeSize = 14;
                        
                        // Dibuja el botón de pin con hover effect
                        bool pinHover = (idx == g_pinHoverIndex);
                        if (pinHover) {
                            HBRUSH hoverBrush = CreateSolidBrush(RGB(255, 255, 200));
                            RECT pinBg = { pinX - 2, pinY - 2, pinX + pinSize + 2, pinY + pinSize + 2 };
                            FillRect(memDC, &pinBg, hoverBrush);
                            DeleteObject(hoverBrush);
                        }
                        DrawPinIcon(memDC, pinX, pinY, windows[idx].pinned);
                        bool closeHover = (idx == g_closeHoverIndex);
                        const int closeY = pinY + pinSize + PIN_CLOSE_VERTICAL_GAP;
                        if (closeHover) {
                            HBRUSH hoverBrush = CreateSolidBrush(RGB(255, 200, 200));
                            RECT closeBg = { pinX - 2, closeY - 2, pinX + closeSize + 2, closeY + closeSize + 2 };
                            FillRect(memDC, &closeBg, hoverBrush);
                            DeleteObject(hoverBrush);
                        }
                        DrawCloseIcon(memDC, pinX, closeY, closeSize);
                        // Elimina DrawPinToPosButton, y en WM_PAINT, solo dibuja el ícono '#' para el botón de fijar a posición
                        int pinToPosY = closeY + closeSize + PIN_CLOSE_VERTICAL_GAP;
                        
                        // Dibuja el botón de pin a posición con hover effect
                        bool pinToPosHover = (idx == g_pinToPosHoverIndex);
                        if (pinToPosHover) {
                            HBRUSH hoverBrush = CreateSolidBrush(RGB(200, 255, 255));
                            RECT pinToPosBg = { pinX - 2, pinToPosY - 2, pinX + PIN_TO_POS_BUTTON_SIZE + 2, pinToPosY + PIN_TO_POS_BUTTON_SIZE + 2 };
                            FillRect(memDC, &pinToPosBg, hoverBrush);
                            DeleteObject(hoverBrush);
                        }
                        DrawPinToPosIcon(memDC, pinX, pinToPosY, PIN_TO_POS_BUTTON_SIZE);
                        // Dibuja el sobre de número debajo del botón #
                        int numberY = pinToPosY + PIN_TO_POS_BUTTON_SIZE + 4;
                        DrawNumberOverlay(memDC, pinX + PIN_TO_POS_BUTTON_SIZE/2 - (idx < 9 ? 6 : (idx < 99 ? 9 : 12)), 
                  numberY, idx + 1);

                    } else {
                        const int closeSize = 14;
                        bool closeHover = (idx == g_closeHoverIndex);
                        if (closeHover) {
                            HBRUSH hoverBrush = CreateSolidBrush(RGB(255, 200, 200));
                            RECT closeBg = { pinX - 2, pinY - 2, pinX + closeSize + 2, pinY + closeSize + 2 };
                            FillRect(memDC, &closeBg, hoverBrush);
                            DeleteObject(hoverBrush);
                        }
                        DrawCloseIcon(memDC, pinX, pinY, closeSize);
                    }
                    // Dibuja el sobre de número en la esquina superior izquierda de cada miniatura
                    DrawNumberOverlay(memDC, cellRect.left + 8, cellRect.top + 8, idx + 1);

                }
            }
            // Dibuja la miniatura arrastrada en la posición del mouse con una sombra
            if (g_dragging && g_dragIndex != -1 && g_dragIndex < n) {
                int dragX = g_dragMouseX - g_previewW / 2;
                int dragY = g_dragMouseY - g_previewH / 2;
                RECT cellRect = { dragX, dragY, dragX + g_previewW, dragY + g_previewH };
                // Dibuja la sombra
                RECT shadowRect = cellRect;
                OffsetRect(&shadowRect, 4, 8);
                HBRUSH shadowBrush = CreateSolidBrush(RGB(0,0,0));
                FillRect(memDC, &shadowRect, shadowBrush);
                DeleteObject(shadowBrush);
                // Dibuja la miniatura levantada
                int border = 8;
                COLORREF borderColor = HIGHLIGHT_COLOR;
                HBRUSH cellBrush = CreateSolidBrush(RGB(32, 48, 64));
                FillRect(memDC, &cellRect, cellBrush);
                DeleteObject(cellBrush);
                HPEN borderPen = CreatePen(PS_SOLID, border, borderColor);
                HPEN oldPen = (HPEN)SelectObject(memDC, borderPen);
                SelectObject(memDC, GetStockObject(NULL_BRUSH));
                RoundRect(memDC, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 40, 40);
                SelectObject(memDC, oldPen);
                DeleteObject(borderPen);
                int iconSize = 18;
                int titleAreaTop = cellRect.bottom - 32;
                int iconY = titleAreaTop + (32 - iconSize) / 2;
                DrawWindowIcon(memDC, windows[g_dragIndex].hwnd, cellRect.left + 12, iconY, iconSize);
                RECT textRect = cellRect;
                textRect.left = cellRect.left + 12 + iconSize + 8;
                textRect.top = cellRect.bottom - 32;
                DrawTextWithShadow(memDC, windows[g_dragIndex].title.c_str(), &textRect, TITLE_COLOR, 12);
                if (windows[g_dragIndex].thumbnail) {
                    DWM_THUMBNAIL_PROPERTIES props = {};
                    props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
                    RECT thumbRect = cellRect;
                    thumbRect.bottom -= 36;
                    thumbRect.left += 10;
                    thumbRect.right -= 10;
                    thumbRect.top += 10;
                    props.rcDestination = thumbRect;
                    props.fVisible = TRUE;
                    props.opacity = 255;
                    DwmUpdateThumbnailProperties(windows[g_dragIndex].thumbnail, &props);
                }
            }
            // Hace que las barras de desplazamiento sean invisibles (no las dibuja)
            // En WM_PAINT, después de todo el dibujo de cuadrícula y miniatura, pero antes de BitBlt, dibuja un contorno rojo si el modo de orden persistente está activo
            if (!g_dynamicOrder) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                int border = PERSISTENT_CONTOUR_THICKNESS;
                // Crea una región que coincida con la forma de la ventana (igual que SetWindowRgn)
                HRGN rgn = CreateRoundRectRgn(
                    rc.left + border/2, rc.top + border/2,
                    rc.right - border/2, rc.bottom - border/2,
                    20, 20);
                // Crea un pincel para el color del contorno
                HBRUSH borderBrush = CreateSolidBrush(RGB((PERSISTENT_CONTOUR_COLOR >> 16) & 0xFF, (PERSISTENT_CONTOUR_COLOR >> 8) & 0xFF, PERSISTENT_CONTOUR_COLOR & 0xFF));
                FrameRgn(memDC, rgn, borderBrush, border, border);
                DeleteObject(borderBrush);
                DeleteObject(rgn);
            }
            
            // Mostrar indicador de filtro activo (solo si no hay caja de búsqueda visible)
            if (g_filteringEnabled && !g_quickFilterEdit) {
                // Dibujar texto de estado del filtro en la esquina superior izquierda
                HFONT hFont = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                    OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
                if (hFont) {
                    HFONT oldFont = (HFONT)SelectObject(memDC, hFont);
                    SetBkMode(memDC, TRANSPARENT);
                    
                    wchar_t filterStatus[256];
                    if (g_persistentFilterMode) {
                        if (!g_filterText.empty()) {
                            swprintf_s(filterStatus, L"PERSISTENT MODE - Filter: %s (%d/%d windows) [Press ESC to close]", g_filterText.c_str(), (int)g_filteredWindows.size(), (int)g_allWindows.size());
                        } else {
                            swprintf_s(filterStatus, L"PERSISTENT MODE - Filters Active (%d/%d windows) [Press ESC to close]", (int)g_filteredWindows.size(), (int)g_allWindows.size());
                        }
                        SetTextColor(memDC, RGB(0, 255, 255)); // Cyan para modo persistente
                    } else {
                        if (!g_filterText.empty()) {
                            swprintf_s(filterStatus, L"Filter: %s (%d/%d windows)", g_filterText.c_str(), (int)g_filteredWindows.size(), (int)g_allWindows.size());
                        } else {
                            swprintf_s(filterStatus, L"Filters Active (%d/%d windows)", (int)g_filteredWindows.size(), (int)g_allWindows.size());
                        }
                        SetTextColor(memDC, RGB(255, 255, 0)); // Amarillo para filtros normales
                    }
                    
                    TextOutW(memDC, 10, 10, filterStatus, lstrlenW(filterStatus));
                    
                    SelectObject(memDC, oldFont);
                    DeleteObject(hFont);
                }
            }
            
                            
            
            // Usar BitBlt simple sin transparencia para mejor rendimiento
            BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, 
                   memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBM);
            DeleteObject(memBM);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
        

            
        case WM_CTLCOLORSTATIC:
            // Aplicar tema oscuro a las etiquetas estáticas
            SetBkMode((HDC)wParam, TRANSPARENT);
            SetTextColor((HDC)wParam, RGB(255, 255, 255));
            return (LRESULT)CreateSolidBrush(RGB(0, 0, 0));
        
        case WM_DESTROY:
            UnregisterThumbnails(g_windows);
            UninstallWindowHook(); // Desinstalar hook de ventanas cerradas
            PostQuitMessage(0);
            return 0;
            
        case WM_ACTIVATE:
            if (wParam == WA_INACTIVE) {
                // Si estamos en modo persistente, no cerrar el overlay
                if (g_persistentFilterMode) {
                    return 0;
                }
                ShowWindow(hwnd, SW_HIDE);
                KillTimer(hwnd, 100); // Limpiar timer de Alt+Q
                KillTimer(hwnd, 200); // Limpiar timer de numpad keys
                KillTimer(hwnd, 300); // Limpiar timer de verificación de ventanas
                return 0;
            }
            break;
        case WM_KILLFOCUS:
            // Cerrar filtro rápido si está activo
            if (g_quickFilterActive) {
                DestroyWindow(g_quickFilterEdit);
                g_quickFilterEdit = nullptr;
                g_quickFilterActive = false;
            }
            // Si estamos en modo persistente, no cerrar el overlay
            if (g_persistentFilterMode) {
                return 0;
            }
            ClearSearchBox();
            ShowWindow(hwnd, SW_HIDE);
            KillTimer(hwnd, 100); // Limpiar timer de Alt+Q
            KillTimer(hwnd, 200); // Limpiar timer de numpad keys
            KillTimer(hwnd, 300); // Limpiar timer de verificación de ventanas
            return 0;
        case WM_VIRTUAL_DESKTOP_CHANGED:
            // Recargar la lista de ventanas y la UI, pero NO recargar los datos persistentes
            InvalidateGrid(hwnd);
            break;
            
        case WM_USER + 1: // Mensaje personalizado para procesar foco de forma asíncrona
            // Procesar el foco sin bloquear el thread principal
            SetFocus(hwnd);
            return 0;
            
        case WM_TIMER:
            // Limpieza automática de rendimiento
            if (wParam == 300) { // Timer de limpieza automática
                CleanupThumbnailCache();
                MonitorMemoryUsage();
                CheckPerformanceMode();
                
                // Actualizar tooltip del tray con información de rendimiento
                wchar_t tooltipText[256];
                swprintf_s(tooltipText, L"BetterAltTab_Unnamed10110\nMemory: %d MB\nMode: %s", 
                    (int)(g_thumbnailMemoryUsage / (1024 * 1024)),
                    g_lowResourceMode ? L"Low Resource" : L"Normal");
                lstrcpyW(nid.szTip, tooltipText);
                Shell_NotifyIcon(NIM_MODIFY, &nid);
                
                return 0;
            }
            
            // Timer para verificar ventanas cerradas
            if (wParam == 300) {
                // Verificar si alguna ventana se cerró
                bool windowsChanged = false;
                
                // Verificar ventanas en g_windows
                for (auto it = g_windows.begin(); it != g_windows.end();) {
                    if (!IsWindow(it->hwnd)) {
                        // La ventana se cerró, removerla
                        if (it->thumbnail) {
                            DwmUnregisterThumbnail(it->thumbnail);
                            g_thumbnailMap.erase(it->hwnd);
                        }
                        it = g_windows.erase(it);
                        windowsChanged = true;
                    } else {
                        ++it;
                    }
                }
                
                // Verificar ventanas en g_filteredWindows
                for (auto it = g_filteredWindows.begin(); it != g_filteredWindows.end();) {
                    if (!IsWindow(it->hwnd)) {
                        // La ventana se cerró, removerla
                        if (it->thumbnail) {
                            DwmUnregisterThumbnail(it->thumbnail);
                            g_thumbnailMap.erase(it->hwnd);
                        }
                        it = g_filteredWindows.erase(it);
                        windowsChanged = true;
                    } else {
                        ++it;
                    }
                }
                
                // Verificar también si g_windowClosed está activo
                if (g_windowClosed) {
                    g_windowClosed = false; // Resetear el flag
                    windowsChanged = true;
                }
                
                // Si hubo cambios, actualizar la interfaz
                if (windowsChanged) {
                    // Ajustar índices de selección si es necesario
                    if (g_selectedIndex >= (int)g_windows.size()) {
                        g_selectedIndex = max(0, (int)g_windows.size() - 1);
                    }
                    if (g_hoverIndex >= (int)g_windows.size()) {
                        g_hoverIndex = max(0, (int)g_windows.size() - 1);
                    }
                    
                    // Redibujar la grilla inmediatamente si el overlay está visible
                    if (IsWindowVisible(hwnd)) {
                        InvalidateGrid(hwnd);
                        UpdateWindow(hwnd);
                    }
                }
                
                return 0;
            }
            
            // Timer adicional para verificación rápida de ventanas cerradas
            if (wParam == 400) {
                // Verificación más agresiva de ventanas cerradas
                bool windowsChanged = false;
                
                // Verificar solo las ventanas principales (más eficiente)
                for (auto it = g_windows.begin(); it != g_windows.end();) {
                    if (!IsWindow(it->hwnd)) {
                        // La ventana se cerró, removerla
                        if (it->thumbnail) {
                            DwmUnregisterThumbnail(it->thumbnail);
                            g_thumbnailMap.erase(it->hwnd);
                        }
                        it = g_windows.erase(it);
                        windowsChanged = true;
                    } else {
                        ++it;
                    }
                }
                
                // Si hubo cambios, actualizar la interfaz inmediatamente
                if (windowsChanged) {
                    // Ajustar índices de selección si es necesario
                    if (g_selectedIndex >= (int)g_windows.size()) {
                        g_selectedIndex = max(0, (int)g_windows.size() - 1);
                    }
                    if (g_hoverIndex >= (int)g_windows.size()) {
                        g_hoverIndex = max(0, (int)g_windows.size() - 1);
                    }
                    
                    // Redibujar la grilla inmediatamente si el overlay está visible
                    if (IsWindowVisible(hwnd)) {
                        InvalidateGrid(hwnd);
                        UpdateWindow(hwnd);
                    }
                }
                
                return 0;
            }
            
            // Timer para detectar cuando se suelta Alt después de Alt+Q
            if (wParam == 100 && g_lastHotkey == 2) {
                // Verificar si se presionó un numpad key usando GetAsyncKeyState
                for (int i = VK_NUMPAD0; i <= VK_NUMPAD9; i++) {
                    if (GetAsyncKeyState(i) & 0x8000) {
                        // se presiono un numpad key
                        int n = (int)g_windows.size();
                        int digit = i - VK_NUMPAD0;
                        int num = -1;
                        if (digit == 0) {
                            num = 9;
                        } else if (digit <= n) {
                            num = digit - 1;
                        }
                        if (num >= 0 && num < n) {
                            g_selectedIndex = num;
                            g_hoverIndex = num;
                            InvalidateGrid(hwnd);
                            AllowSetForegroundWindow(ASFW_ANY);
                            if (IsIconic(g_windows[num].hwnd)) {
                                ShowWindow(g_windows[num].hwnd, SW_RESTORE);
                            }
                            if (!SetForegroundWindow(g_windows[num].hwnd)) {
                                SwitchToThisWindow(g_windows[num].hwnd, TRUE);
                            }
                            ShowWindow(hwnd, SW_HIDE);
                            g_lastHotkey = 0;
                            KillTimer(hwnd, 100);
                            return 0;
                        }
                    }
                }
                
                // Verificar si Alt ya no está presionado
                if (!(GetAsyncKeyState(VK_MENU) & 0x8000)) {
                    // Enfoca la ventana seleccionada antes de ocultarla
                    auto& windows = g_windows;
                    int n = (int)windows.size();
                    if (n > 0 && g_selectedIndex >= 0 && g_selectedIndex < n) {
                        if (IsIconic(windows[g_selectedIndex].hwnd)) {
                            ShowWindow(windows[g_selectedIndex].hwnd, SW_RESTORE);
                        }
                        AllowSetForegroundWindow(ASFW_ANY);
                        if (!SetForegroundWindow(windows[g_selectedIndex].hwnd)) {
                            SwitchToThisWindow(windows[g_selectedIndex].hwnd, TRUE);
                        }
                    }
                    ShowWindow(hwnd, SW_HIDE);
                    g_lastHotkey = 0;
                    KillTimer(hwnd, 100);
                    return 0;
                }
            }
            // Timer para detectar teclas numpad cuando el overlay está visible
            else if (wParam == 200 && IsWindowVisible(hwnd)) {
                // Actualizar filtro en tiempo real si está activo
                if (g_quickFilterActive && g_quickFilterEdit) {
                    wchar_t filterText[256];
                    GetWindowTextW(g_quickFilterEdit, filterText, 256);
                    if (g_filterText != filterText) {
                        g_filterText = filterText;
                        SetFilterText(g_filterText);
                        InvalidateGrid(hwnd);
                    }
                }
                // Actualizar filtro en tiempo real si está en modo persistente
                if (g_persistentFilterMode && g_quickFilterEdit) {
                    wchar_t filterText[256];
                    GetWindowTextW(g_quickFilterEdit, filterText, 256);
                    if (g_filterText != filterText) {
                        g_filterText = filterText;
                        SetFilterText(g_filterText);
                        InvalidateGrid(hwnd);
                    }
                }
                // Verificar si se presionó Ctrl+número
                static bool ctrlWasDown = false;
                bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
                if (ctrlDown && !ctrlWasDown && !g_ctrlNumberInputActive) {
                    // Ctrl presionado: mostrar overlay
                    g_ctrlNumberInputActive = true;
                    g_ctrlNumberBuffer.clear();
                    ShowCtrlNumberOverlay(hwnd);
                }
                ctrlWasDown = ctrlDown;
                
                // Si el overlay de Ctrl está activo, solo mantenemos abierto (el input es manejado por el control de entrada)
                if (g_ctrlNumberInputActive && g_ctrlNumberOverlayHwnd) {
                    // El control de entrada maneja todo el input, así que no hacemos nada aquí
                }
                
                // Manejo de teclas numpad original (solo si el overlay de Ctrl no está activo)
                if (!g_ctrlNumberInputActive) {
                    for (int i = VK_NUMPAD0; i <= VK_NUMPAD9; i++) {
                        if (GetAsyncKeyState(i) & 0x8000) {
                            // se presiono un numpad key
                            int n = (int)g_windows.size();
                            int digit = i - VK_NUMPAD0;
                            int num = -1;
                            if (digit == 0) {
                                num = 9;
                            } else if (digit <= n) {
                                num = digit - 1;
                            }
                            if (num >= 0 && num < n) {
                                g_selectedIndex = num;
                                g_hoverIndex = num;
                                InvalidateGrid(hwnd);
                                AllowSetForegroundWindow(ASFW_ANY);
                                if (IsIconic(g_windows[num].hwnd)) {
                                    ShowWindow(g_windows[num].hwnd, SW_RESTORE);
                                }
                                if (!SetForegroundWindow(g_windows[num].hwnd)) {
                                    SwitchToThisWindow(g_windows[num].hwnd, TRUE);
                                }
                                ShowWindow(hwnd, SW_HIDE);
                                KillTimer(hwnd, 100);
                                KillTimer(hwnd, 200);
                                return 0;
                            }
                        }
                    }
                }
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Desregistrar todas las miniaturas de todas las ventanas (no solo las actuales)
void UnregisterAllThumbnails() {
    for (auto& pair : g_thumbnailMap) {
        if (pair.second) {
            DwmUnregisterThumbnail(pair.second);
        }
    }
    g_thumbnailMap.clear(); // Limpiar el mapa de miniaturas
    g_thumbnailMemoryUsage = 0; // Resetear uso de memoria
}

// ===============================
// Logging para depuración
// ===============================
// Mutex para escritura de logs (debug)
std::mutex g_logMutex;

// Esta función escribe mensajes de depuración en un archivo de texto.
// Es como un diario que guarda información para ayudarnos a encontrar errores.
void LogDebugMessage(const wchar_t* msg) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    std::wofstream logFile(L"debug_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << msg;
        logFile.flush();
    }
}

// ===============================
// Extra overlay para Ctrl+number input
// ===============================

// Declaración adelantada
LRESULT CALLBACK CtrlNumberOverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Procedimiento de subclase para el control de entrada
WNDPROC g_oldEditProc = nullptr;

// Esta función intercepta las teclas en el campo de entrada numérica.
// Es como un filtro que captura Enter y Escape antes de que lleguen al control normal.
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN) {
        if (wParam == VK_RETURN) {
            // Enter presionado - obtener texto y enfocar ventana
            wchar_t buffer[16];
            GetWindowTextW(hwnd, buffer, 16);
            g_ctrlNumberBuffer = buffer;
            
            // Debug: Log el número
            wchar_t debugMsg[256];
            wsprintfW(debugMsg, L"Enter pressed, number: %s\n", g_ctrlNumberBuffer.c_str());
            OutputDebugStringW(debugMsg);
            
            int n = (int)g_windows.size();
            int num = 0;
            if (!g_ctrlNumberBuffer.empty()) {
                try {
                    num = std::stoi(g_ctrlNumberBuffer);
                } catch (...) { num = 0; }
            }
            
            // Debug: Log el número parseado y la cantidad de ventanas
            wsprintfW(debugMsg, L"Parsed number: %d, total windows: %d\n", num, n);
            OutputDebugStringW(debugMsg);
            
            if (num >= 1 && num <= n) {
                int idx = num - 1;
                AllowSetForegroundWindow(ASFW_ANY);
                if (IsIconic(g_windows[idx].hwnd)) {
                    ShowWindow(g_windows[idx].hwnd, SW_RESTORE);
                }
                if (!SetForegroundWindow(g_windows[idx].hwnd)) {
                    SwitchToThisWindow(g_windows[idx].hwnd, TRUE);
                }
                
                // Debug: Log éxito
                OutputDebugStringW(L"Window focused successfully\n");
            } else {
                // Debug: Log fallo
                wsprintfW(debugMsg, L"Invalid number: %d (valid range: 1-%d)\n", num, n);
                OutputDebugStringW(debugMsg);
            }
            // Cerrar la ventana cuando se presiona Enter
            DestroyWindow(GetParent(hwnd));
            return 0;
        }
        else if (wParam == VK_ESCAPE) {
            // Escape presionado - cerrar la ventana
            DestroyWindow(GetParent(hwnd));
            return 0;
        }
    }
    return CallWindowProc(g_oldEditProc, hwnd, msg, wParam, lParam);
}

// Esta función muestra una ventana overlay para ingresar números con Ctrl.
// Es como un popup negro que aparece para que escribas el número de la ventana que querés.
void ShowCtrlNumberOverlay(HWND parent) {
    if (g_ctrlNumberOverlayHwnd) return;
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = CtrlNumberOverlayProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"CtrlNumberOverlayClass";
    wc.hbrBackground = nullptr;
    RegisterClassW(&wc);
    RECT rc;
    GetWindowRect(parent, &rc);
    int width = 80, height = 44; // Más pequeño
    int x = rc.left + ((rc.right - rc.left) - width) / 2;
    int y = rc.top + ((rc.bottom - rc.top) - height) / 2;
    g_ctrlNumberOverlayHwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED,
        wc.lpszClassName, L"",
        WS_POPUP | WS_BORDER,
        x, y, width, height,
        nullptr, nullptr, wc.hInstance, nullptr); // Sin ventana padre
    SetLayeredWindowAttributes(g_ctrlNumberOverlayHwnd, 0, 255, LWA_ALPHA); // Negro puro, completamente opaco
    
    // Bordes redondeados
    HRGN rgn = CreateRoundRectRgn(0, 0, width, height, 16, 16);
    SetWindowRgn(g_ctrlNumberOverlayHwnd, rgn, TRUE);
    
    // Crear control de entrada para entrada de número, centrado, texto blanco, fondo negro
    HWND hEdit = CreateWindowExW(
        0,
        L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER,
        4, 4, width-8, height-8,
        g_ctrlNumberOverlayHwnd, (HMENU)1001, wc.hInstance, nullptr);
    
    // Establecer fuente para el control de entrada
    HFONT hFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    // Establecer texto blanco, fondo negro
    SendMessage(hEdit, EM_SETSEL, 0, -1);
    SendMessage(hEdit, EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, MAKELPARAM(2,2));
    SendMessage(hEdit, EM_SETLIMITTEXT, 3, 0);
    // Subclase el control de entrada para manejar la tecla Enter
    g_oldEditProc = (WNDPROC)SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
    ShowWindow(g_ctrlNumberOverlayHwnd, SW_SHOW);
    SetForegroundWindow(g_ctrlNumberOverlayHwnd); // Traer al frente
    SetFocus(hEdit); // Foco en el control de entrada
    UpdateWindow(g_ctrlNumberOverlayHwnd);
}
// Esta función oculta la ventana overlay de entrada de números.
// Es como cerrar el popup cuando ya no lo necesitamos.
void HideCtrlNumberOverlay() {
    if (g_ctrlNumberOverlayHwnd) {
        DestroyWindow(g_ctrlNumberOverlayHwnd);
        g_ctrlNumberOverlayHwnd = nullptr;
    }
    g_ctrlNumberInputActive = false;
    g_ctrlNumberBuffer.clear();
}
// Esta función maneja los mensajes de la ventana overlay de entrada de números.
// Es como el cerebro de la ventana popup: maneja el dibujo, las teclas y los eventos.
LRESULT CALLBACK CtrlNumberOverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        // fondo negro puro, sin borde, sin prompt
        HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rc, bgBrush);
        DeleteObject(bgBrush);
        // dibujar borde blanco para visibilidad
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        RoundRect(hdc, 0, 0, rc.right, rc.bottom, 16, 16);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(pen);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam;
        SetBkColor(hdcEdit, RGB(0,0,0));
        SetTextColor(hdcEdit, RGB(255,255,255));
        static HBRUSH hBrush = NULL;
        if (!hBrush) hBrush = CreateSolidBrush(RGB(0,0,0));
        return (LRESULT)hBrush;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            // Cerrar la ventana cuando se presiona Escape
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    case WM_DESTROY:
        // Limpiar cuando la ventana se destruye
        g_ctrlNumberInputActive = false;
        g_ctrlNumberOverlayHwnd = nullptr;
        g_ctrlNumberBuffer.clear();
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1001 && HIWORD(wParam) == EN_UPDATE) {
            HWND hEdit = GetDlgItem(hwnd, 1001);
            if (hEdit) {
                wchar_t buffer[16];
                GetWindowTextW(hEdit, buffer, 16);
                g_ctrlNumberBuffer = buffer;
            }
        }
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// ============================================================
//  Implementación de funciones de optimización de rendimiento
// ============================================================

void LoadPerformanceSettings() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    wcscat_s(exePath, L"\\BetterAltTab.ini");
    
    // Cargar configuración de rendimiento
    g_cleanupInterval = GetPrivateProfileIntW(L"Performance", L"AutoCleanupInterval", 30000, exePath);
    g_maxThumbnailMemory = GetPrivateProfileIntW(L"Performance", L"MaxThumbnailMemory", 100, exePath) * 1024 * 1024;
    g_windowCacheTimeout = GetPrivateProfileIntW(L"Performance", L"WindowCacheTimeout", 5000, exePath);
    g_redrawThrottle = GetPrivateProfileIntW(L"Performance", L"NormalRefreshRate", 50, exePath);
    
    // Aplicar configuración inicial
    CheckPerformanceMode();
    OptimizeProcessPriority();
}

void CheckPerformanceMode() {
    MEMORYSTATUSEX memInfo = { sizeof(MEMORYSTATUSEX) };
    if (GlobalMemoryStatusEx(&memInfo)) {
        DWORDLONG availableMemory = memInfo.ullAvailPhys;
        DWORD memoryLoad = memInfo.dwMemoryLoad;
        
        // Activar modo de bajo recursos si la memoria disponible es baja
        g_lowResourceMode = (availableMemory < (512 * 1024 * 1024)) || (memoryLoad > 80);
        
        // Ajustar parámetros según el modo
        if (g_lowResourceMode) {
            g_cleanupInterval = 15000; // Limpieza más frecuente
            g_redrawThrottle = 100; // Redibujado más lento
        } else {
            g_cleanupInterval = 30000; // Limpieza normal
            g_redrawThrottle = 50; // Redibujado normal
        }
    }
}

void CleanupThumbnailCache() {
    DWORD currentTime = GetTickCount();
    
    // Solo limpiar si ha pasado suficiente tiempo
    if (currentTime - g_lastCleanupTime < g_cleanupInterval) {
        return;
    }
    
    // Limpiar miniaturas de ventanas que ya no existen
    std::vector<HWND> windowsToRemove;
    for (auto& pair : g_thumbnailMap) {
        if (!IsWindow(pair.first)) {
            if (pair.second) {
                DwmUnregisterThumbnail(pair.second);
                g_thumbnailMemoryUsage -= (g_previewW * g_previewH * 4); // Estimación de memoria
            }
            windowsToRemove.push_back(pair.first);
        }
    }
    
    // Remover entradas del mapa
    for (HWND hwnd : windowsToRemove) {
        g_thumbnailMap.erase(hwnd);
    }
    
    // Si el uso de memoria es muy alto, limpiar más agresivamente
    if (g_thumbnailMemoryUsage > g_maxThumbnailMemory) {
        // Mantener solo las miniaturas más recientes
        std::vector<std::pair<HWND, HTHUMBNAIL>> thumbnails;
        for (auto& pair : g_thumbnailMap) {
            thumbnails.push_back(pair);
        }
        
        // Ordenar por orden de uso (simplificado)
        if (thumbnails.size() > 10) {
            for (size_t i = 10; i < thumbnails.size(); ++i) {
                if (thumbnails[i].second) {
                    DwmUnregisterThumbnail(thumbnails[i].second);
                    g_thumbnailMemoryUsage -= (g_previewW * g_previewH * 4);
                }
                g_thumbnailMap.erase(thumbnails[i].first);
            }
        }
    }
    
    g_lastCleanupTime = currentTime;
}

void MonitorMemoryUsage() {
    // Verificar uso de memoria del proceso
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        // Si el proceso usa más de 200MB, activar modo de bajo recursos
        if (pmc.WorkingSetSize > (200 * 1024 * 1024)) {
            g_lowResourceMode = true;
            g_cleanupInterval = 10000; // Limpieza muy frecuente
        }
    }
}

bool ShouldRedraw() {
    DWORD currentTime = GetTickCount();
    if (currentTime - g_lastRedrawTime < g_redrawThrottle) {
        return false;
    }
    g_lastRedrawTime = currentTime;
    return true;
}

void OptimizeProcessPriority() {
    // Establecer prioridad del proceso
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    wcscat_s(exePath, L"\\BetterAltTab.ini");
    
    if (GetPrivateProfileIntW(L"Performance", L"OptimizePriority", 1, exePath)) {
        SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
    }
}

void ResetPerformanceCounters() {
    g_lastCleanupTime = 0;
    g_lastWindowEnumTime = 0;
    g_lastRedrawTime = 0;
    g_thumbnailMemoryUsage = 0;
    g_performanceMode = false;
    g_lowResourceMode = false;
}

void ConfigureFullPerformance() {
    // Configuración para máximo rendimiento en laptop con 16GB+ RAM
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    
    // Configuraciones de rendimiento máximo
    WritePrivateProfileStringW(L"Performance", L"LowResourceMode", L"off", exePath);
    WritePrivateProfileStringW(L"Performance", L"MaxThumbnailCache", L"50", exePath);
    WritePrivateProfileStringW(L"Performance", L"NormalRefreshRate", L"16", exePath);
    WritePrivateProfileStringW(L"Performance", L"LowResourceRefreshRate", L"50", exePath);
    WritePrivateProfileStringW(L"Performance", L"MinThumbnailSize", L"300", exePath);
    WritePrivateProfileStringW(L"Performance", L"MemoryThreshold", L"2048", exePath);
    WritePrivateProfileStringW(L"Performance", L"CPUThreshold", L"95", exePath);
    WritePrivateProfileStringW(L"Performance", L"ReserveMemory", L"true", exePath);
    WritePrivateProfileStringW(L"Performance", L"OptimizePriority", L"true", exePath);
    WritePrivateProfileStringW(L"Performance", L"ReduceVisualEffects", L"false", exePath);
    WritePrivateProfileStringW(L"Performance", L"AutoCleanupInterval", L"60000", exePath);
    WritePrivateProfileStringW(L"Performance", L"MaxThumbnailMemory", L"200", exePath);
    
    // Recargar configuración
    LoadPerformanceSettings();
    
    // Aplicar cambios inmediatamente
    g_lowResourceMode = false;
    g_cleanupInterval = 60000;
    g_maxThumbnailMemory = 200 * 1024 * 1024;
    g_windowCacheTimeout = 10000;
    
    // Limpiar cache y optimizar
    CleanupThumbnailCache();
    OptimizeProcessPriority();
    
    // Mostrar notificación
    MessageBoxW(NULL, L"Full Performance mode activated!\n\nConfiguration optimized for:\n- 16GB+ RAM systems\n- Maximum responsiveness\n- High quality thumbnails\n- Extended caching", L"BetterAltTab - Performance Mode", MB_OK | MB_ICONINFORMATION);
}

void ConfigureFullLowResources() {
    // Configuración para mínimo consumo de recursos
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    
    // Obtener información del sistema
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    
    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    DWORD memoryLoad = memInfo.dwMemoryLoad;
    
    // Configuraciones adaptativas según recursos disponibles
    int maxMemory = (int)(totalPhysMem / (1024 * 1024)); // RAM total en MB
    int maxCache = max(5, maxMemory / 100); // Cache proporcional a RAM
    int cleanupInterval = max(5000, memoryLoad * 100); // Limpieza más frecuente si hay poca RAM
    
    // Configuraciones de bajo consumo
    WritePrivateProfileStringW(L"Performance", L"LowResourceMode", L"on", exePath);
    WritePrivateProfileStringW(L"Performance", L"MaxThumbnailCache", std::to_wstring(maxCache).c_str(), exePath);
    WritePrivateProfileStringW(L"Performance", L"NormalRefreshRate", L"100", exePath);
    WritePrivateProfileStringW(L"Performance", L"LowResourceRefreshRate", L"200", exePath);
    WritePrivateProfileStringW(L"Performance", L"MinThumbnailSize", L"150", exePath);
    WritePrivateProfileStringW(L"Performance", L"MemoryThreshold", L"256", exePath);
    WritePrivateProfileStringW(L"Performance", L"CPUThreshold", L"60", exePath);
    WritePrivateProfileStringW(L"Performance", L"ReserveMemory", L"false", exePath);
    WritePrivateProfileStringW(L"Performance", L"OptimizePriority", L"false", exePath);
    WritePrivateProfileStringW(L"Performance", L"ReduceVisualEffects", L"true", exePath);
    WritePrivateProfileStringW(L"Performance", L"AutoCleanupInterval", std::to_wstring(cleanupInterval).c_str(), exePath);
    WritePrivateProfileStringW(L"Performance", L"MaxThumbnailMemory", std::to_wstring(maxMemory / 4).c_str(), exePath);
    
    // Recargar configuración
    LoadPerformanceSettings();
    
    // Aplicar cambios inmediatamente
    g_lowResourceMode = true;
    g_cleanupInterval = cleanupInterval;
    g_maxThumbnailMemory = (maxMemory / 4) * 1024 * 1024;
    g_windowCacheTimeout = 2000;
    
    // Limpiar cache y optimizar
    CleanupThumbnailCache();
    ResetPerformanceCounters();
    
    // Mostrar notificación con información del sistema
    wchar_t message[512];
    swprintf_s(message, L"Full Low Resources mode activated!\n\nSystem Analysis:\n- Total RAM: %d MB\n- Memory Load: %d%%\n- Cache Size: %d thumbnails\n- Cleanup Interval: %d ms\n\nConfiguration optimized for:\n- Minimum resource usage\n- Maximum compatibility\n- Extended battery life", 
               maxMemory, memoryLoad, maxCache, cleanupInterval);
    
    MessageBoxW(NULL, message, L"BetterAltTab - Low Resources Mode", MB_OK | MB_ICONINFORMATION);
}

// ============================================================
//  resize de las miniaturas para que todas las columnas encajen en el overlay
// ============================================================
int g_previewW = BASE_PREVIEW_WIDTH; // inicializado con constante
int g_previewH = BASE_PREVIEW_HEIGHT;

void UpdatePreviewSize()
{
    int usable = g_overlayWidth - 2 * GRID_PADDING_X - (g_fixedCols - 1) * PREVIEW_MARGIN;
    g_previewW = max(40, usable / g_fixedCols);  // Min size to avoid tiny thumbs
    g_previewH = (int)(g_previewW * (float)BASE_PREVIEW_HEIGHT / BASE_PREVIEW_WIDTH);  // Preserve aspect
}

#undef PREVIEW_WIDTH
#undef PREVIEW_HEIGHT
#define PREVIEW_WIDTH  g_previewW
#define PREVIEW_HEIGHT g_previewH

LRESULT CALLBACK SettingsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static int currentTab = 0;
    static HFONT hTitleFont, hLabelFont, hControlFont;
    
    switch (msg) {
        case WM_CREATE: {
            // Crear fuentes personalizadas para el tema dark OLED
            hTitleFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
            hLabelFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
            hControlFont = CreateFontW(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
            
            // Título principal
            CreateWindowW(L"STATIC", L"BetterAltTab Settings", WS_CHILD | WS_VISIBLE | SS_CENTER,
                20, 20, 760, 30, hwnd, nullptr, nullptr, nullptr);
            
            // Crear botones de pestañas personalizados (sin TabControl)
            CreateWindowW(L"BUTTON", L"General", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                20, 70, 120, 30, hwnd, (HMENU)1001, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Performance", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                150, 70, 120, 30, hwnd, (HMENU)1002, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"UI", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                280, 70, 120, 30, hwnd, (HMENU)1003, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Hotkeys", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                410, 70, 120, 30, hwnd, (HMENU)1004, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Advanced", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                540, 70, 120, 30, hwnd, (HMENU)1005, nullptr, nullptr);
            
            // Crear controles para la pestaña General (posición ajustada)
            CreateWindowW(L"STATIC", L"Columns:", WS_CHILD | WS_VISIBLE, 40, 120, 120, 20, hwnd, (HMENU)IDC_COLUMNS_LABEL, nullptr, nullptr);
            wchar_t currentCols[4];
            _itow_s(g_fixedCols, currentCols, 4, 10);
            CreateWindowW(L"EDIT", currentCols, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 170, 120, 60, 25, hwnd, (HMENU)IDC_COLUMNS_EDIT, nullptr, nullptr);
            
            CreateWindowW(L"STATIC", L"Overlay Alpha:", WS_CHILD | WS_VISIBLE, 40, 160, 120, 20, hwnd, (HMENU)IDC_OVERLAY_ALPHA_LABEL, nullptr, nullptr);
            wchar_t currentAlpha[4];
            _itow_s(255, currentAlpha, 4, 10);
            CreateWindowW(L"EDIT", currentAlpha, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 170, 160, 60, 25, hwnd, (HMENU)IDC_OVERLAY_ALPHA_EDIT, nullptr, nullptr);
            
            CreateWindowW(L"BUTTON", L"Dynamic Order", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 200, 150, 25, hwnd, (HMENU)IDC_DYNAMIC_ORDER_CHECK, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Save Window Order", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 240, 150, 25, hwnd, (HMENU)IDC_SAVE_WINDOW_ORDER_CHECK, nullptr, nullptr);
            
            // Crear controles para la pestaña Performance
            CreateWindowW(L"STATIC", L"Low Resource Mode:", WS_CHILD | WS_VISIBLE, 40, 120, 140, 20, hwnd, (HMENU)IDC_LOW_RESOURCE_MODE_LABEL, nullptr, nullptr);
            HWND hLowResourceCombo = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 190, 120, 120, 100, hwnd, (HMENU)IDC_LOW_RESOURCE_MODE_COMBO, nullptr, nullptr);
            SendMessage(hLowResourceCombo, CB_ADDSTRING, 0, (LPARAM)L"Auto");
            SendMessage(hLowResourceCombo, CB_ADDSTRING, 0, (LPARAM)L"On");
            SendMessage(hLowResourceCombo, CB_ADDSTRING, 0, (LPARAM)L"Off");
            SendMessage(hLowResourceCombo, CB_SETCURSEL, 0, 0);
            
            CreateWindowW(L"STATIC", L"Max Thumbnail Cache:", WS_CHILD | WS_VISIBLE, 40, 160, 140, 20, hwnd, (HMENU)IDC_MAX_THUMBNAIL_CACHE_LABEL, nullptr, nullptr);
            wchar_t currentCache[4];
            _itow_s(20, currentCache, 4, 10);
            CreateWindowW(L"EDIT", currentCache, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 190, 160, 60, 25, hwnd, (HMENU)IDC_MAX_THUMBNAIL_CACHE_EDIT, nullptr, nullptr);
            
            CreateWindowW(L"STATIC", L"Normal Refresh Rate (ms):", WS_CHILD | WS_VISIBLE, 40, 200, 160, 20, hwnd, (HMENU)IDC_NORMAL_REFRESH_RATE_LABEL, nullptr, nullptr);
            wchar_t currentRefresh[4];
            _itow_s(25, currentRefresh, 4, 10);
            CreateWindowW(L"EDIT", currentRefresh, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 210, 200, 60, 25, hwnd, (HMENU)IDC_NORMAL_REFRESH_RATE_EDIT, nullptr, nullptr);
            
            CreateWindowW(L"STATIC", L"Auto Cleanup Interval (ms):", WS_CHILD | WS_VISIBLE, 40, 240, 160, 20, hwnd, (HMENU)IDC_AUTO_CLEANUP_INTERVAL_LABEL, nullptr, nullptr);
            wchar_t currentCleanup[6];
            _itow_s(30000, currentCleanup, 6, 10);
            CreateWindowW(L"EDIT", currentCleanup, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 210, 240, 80, 25, hwnd, (HMENU)IDC_AUTO_CLEANUP_INTERVAL_EDIT, nullptr, nullptr);
            
            CreateWindowW(L"STATIC", L"Max Thumbnail Memory (MB):", WS_CHILD | WS_VISIBLE, 40, 280, 160, 20, hwnd, (HMENU)IDC_MAX_THUMBNAIL_MEMORY_LABEL, nullptr, nullptr);
            wchar_t currentMemory[4];
            _itow_s(100, currentMemory, 4, 10);
            CreateWindowW(L"EDIT", currentMemory, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 210, 280, 60, 25, hwnd, (HMENU)IDC_MAX_THUMBNAIL_MEMORY_EDIT, nullptr, nullptr);
            
            CreateWindowW(L"STATIC", L"Window Cache Timeout (ms):", WS_CHILD | WS_VISIBLE, 40, 320, 160, 20, hwnd, (HMENU)IDC_WINDOW_CACHE_TIMEOUT_LABEL, nullptr, nullptr);
            wchar_t currentTimeout[6];
            _itow_s(10000, currentTimeout, 6, 10);
            CreateWindowW(L"EDIT", currentTimeout, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 210, 320, 80, 25, hwnd, (HMENU)IDC_WINDOW_CACHE_TIMEOUT_EDIT, nullptr, nullptr);
            
            // Crear controles para la pestaña UI
            CreateWindowW(L"BUTTON", L"Show Tray Icon", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 120, 150, 25, hwnd, (HMENU)IDC_SHOW_TRAY_ICON_CHECK, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Show System Info", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 160, 150, 25, hwnd, (HMENU)IDC_SHOW_SYSTEM_INFO_CHECK, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Show Window Count", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 200, 150, 25, hwnd, (HMENU)IDC_SHOW_WINDOW_COUNT_CHECK, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Show Memory Usage", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 240, 150, 25, hwnd, (HMENU)IDC_SHOW_MEMORY_USAGE_CHECK, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Show Performance Mode", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 280, 150, 25, hwnd, (HMENU)IDC_SHOW_PERFORMANCE_MODE_CHECK, nullptr, nullptr);
            
            CreateWindowW(L"STATIC", L"Tray Icon Size:", WS_CHILD | WS_VISIBLE, 40, 320, 120, 20, hwnd, (HMENU)IDC_TRAY_ICON_SIZE_LABEL, nullptr, nullptr);
            HWND hTraySizeCombo = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 170, 320, 100, 100, hwnd, (HMENU)IDC_TRAY_ICON_SIZE_COMBO, nullptr, nullptr);
            SendMessage(hTraySizeCombo, CB_ADDSTRING, 0, (LPARAM)L"16");
            SendMessage(hTraySizeCombo, CB_ADDSTRING, 0, (LPARAM)L"32");
            SendMessage(hTraySizeCombo, CB_ADDSTRING, 0, (LPARAM)L"48");
            SendMessage(hTraySizeCombo, CB_SETCURSEL, 0, 0);
            
            CreateWindowW(L"STATIC", L"Tray Icon Color:", WS_CHILD | WS_VISIBLE, 40, 360, 120, 20, hwnd, (HMENU)IDC_TRAY_ICON_COLOR_LABEL, nullptr, nullptr);
            CreateWindowW(L"EDIT", L"00FF00", WS_CHILD | WS_VISIBLE | WS_BORDER, 170, 360, 100, 25, hwnd, (HMENU)IDC_TRAY_ICON_COLOR_EDIT, nullptr, nullptr);
            
            // Crear controles para la pestaña Hotkeys
            CreateWindowW(L"STATIC", L"Alt+Q Key:", WS_CHILD | WS_VISIBLE, 40, 120, 120, 20, hwnd, (HMENU)IDC_ALTQ_KEY_LABEL, nullptr, nullptr);
            CreateWindowW(L"EDIT", L"VK_Q", WS_CHILD | WS_VISIBLE | WS_BORDER, 170, 120, 100, 25, hwnd, (HMENU)IDC_ALTQ_KEY_EDIT, nullptr, nullptr);
            
            CreateWindowW(L"STATIC", L"Alt+A Key:", WS_CHILD | WS_VISIBLE, 40, 160, 120, 20, hwnd, (HMENU)IDC_ALTA_KEY_LABEL, nullptr, nullptr);
            CreateWindowW(L"EDIT", L"VK_A", WS_CHILD | WS_VISIBLE | WS_BORDER, 170, 160, 100, 25, hwnd, (HMENU)IDC_ALTA_KEY_EDIT, nullptr, nullptr);
            
            CreateWindowW(L"STATIC", L"Numpad Modifier:", WS_CHILD | WS_VISIBLE, 40, 200, 120, 20, hwnd, (HMENU)IDC_NUMPAD_MODIFIER_LABEL, nullptr, nullptr);
            HWND hNumpadCombo = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 170, 200, 120, 100, hwnd, (HMENU)IDC_NUMPAD_MODIFIER_COMBO, nullptr, nullptr);
            SendMessage(hNumpadCombo, CB_ADDSTRING, 0, (LPARAM)L"VK_CONTROL");
            SendMessage(hNumpadCombo, CB_ADDSTRING, 0, (LPARAM)L"VK_LCONTROL");
            SendMessage(hNumpadCombo, CB_ADDSTRING, 0, (LPARAM)L"VK_RCONTROL");
            SendMessage(hNumpadCombo, CB_SETCURSEL, 0, 0);
            
            // Crear controles para la pestaña Advanced
            CreateWindowW(L"BUTTON", L"Debug Mode", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 120, 150, 25, hwnd, (HMENU)IDC_DEBUG_MODE_CHECK, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Event Logging", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 40, 160, 150, 25, hwnd, (HMENU)IDC_EVENT_LOGGING_CHECK, nullptr, nullptr);
            
            CreateWindowW(L"STATIC", L"Log Level:", WS_CHILD | WS_VISIBLE, 40, 200, 120, 20, hwnd, (HMENU)IDC_LOG_LEVEL_LABEL, nullptr, nullptr);
            HWND hLogLevelCombo = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 170, 200, 100, 100, hwnd, (HMENU)IDC_LOG_LEVEL_COMBO, nullptr, nullptr);
            SendMessage(hLogLevelCombo, CB_ADDSTRING, 0, (LPARAM)L"1");
            SendMessage(hLogLevelCombo, CB_ADDSTRING, 0, (LPARAM)L"2");
            SendMessage(hLogLevelCombo, CB_ADDSTRING, 0, (LPARAM)L"3");
            SendMessage(hLogLevelCombo, CB_ADDSTRING, 0, (LPARAM)L"4");
            SendMessage(hLogLevelCombo, CB_ADDSTRING, 0, (LPARAM)L"5");
            SendMessage(hLogLevelCombo, CB_SETCURSEL, 0, 0);
            
            // Botones de acción en la parte inferior
            CreateWindowW(L"BUTTON", L"Apply", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 600, 540, 80, 30, hwnd, (HMENU)IDC_APPLY_BUTTON, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 690, 540, 80, 30, hwnd, (HMENU)IDC_CANCEL_BUTTON, nullptr, nullptr);
            
            // Aplicar fuentes y estilos a todos los controles
            ApplyDarkThemeToControls(hwnd, hTitleFont, hLabelFont, hControlFont);
            
            // Mostrar solo la primera pestaña inicialmente
            ShowTabControls(hwnd, 0);
            return 0;
        }
        
        case WM_COMMAND: {
            if (LOWORD(wParam) >= 1001 && LOWORD(wParam) <= 1005) {
                // Cambio de pestaña
                currentTab = LOWORD(wParam) - 1001;
                ShowTabControls(hwnd, currentTab);
                
                // Actualizar estado visual de los botones de pestaña
                for (int i = 1001; i <= 1005; i++) {
                    HWND hButton = GetDlgItem(hwnd, i);
                    if (hButton) {
                        if (i == LOWORD(wParam)) {
                            // Pestaña activa - resaltar
                            SetWindowLong(hButton, GWL_STYLE, GetWindowLong(hButton, GWL_STYLE) | BS_PUSHLIKE);
                            InvalidateRect(hButton, NULL, TRUE);
                        } else {
                            // Pestaña inactiva - normal
                            SetWindowLong(hButton, GWL_STYLE, GetWindowLong(hButton, GWL_STYLE) & ~BS_PUSHLIKE);
                            InvalidateRect(hButton, NULL, TRUE);
                        }
                    }
                }
                return 0;
            } else if (LOWORD(wParam) == IDC_APPLY_BUTTON) {
                SaveAllSettings(hwnd);
                    UpdatePreviewSize();
                    DestroyWindow(hwnd);
            } else if (LOWORD(wParam) == IDC_CANCEL_BUTTON) {
                DestroyWindow(hwnd);
            }
            return 0;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Fondo negro OLED
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(hdc, &rect, blackBrush);
            DeleteObject(blackBrush);
            
            // Borde blanco fino
            HPEN whitePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
            HPEN oldPen = (HPEN)SelectObject(hdc, whitePen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBrush);
            DeleteObject(whitePen);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            SetBkColor(hdcStatic, RGB(0, 0, 0));
            SetTextColor(hdcStatic, RGB(255, 255, 255));
            return (LRESULT)CreateSolidBrush(RGB(0, 0, 0));
        }
        
        case WM_CTLCOLOREDIT: {
            HDC hdcEdit = (HDC)wParam;
            SetBkColor(hdcEdit, RGB(20, 20, 20));
            SetTextColor(hdcEdit, RGB(255, 255, 255));
            return (LRESULT)CreateSolidBrush(RGB(20, 20, 20));
        }
        
        case WM_CTLCOLORBTN: {
            HDC hdcButton = (HDC)wParam;
            if (GetDlgCtrlID((HWND)lParam) >= 1001 && GetDlgCtrlID((HWND)lParam) <= 1005) {
                // Botones de pestaña - tema especial
                if (GetDlgCtrlID((HWND)lParam) == currentTab + 1001) {
                    // Pestaña activa
                    SetBkColor(hdcButton, RGB(50, 50, 50));
                    SetTextColor(hdcButton, RGB(255, 255, 255));
                    return (LRESULT)CreateSolidBrush(RGB(50, 50, 50));
                } else {
                    // Pestaña inactiva
                    SetBkColor(hdcButton, RGB(30, 30, 30));
                    SetTextColor(hdcButton, RGB(200, 200, 200));
                    return (LRESULT)CreateSolidBrush(RGB(30, 30, 30));
                }
            } else {
                // Otros botones - tema normal
                SetBkColor(hdcButton, RGB(0, 0, 0));
                SetTextColor(hdcButton, RGB(255, 255, 255));
                return (LRESULT)CreateSolidBrush(RGB(0, 0, 0));
            }
        }
        
        case WM_CLOSE: {
            DestroyWindow(hwnd);
            return 0;
        }
        
        case WM_DESTROY: {
            // Limpiar fuentes
            if (hTitleFont) DeleteObject(hTitleFont);
            if (hLabelFont) DeleteObject(hLabelFont);
            if (hControlFont) DeleteObject(hControlFont);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void ShowSettingsDialog(HWND parent) {
    const wchar_t SETTINGS_CLASS_NAME[] = L"BetterAltTabSettingsClass";
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = SettingsDlgProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = SETTINGS_CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); // Fondo negro OLED
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    // Obtener dimensiones de la pantalla para centrar la ventana
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 800;
    int windowHeight = 600;
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW, // Siempre al frente, sin barra de tareas
        SETTINGS_CLASS_NAME, L"BetterAltTab Settings",
        WS_POPUP | WS_BORDER, // Ventana popup con borde fino
        x, y, windowWidth, windowHeight,
        parent, nullptr, wc.hInstance, nullptr);

    if (hwnd) {
        // Aplicar tema dark OLED
        SetWindowTheme(hwnd, L"", L"");
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        SetForegroundWindow(hwnd);
    }
}

// ===============================
// Funciones de optimización de rendimiento
// ===============================
void LoadPerformanceSettings(); // Carga configuración de rendimiento desde INI
void CheckPerformanceMode(); // Verifica y ajusta el modo de rendimiento
void CleanupThumbnailCache(); // Limpia cache de miniaturas automáticamente
void MonitorMemoryUsage(); // Monitorea el uso de memoria
bool ShouldRedraw(); // Determina si se debe redibujar basado en throttling
void OptimizeProcessPriority(); // Optimiza la prioridad del proceso
void ResetPerformanceCounters(); // Resetea contadores de rendimiento



// ============================================================
//  Implementación de funciones de filtrado
// ============================================================

void LoadFilterSettings() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    wcscat_s(exePath, L"\\BetterAltTab.ini");
    
    // Cargar configuración de filtros
    g_filteringEnabled = GetPrivateProfileIntW(L"Filters", L"Enabled", 0, exePath) != 0;
    g_filterByTitle = GetPrivateProfileIntW(L"Filters", L"FilterByTitle", 1, exePath) != 0;
    
    // Cargar configuración de Alt+A
    g_altAEnabled = GetPrivateProfileIntW(L"General", L"AltAEnabled", 1, exePath) != 0;
    
    // Cargar configuración de modo dinámico
    g_dynamicOrder = GetPrivateProfileIntW(L"General", L"DynamicOrder", 1, exePath) != 0;
    g_filterByClassName = GetPrivateProfileIntW(L"Filters", L"FilterByClassName", 1, exePath) != 0;
    g_filterByProcessName = GetPrivateProfileIntW(L"Filters", L"FilterByProcessName", 1, exePath) != 0;
    g_filterCaseSensitive = GetPrivateProfileIntW(L"Filters", L"CaseSensitive", 0, exePath) != 0;
    g_filterRegex = GetPrivateProfileIntW(L"Filters", L"UseRegex", 0, exePath) != 0;
    g_filterExcludeHidden = GetPrivateProfileIntW(L"Filters", L"ExcludeHidden", 1, exePath) != 0;
    g_filterExcludeMinimized = GetPrivateProfileIntW(L"Filters", L"ExcludeMinimized", 0, exePath) != 0;
    
    // Cargar listas de exclusión/inclusión
    wchar_t buffer[1024];
    if (GetPrivateProfileStringW(L"Filters", L"ExcludedProcesses", L"", buffer, 1024, exePath)) {
        g_excludedProcesses.clear();
        std::wstring processes = buffer;
        size_t pos = 0;
        while ((pos = processes.find(L'|')) != std::wstring::npos) {
            g_excludedProcesses.push_back(processes.substr(0, pos));
            processes.erase(0, pos + 1);
        }
        if (!processes.empty()) g_excludedProcesses.push_back(processes);
    }
    
    if (GetPrivateProfileStringW(L"Filters", L"ExcludedClasses", L"", buffer, 1024, exePath)) {
        g_excludedClasses.clear();
        std::wstring classes = buffer;
        size_t pos = 0;
        while ((pos = classes.find(L'|')) != std::wstring::npos) {
            g_excludedClasses.push_back(classes.substr(0, pos));
            classes.erase(0, pos + 1);
        }
        if (!classes.empty()) g_excludedClasses.push_back(classes);
    }
    
    if (GetPrivateProfileStringW(L"Filters", L"IncludedProcesses", L"", buffer, 1024, exePath)) {
        g_includedProcesses.clear();
        std::wstring processes = buffer;
        size_t pos = 0;
        while ((pos = processes.find(L'|')) != std::wstring::npos) {
            g_includedProcesses.push_back(processes.substr(0, pos));
            processes.erase(0, pos + 1);
        }
        if (!processes.empty()) g_includedProcesses.push_back(processes);
    }
    
    if (GetPrivateProfileStringW(L"Filters", L"IncludedClasses", L"", buffer, 1024, exePath)) {
        g_includedClasses.clear();
        std::wstring classes = buffer;
        size_t pos = 0;
        while ((pos = classes.find(L'|')) != std::wstring::npos) {
            g_includedClasses.push_back(classes.substr(0, pos));
            classes.erase(0, pos + 1);
        }
        if (!classes.empty()) g_includedClasses.push_back(classes);
    }
}

void SaveFilterSettings() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    wcscat_s(exePath, L"\\BetterAltTab.ini");
    
    // Guardar configuración de filtros
    WritePrivateProfileStringW(L"Filters", L"Enabled", g_filteringEnabled ? L"1" : L"0", exePath);
    WritePrivateProfileStringW(L"Filters", L"FilterByTitle", g_filterByTitle ? L"1" : L"0", exePath);
    
    // Guardar configuración de Alt+A
    WritePrivateProfileStringW(L"General", L"AltAEnabled", g_altAEnabled ? L"1" : L"0", exePath);
    
    // Guardar configuración de modo dinámico
    WritePrivateProfileStringW(L"General", L"DynamicOrder", g_dynamicOrder ? L"1" : L"0", exePath);
    WritePrivateProfileStringW(L"Filters", L"FilterByClassName", g_filterByClassName ? L"1" : L"0", exePath);
    WritePrivateProfileStringW(L"Filters", L"FilterByProcessName", g_filterByProcessName ? L"1" : L"0", exePath);
    WritePrivateProfileStringW(L"Filters", L"CaseSensitive", g_filterCaseSensitive ? L"1" : L"0", exePath);
    WritePrivateProfileStringW(L"Filters", L"UseRegex", g_filterRegex ? L"1" : L"0", exePath);
    WritePrivateProfileStringW(L"Filters", L"ExcludeHidden", g_filterExcludeHidden ? L"1" : L"0", exePath);
    WritePrivateProfileStringW(L"Filters", L"ExcludeMinimized", g_filterExcludeMinimized ? L"1" : L"0", exePath);
    
    // Guardar listas de exclusión/inclusión
    std::wstring excludedProcesses;
    for (const auto& proc : g_excludedProcesses) {
        if (!excludedProcesses.empty()) excludedProcesses += L"|";
        excludedProcesses += proc;
    }
    WritePrivateProfileStringW(L"Filters", L"ExcludedProcesses", excludedProcesses.c_str(), exePath);
    
    std::wstring excludedClasses;
    for (const auto& cls : g_excludedClasses) {
        if (!excludedClasses.empty()) excludedClasses += L"|";
        excludedClasses += cls;
    }
    WritePrivateProfileStringW(L"Filters", L"ExcludedClasses", excludedClasses.c_str(), exePath);
    
    std::wstring includedProcesses;
    for (const auto& proc : g_includedProcesses) {
        if (!includedProcesses.empty()) includedProcesses += L"|";
        includedProcesses += proc;
    }
    WritePrivateProfileStringW(L"Filters", L"IncludedProcesses", includedProcesses.c_str(), exePath);
    
    std::wstring includedClasses;
    for (const auto& cls : g_includedClasses) {
        if (!includedClasses.empty()) includedClasses += L"|";
        includedClasses += cls;
    }
    WritePrivateProfileStringW(L"Filters", L"IncludedClasses", includedClasses.c_str(), exePath);
}

std::wstring GetProcessName(HWND hwnd) {
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid)) return L"";
    
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) return L"";
    
    wchar_t exePath[MAX_PATH] = {0};
    std::wstring processName = L"";
    
    if (GetModuleFileNameExW(hProcess, nullptr, exePath, MAX_PATH)) {
        wchar_t* fileName = wcsrchr(exePath, L'\\');
        if (fileName) {
            processName = fileName + 1;
        }
    }
    
    CloseHandle(hProcess);
    return processName;
}

bool WindowMatchesFilter(const WindowInfo& window) {
    if (!g_filteringEnabled) return true;
    

    
    // Verificar exclusiones por proceso
    if (!g_excludedProcesses.empty()) {
        std::wstring processName = GetProcessName(window.hwnd);
        for (const auto& excluded : g_excludedProcesses) {
            if (processName.find(excluded) != std::wstring::npos) return false;
        }
    }
    
    // Verificar exclusiones por clase
    if (!g_excludedClasses.empty()) {
        for (const auto& excluded : g_excludedClasses) {
            if (window.className.find(excluded) != std::wstring::npos) return false;
        }
    }
    
    // Verificar inclusiones (whitelist) - si hay inclusiones, solo mostrar esas
    if (!g_includedProcesses.empty()) {
        std::wstring processName = GetProcessName(window.hwnd);
        bool found = false;
        for (const auto& included : g_includedProcesses) {
            if (processName.find(included) != std::wstring::npos) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    
    if (!g_includedClasses.empty()) {
        bool found = false;
        for (const auto& included : g_includedClasses) {
            if (window.className.find(included) != std::wstring::npos) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    
    // Verificar filtro de texto
    if (!g_filterText.empty()) {
        bool matches = false;
        std::wstring searchText = g_filterText;
        std::wstring title = window.title;
        std::wstring className = window.className;
        std::wstring processName = GetProcessName(window.hwnd);
        
        if (!g_filterCaseSensitive) {
            std::transform(searchText.begin(), searchText.end(), searchText.begin(), ::tolower);
            std::transform(title.begin(), title.end(), title.begin(), ::tolower);
            std::transform(className.begin(), className.end(), className.begin(), ::tolower);
            std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);
        }
        
        if (g_filterByTitle && title.find(searchText) != std::wstring::npos) matches = true;
        if (g_filterByClassName && className.find(searchText) != std::wstring::npos) matches = true;
        if (g_filterByProcessName && processName.find(searchText) != std::wstring::npos) matches = true;
        
        // Si hay texto de filtro, la ventana debe coincidir
        if (!matches) return false;
    }
    // Si no hay texto de filtro, todas las ventanas pasan (sin filtrar)
    
    // Verificar exclusiones por estado de ventana
    if (g_filterExcludeHidden && !IsWindowVisible(window.hwnd)) return false;
    if (g_filterExcludeMinimized && IsIconic(window.hwnd)) return false;
    
    return true;
}

void ApplyFilters() {

    
    if (!g_filteringEnabled) {
        g_filteredWindows = g_windows; // Usar g_windows en lugar de g_allWindows

        return;
    }
    
    g_filteredWindows.clear();
    for (const auto& window : g_windows) { // Usar g_windows en lugar de g_allWindows
        if (WindowMatchesFilter(window)) {
            g_filteredWindows.push_back(window);
        }
    }
    
    // Reset selection to first window when filtering changes
    if (!g_filteredWindows.empty()) {
        g_selectedIndex = 0;
        g_hoverIndex = 0;
    } else {
        g_selectedIndex = -1;
        g_hoverIndex = -1;
    }
    

}

void ToggleFiltering() {
    g_filteringEnabled = !g_filteringEnabled;
    ApplyFilters();
    SaveFilterSettings();
}

void ClearFilters() {
    g_filterText.clear();
    g_excludedProcesses.clear();
    g_excludedClasses.clear();
    g_includedProcesses.clear();
    g_includedClasses.clear();
    ApplyFilters();
    SaveFilterSettings();
}

void SetFilterText(const std::wstring& text) {
    g_filterText = text;
    ApplyFilters();
}

void AddExcludedProcess(const std::wstring& processName) {
    if (std::find(g_excludedProcesses.begin(), g_excludedProcesses.end(), processName) == g_excludedProcesses.end()) {
        g_excludedProcesses.push_back(processName);
        ApplyFilters();
        SaveFilterSettings();
    }
}

void RemoveExcludedProcess(const std::wstring& processName) {
    auto it = std::find(g_excludedProcesses.begin(), g_excludedProcesses.end(), processName);
    if (it != g_excludedProcesses.end()) {
        g_excludedProcesses.erase(it);
        ApplyFilters();
        SaveFilterSettings();
    }
}

void AddExcludedClass(const std::wstring& className) {
    if (std::find(g_excludedClasses.begin(), g_excludedClasses.end(), className) == g_excludedClasses.end()) {
        g_excludedClasses.push_back(className);
        ApplyFilters();
        SaveFilterSettings();
    }
}

void RemoveExcludedClass(const std::wstring& className) {
    auto it = std::find(g_excludedClasses.begin(), g_excludedClasses.end(), className);
    if (it != g_excludedClasses.end()) {
        g_excludedClasses.erase(it);
        ApplyFilters();
        SaveFilterSettings();
    }
}

void AddIncludedProcess(const std::wstring& processName) {
    if (std::find(g_includedProcesses.begin(), g_includedProcesses.end(), processName) == g_includedProcesses.end()) {
        g_includedProcesses.push_back(processName);
        ApplyFilters();
        SaveFilterSettings();
    }
}

void RemoveIncludedProcess(const std::wstring& processName) {
    auto it = std::find(g_includedProcesses.begin(), g_includedProcesses.end(), processName);
    if (it != g_includedProcesses.end()) {
        g_includedProcesses.erase(it);
        ApplyFilters();
        SaveFilterSettings();
    }
}

void AddIncludedClass(const std::wstring& className) {
    if (std::find(g_includedClasses.begin(), g_includedClasses.end(), className) == g_includedClasses.end()) {
        g_includedClasses.push_back(className);
        ApplyFilters();
        SaveFilterSettings();
    }
}

void RemoveIncludedClass(const std::wstring& className) {
    auto it = std::find(g_includedClasses.begin(), g_includedClasses.end(), className);
    if (it != g_includedClasses.end()) {
        g_includedClasses.erase(it);
        ApplyFilters();
        SaveFilterSettings();
    }
}

// Forward declaration for the filter dialog window procedure
LRESULT CALLBACK FilterDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ShowFilterDialog(HWND parent) {
    // Crear un diálogo simple para configurar filtros
    const wchar_t FILTER_DIALOG_CLASS[] = L"BetterAltTabFilterDialogClass";
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = FilterDialogProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = FILTER_DIALOG_CLASS;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);
    
    HWND hDialog = CreateWindowExW(
        0, FILTER_DIALOG_CLASS, L"Filter Settings",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 180,
        parent, nullptr, wc.hInstance, nullptr);
    
    if (hDialog) {
        ShowWindow(hDialog, SW_SHOW);
        UpdateWindow(hDialog);
    }
}

// Window procedure for the filter dialog
LRESULT CALLBACK FilterDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hFilterText, hFilterByTitle, hFilterByClass, hFilterByProcess;
    static HWND hCaseSensitive, hExcludeHidden, hExcludeMinimized;
    static HWND hEnabled, hClear, hApply;
    
    switch (msg) {
        case WM_CREATE: {
            // Crear controles del diálogo
            CreateWindowW(L"STATIC", L"Filter Text:", WS_CHILD | WS_VISIBLE, 10, 10, 80, 20, hwnd, nullptr, nullptr, nullptr);
            hFilterText = CreateWindowW(L"EDIT", g_filterText.c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 10, 200, 20, hwnd, nullptr, nullptr, nullptr);
            
            hFilterByTitle = CreateWindowW(L"BUTTON", L"Filter by Title", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, 40, 120, 20, hwnd, nullptr, nullptr, nullptr);
            hFilterByClass = CreateWindowW(L"BUTTON", L"Filter by Class", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 140, 40, 120, 20, hwnd, nullptr, nullptr, nullptr);
            hFilterByProcess = CreateWindowW(L"BUTTON", L"Filter by Process", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 270, 40, 120, 20, hwnd, nullptr, nullptr, nullptr);
            
            hCaseSensitive = CreateWindowW(L"BUTTON", L"Case Sensitive", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, 70, 120, 20, hwnd, nullptr, nullptr, nullptr);
            hExcludeHidden = CreateWindowW(L"BUTTON", L"Exclude Hidden", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 140, 70, 120, 20, hwnd, nullptr, nullptr, nullptr);
            hExcludeMinimized = CreateWindowW(L"BUTTON", L"Exclude Minimized", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 270, 70, 120, 20, hwnd, nullptr, nullptr, nullptr);
            
            hEnabled = CreateWindowW(L"BUTTON", L"Enable Filtering", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, 100, 120, 20, hwnd, nullptr, nullptr, nullptr);
            hClear = CreateWindowW(L"BUTTON", L"Clear Filters", WS_CHILD | WS_VISIBLE, 140, 100, 80, 25, hwnd, nullptr, nullptr, nullptr);
            hApply = CreateWindowW(L"BUTTON", L"Apply", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 270, 100, 80, 25, hwnd, nullptr, nullptr, nullptr);
            
            // Establecer estados iniciales
            Button_SetCheck(hFilterByTitle, g_filterByTitle ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(hFilterByClass, g_filterByClassName ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(hFilterByProcess, g_filterByProcessName ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(hCaseSensitive, g_filterCaseSensitive ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(hExcludeHidden, g_filterExcludeHidden ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(hExcludeMinimized, g_filterExcludeMinimized ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(hEnabled, g_filteringEnabled ? BST_CHECKED : BST_UNCHECKED);
            
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == (WORD)hClear) {
                ClearFilters();
                SetWindowTextW(hFilterText, L"");
                Button_SetCheck(hEnabled, BST_UNCHECKED);
                return 0;
            } else if (LOWORD(wParam) == (WORD)hApply) {
                // Obtener valores de los controles
                wchar_t filterText[256];
                GetWindowTextW(hFilterText, filterText, 256);
                g_filterText = filterText;
                
                g_filterByTitle = (Button_GetCheck(hFilterByTitle) == BST_CHECKED);
                g_filterByClassName = (Button_GetCheck(hFilterByClass) == BST_CHECKED);
                g_filterByProcessName = (Button_GetCheck(hFilterByProcess) == BST_CHECKED);
                g_filterCaseSensitive = (Button_GetCheck(hCaseSensitive) == BST_CHECKED);
                g_filterExcludeHidden = (Button_GetCheck(hExcludeHidden) == BST_CHECKED);
                g_filterExcludeMinimized = (Button_GetCheck(hExcludeMinimized) == BST_CHECKED);
                g_filteringEnabled = (Button_GetCheck(hEnabled) == BST_CHECKED);
                
                // Aplicar filtros y guardar configuración
                ApplyFilters();
                SaveFilterSettings();
                
                // Redibujar la grilla si está visible
                HWND parent = GetParent(hwnd);
                if (parent && IsWindowVisible(parent)) {
                    InvalidateGrid(parent);
                }
                
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void ClearSearchBox() {
    if (g_quickFilterEdit) {
        SetWindowTextW(g_quickFilterEdit, L"");
        g_filterText.clear();
    }
}

// Función para mostrar/ocultar controles según la pestaña seleccionada
void ShowTabControls(HWND hwnd, int tabIndex) {
    // Ocultar todos los controles primero
    for (int i = IDC_COLUMNS_LABEL; i <= IDC_LOG_LEVEL_COMBO; i++) {
        HWND hControl = GetDlgItem(hwnd, i);
        if (hControl) {
            ShowWindow(hControl, SW_HIDE);
        }
    }
    
    // Mostrar controles según la pestaña
    switch (tabIndex) {
        case 0: // General
            ShowWindow(GetDlgItem(hwnd, IDC_COLUMNS_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_COLUMNS_EDIT), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_OVERLAY_ALPHA_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_OVERLAY_ALPHA_EDIT), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_DYNAMIC_ORDER_CHECK), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_SAVE_WINDOW_ORDER_CHECK), SW_SHOW);
            break;
            
        case 1: // Performance
            ShowWindow(GetDlgItem(hwnd, IDC_LOW_RESOURCE_MODE_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_LOW_RESOURCE_MODE_COMBO), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_MAX_THUMBNAIL_CACHE_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_MAX_THUMBNAIL_CACHE_EDIT), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_NORMAL_REFRESH_RATE_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_NORMAL_REFRESH_RATE_EDIT), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_AUTO_CLEANUP_INTERVAL_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_AUTO_CLEANUP_INTERVAL_EDIT), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_MAX_THUMBNAIL_MEMORY_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_MAX_THUMBNAIL_MEMORY_EDIT), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_WINDOW_CACHE_TIMEOUT_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_WINDOW_CACHE_TIMEOUT_EDIT), SW_SHOW);
            break;
            
        case 2: // UI
            ShowWindow(GetDlgItem(hwnd, IDC_SHOW_TRAY_ICON_CHECK), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_SHOW_SYSTEM_INFO_CHECK), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_SHOW_WINDOW_COUNT_CHECK), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_SHOW_MEMORY_USAGE_CHECK), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_SHOW_PERFORMANCE_MODE_CHECK), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_TRAY_ICON_SIZE_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_TRAY_ICON_SIZE_COMBO), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_TRAY_ICON_COLOR_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_TRAY_ICON_COLOR_EDIT), SW_SHOW);
            break;
            
        case 3: // Hotkeys
            ShowWindow(GetDlgItem(hwnd, IDC_ALTQ_KEY_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_ALTQ_KEY_EDIT), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_ALTA_KEY_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_ALTA_KEY_EDIT), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_NUMPAD_MODIFIER_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_NUMPAD_MODIFIER_COMBO), SW_SHOW);
            break;
            
        case 4: // Advanced
            ShowWindow(GetDlgItem(hwnd, IDC_DEBUG_MODE_CHECK), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_EVENT_LOGGING_CHECK), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_LOG_LEVEL_LABEL), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, IDC_LOG_LEVEL_COMBO), SW_SHOW);
            break;
    }
    
    // Los botones siempre están visibles
    ShowWindow(GetDlgItem(hwnd, IDC_APPLY_BUTTON), SW_SHOW);
    ShowWindow(GetDlgItem(hwnd, IDC_CANCEL_BUTTON), SW_SHOW);
}

// Función para guardar todas las configuraciones
void SaveAllSettings(HWND hwnd) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    wcscat_s(exePath, L"\\BetterAltTab.ini");
    
    // Obtener valores de los controles
    wchar_t buffer[256];
    
    // General
    GetDlgItemTextW(hwnd, IDC_COLUMNS_EDIT, buffer, 256);
    int newCols = _wtoi(buffer);
    if (newCols > 0 && newCols < 20) {
        g_fixedCols = newCols;
        WritePrivateProfileStringW(L"General", L"Columns", buffer, exePath);
    }
    
    GetDlgItemTextW(hwnd, IDC_OVERLAY_ALPHA_EDIT, buffer, 256);
    int newAlpha = _wtoi(buffer);
    if (newAlpha >= 0 && newAlpha <= 255) {
        WritePrivateProfileStringW(L"General", L"OverlayAlpha", buffer, exePath);
    }
    
    // Performance
    HWND hLowResourceCombo = GetDlgItem(hwnd, IDC_LOW_RESOURCE_MODE_COMBO);
    int lowResourceIndex = SendMessage(hLowResourceCombo, CB_GETCURSEL, 0, 0);
    const wchar_t* lowResourceModes[] = {L"auto", L"on", L"off"};
    WritePrivateProfileStringW(L"Performance", L"LowResourceMode", lowResourceModes[lowResourceIndex], exePath);
    
    GetDlgItemTextW(hwnd, IDC_MAX_THUMBNAIL_CACHE_EDIT, buffer, 256);
    WritePrivateProfileStringW(L"Performance", L"MaxThumbnailCache", buffer, exePath);
    
    GetDlgItemTextW(hwnd, IDC_NORMAL_REFRESH_RATE_EDIT, buffer, 256);
    WritePrivateProfileStringW(L"Performance", L"NormalRefreshRate", buffer, exePath);
    
    GetDlgItemTextW(hwnd, IDC_AUTO_CLEANUP_INTERVAL_EDIT, buffer, 256);
    WritePrivateProfileStringW(L"Performance", L"AutoCleanupInterval", buffer, exePath);
    
    GetDlgItemTextW(hwnd, IDC_MAX_THUMBNAIL_MEMORY_EDIT, buffer, 256);
    WritePrivateProfileStringW(L"Performance", L"MaxThumbnailMemory", buffer, exePath);
    
    GetDlgItemTextW(hwnd, IDC_WINDOW_CACHE_TIMEOUT_EDIT, buffer, 256);
    WritePrivateProfileStringW(L"Performance", L"WindowCacheTimeout", buffer, exePath);
    
    // UI
    BOOL showTrayIcon = (IsDlgButtonChecked(hwnd, IDC_SHOW_TRAY_ICON_CHECK) == BST_CHECKED);
    WritePrivateProfileStringW(L"UI", L"ShowTrayIcon", showTrayIcon ? L"1" : L"0", exePath);
    
    BOOL showSystemInfo = (IsDlgButtonChecked(hwnd, IDC_SHOW_SYSTEM_INFO_CHECK) == BST_CHECKED);
    WritePrivateProfileStringW(L"UI", L"ShowSystemInfo", showSystemInfo ? L"1" : L"0", exePath);
    
    BOOL showWindowCount = (IsDlgButtonChecked(hwnd, IDC_SHOW_WINDOW_COUNT_CHECK) == BST_CHECKED);
    WritePrivateProfileStringW(L"UI", L"ShowWindowCount", showWindowCount ? L"1" : L"0", exePath);
    
    BOOL showMemoryUsage = (IsDlgButtonChecked(hwnd, IDC_SHOW_MEMORY_USAGE_CHECK) == BST_CHECKED);
    WritePrivateProfileStringW(L"UI", L"ShowMemoryUsage", showMemoryUsage ? L"1" : L"0", exePath);
    
    BOOL showPerformanceMode = (IsDlgButtonChecked(hwnd, IDC_SHOW_PERFORMANCE_MODE_CHECK) == BST_CHECKED);
    WritePrivateProfileStringW(L"UI", L"ShowPerformanceMode", showPerformanceMode ? L"1" : L"0", exePath);
    
    HWND hTraySizeCombo = GetDlgItem(hwnd, IDC_TRAY_ICON_SIZE_COMBO);
    int traySizeIndex = SendMessage(hTraySizeCombo, CB_GETCURSEL, 0, 0);
    const wchar_t* traySizes[] = {L"16", L"32", L"48"};
    WritePrivateProfileStringW(L"UI", L"TrayIconSize", traySizes[traySizeIndex], exePath);
    
    GetDlgItemTextW(hwnd, IDC_TRAY_ICON_COLOR_EDIT, buffer, 256);
    WritePrivateProfileStringW(L"UI", L"TrayIconColor", buffer, exePath);
    
    // Hotkeys
    GetDlgItemTextW(hwnd, IDC_ALTQ_KEY_EDIT, buffer, 256);
    WritePrivateProfileStringW(L"Hotkeys", L"AltQKey", buffer, exePath);
    
    GetDlgItemTextW(hwnd, IDC_ALTA_KEY_EDIT, buffer, 256);
    WritePrivateProfileStringW(L"Hotkeys", L"AltAKey", buffer, exePath);
    
    HWND hNumpadCombo = GetDlgItem(hwnd, IDC_NUMPAD_MODIFIER_COMBO);
    int numpadIndex = SendMessage(hNumpadCombo, CB_GETCURSEL, 0, 0);
    const wchar_t* numpadModifiers[] = {L"VK_CONTROL", L"VK_LCONTROL", L"VK_RCONTROL"};
    WritePrivateProfileStringW(L"Hotkeys", L"NumpadModifier", numpadModifiers[numpadIndex], exePath);
    
    // Advanced
    BOOL debugMode = (IsDlgButtonChecked(hwnd, IDC_DEBUG_MODE_CHECK) == BST_CHECKED);
    WritePrivateProfileStringW(L"Advanced", L"DebugMode", debugMode ? L"1" : L"0", exePath);
    
    BOOL eventLogging = (IsDlgButtonChecked(hwnd, IDC_EVENT_LOGGING_CHECK) == BST_CHECKED);
    WritePrivateProfileStringW(L"Advanced", L"EventLogging", eventLogging ? L"1" : L"0", exePath);
    
    HWND hLogLevelCombo = GetDlgItem(hwnd, IDC_LOG_LEVEL_COMBO);
    int logLevelIndex = SendMessage(hLogLevelCombo, CB_GETCURSEL, 0, 0);
    const wchar_t* logLevels[] = {L"1", L"2", L"3", L"4", L"5"};
    WritePrivateProfileStringW(L"Advanced", L"LogLevel", logLevels[logLevelIndex], exePath);
    
    // Recargar configuraciones
    LoadPerformanceSettings();
}

// Función para aplicar el tema dark OLED a todos los controles
void ApplyDarkThemeToControls(HWND hwnd, HFONT hTitleFont, HFONT hLabelFont, HFONT hControlFont) {
    // Aplicar fuente del título al título principal
    HWND hTitle = GetDlgItem(hwnd, 0); // El primer control creado (título)
    if (hTitle) {
        SendMessage(hTitle, WM_SETFONT, (WPARAM)hTitleFont, TRUE);
    }
    
    // Aplicar fuentes a todos los controles según su tipo
    for (int i = IDC_COLUMNS_LABEL; i <= IDC_LOG_LEVEL_COMBO; i++) {
        HWND hControl = GetDlgItem(hwnd, i);
        if (hControl) {
            // Aplicar fuente según el tipo de control
            if (i >= IDC_COLUMNS_LABEL && i <= IDC_TRAY_ICON_COLOR_LABEL) {
                // Labels - usar fuente de etiqueta
                SendMessage(hControl, WM_SETFONT, (WPARAM)hLabelFont, TRUE);
            } else {
                // Controles de entrada - usar fuente de control
                SendMessage(hControl, WM_SETFONT, (WPARAM)hControlFont, TRUE);
            }
        }
    }
    
    // Aplicar fuentes a los botones
    SendMessage(GetDlgItem(hwnd, IDC_APPLY_BUTTON), WM_SETFONT, (WPARAM)hControlFont, TRUE);
    SendMessage(GetDlgItem(hwnd, IDC_CANCEL_BUTTON), WM_SETFONT, (WPARAM)hControlFont, TRUE);
}