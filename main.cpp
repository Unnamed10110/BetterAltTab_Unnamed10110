/*
// Procedimiento principal de la ventana del overlay (maneja todos los mensajes de Windows)
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Enumera todas las ventanas abiertas que cumplen los criterios para mostrarse en el overlay
std::vector<WindowInfo> EnumerateWindows(HWND excludeHwnd);

// Registra las miniaturas DWM de todas las ventanas para poder dibujarlas en el overlay
void RegisterThumbnails(HWND host, std::vector<WindowInfo>& windows);

// Elimina las miniaturas DWM de todas las ventanas (limpieza de recursos)
void UnregisterThumbnails(std::vector<WindowInfo>& windows);

// Guarda el orden actual de las ventanas en disco para persistencia
void SaveGridOrder(const std::vector<WindowInfo>& windows);

// Carga el orden de las ventanas desde disco (persistencia)
void LoadGridOrder(std::vector<PersistedWindow>& order);

// Aplica el orden persistente a la lista de ventanas actual
void ApplyGridOrder(std::vector<WindowInfo>& windows, const std::vector<PersistedWindow>& order);

// Dibuja el ícono de una ventana en el overlay principal
void DrawWindowIcon(HDC hdc, HWND hwnd, int x, int y, int size);

// Dibuja texto con sombra para mejor visibilidad en el overlay
void DrawTextWithShadow(HDC hdc, LPCWSTR text, RECT* rc, COLORREF color, int glowSize);

// Centra la ventana del overlay en la pantalla
void CenterOverlayWindow(HWND hwnd, int width, int height);

// Fuerza el repintado de la grilla de ventanas en el overlay
void InvalidateGrid(HWND hwnd);

// Elimina todas las miniaturas DWM registradas (limpieza global)
void UnregisterAllThumbnails();

// Cambia el Z-order de las ventanas según el orden de la grilla (lleva cada ventana al tope en orden)
void SetWindowsZOrder(const std::vector<WindowInfo>& windows);

// Callback para EnumWindows: filtra solo las ventanas visibles y válidas para el overlay
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

// Dibuja el ícono de pin en la miniatura de una ventana
void DrawPinIcon(HDC hdc, int x, int y, bool pinned);

// Dibuja el ícono de cerrar en la miniatura de una ventana
void DrawCloseIcon(HDC hdc, int x, int y, int size);

// Dibuja el ícono de fijar a posición en la miniatura de una ventana
void DrawPinToPosIcon(HDC hdc, int x, int y, int size);

// Dibuja el número sobre la miniatura de una ventana
void DrawNumberOverlay(HDC hdc, int x, int y, int number);

// Dibuja el orden de las ventanas para depuración
void DebugOrder(const std::vector<WindowInfo>& windows, int dragIdx, int hoverIdx, int insertIdx);

// Ordena las ventanas para la grilla según el modo actual
void SortWindowsForGrid(std::vector<WindowInfo>& windows);

// Procedimiento de ventana para el overlay minimalista de entrada de número (maneja mensajes)
LRESULT CALLBACK CtrlNumberOverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Muestra el overlay minimalista para ingresar un número (Ctrl)
void ShowCtrlNumberOverlay(HWND parent);

// Oculta y destruye el overlay minimalista de entrada de número
void HideCtrlNumberOverlay();

// Subclase del control de edición para capturar Enter/Escape y validar la entrada
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Escribe mensajes de depuración en un archivo de texto
void LogDebugMessage(const wchar_t* msg);






*/ */



// Habilita soporte para Unicode en todo el proyecto (funciones de Windows y C usan wchar_t)
#define UNICODE // Para funciones de la API de Windows
#define _UNICODE // Para funciones estándar de C

// ===============================
// INCLUDES DEL SISTEMA Y LIBRERÍAS
// ===============================

#include <windows.h>            // Funciones y tipos principales de la API de Windows
#include <dwmapi.h>             // Funciones para miniaturas y efectos de ventanas (DWM)
#include <windowsx.h>           // Macros útiles para manipular ventanas y mensajes
#include <vector>               // Contenedor dinámico de la STL para listas
#include <string>               // Soporte para std::string y std::wstring
#include <unordered_map>        // Mapa hash para acceso rápido por clave
#include <fstream>              // Entrada/salida de archivos (persistencia)
#include <commctrl.h>           // Controles comunes de Windows (tooltips, etc.)
#include <uxtheme.h>            // Temas visuales de Windows (bordes, estilos)
#include <map>                  // Mapa ordenado de la STL
#include <shobjidl.h>           // Interfaz para escritorios virtuales (IVirtualDesktopManager)
#include <winuser.h>            // Definiciones adicionales de la API de usuario de Windows
#include <psapi.h>              // Funciones para información de procesos
#include <strsafe.h>            // Funciones seguras para manejo de strings
#include <mutex>                // Mutex de la STL para sincronización entre hilos
#include <algorithm>            // Algoritmos estándar (sort, etc.)
#include <shellapi.h>           // Funciones para íconos de bandeja y shell
#include <shlwapi.h>            // Funciones utilitarias de shell (Path, etc.)
#include <objbase.h>            // Inicialización de COM

// ===============================
// PRAGMAS DE ENLACE DE LIBRERÍAS
// ===============================

#pragma comment(lib, "Shlwapi.lib")      // Enlaza la librería de utilidades de shell
#pragma comment(lib, "dwmapi.lib")       // Enlaza la librería de Desktop Window Manager
#pragma comment(lib, "comctl32.lib")     // Enlaza la librería de controles comunes
#pragma comment(lib, "uxtheme.lib")      // Enlaza la librería de temas visuales
#pragma comment(lib, "ole32.lib")        // Enlaza la librería de COM
#pragma comment(lib, "psapi.lib")        // Enlaza la librería de información de procesos
#pragma comment(lib, "msimg32.lib")      // Enlaza la librería para AlphaBlend (transparencias)
#pragma comment(lib, "msimg32.lib")      // (Repetido, pero sirve para AlphaBlend)

// ===============================
// DEFINES DE CONFIGURACIÓN Y ARCHIVOS
// ===============================

#define HOTKEY_ID 0xBEEF         // ID único para el hotkey principal (Alt+Q)
#define HOTKEY_ID_ALTQ 0xBEEE    // ID único para el hotkey alternativo (Ctrl+Q)
#define GRID_ORDER_FILE L"grid_order.bin" // Archivo donde se guarda el orden de las ventanas


// Paso 2: Configuramos los márgenes, tamaños y colores de la UI.
// Así logramos una apariencia moderna y cómoda para el usuario.

// ===============================
// CONFIGURACIÓN DEL OVERLAY PRINCIPAL (Alt+Q)
// ===============================

const int GRID_COLS = 4; // Columnas de la grilla (no se usa, ver FIXED_COLS)
const int GRID_PADDING_X = 48; // Margen horizontal interno del overlay principal
const int GRID_PADDING_Y = 48; // Margen vertical interno del overlay principal

// Tamaño total del overlay principal
const int OVERLAY_WIDTH = 1692; // Ancho total del overlay principal (1880 * 0.9)
const int OVERLAY_HEIGHT = 1000; // Alto total del overlay principal

// Tamaño de cada miniatura de ventana
const int PREVIEW_WIDTH = 299; // Ancho de cada miniatura de ventana
const int PREVIEW_HEIGHT = 207; // Alto de cada miniatura de ventana

// Espaciado entre miniaturas
const int PREVIEW_MARGIN = 32; // Margen entre cada miniatura de ventana

// Colores del overlay principal
const COLORREF OVERLAY_BG_COLOR = RGB(0, 0, 0); // Color de fondo del overlay principal (negro)
const BYTE OVERLAY_BG_ALPHA = 230; // Transparencia del fondo (0=transparente, 255=opaco)
const COLORREF BORDER_COLOR = RGB(32, 32, 48); // Color del borde normal de las miniaturas
const COLORREF HOVER_BORDER_COLOR = RGB(80, 180, 255); // Color del borde al pasar el mouse (azul)
const COLORREF DRAG_BORDER_COLOR = RGB(180, 140, 255); // Color del borde al arrastrar (violeta)
const COLORREF TITLE_COLOR = RGB(255,255,255); // Color del texto del título de las ventanas (blanco)
const COLORREF HIGHLIGHT_COLOR = RGB(0, 255, 68); // Color de resaltado (verde)

// Tamaños de elementos de la UI
const int ICON_SIZE = 27; // Tamaño de los íconos (pin, cerrar, etc.)
const int SCROLLBAR_HEIGHT = 12; // Alto de la barra de desplazamiento

// Configuración de la grilla
const int FIXED_COLS = 5; // Número fijo de columnas en la grilla (siempre 5 ventanas por fila)

// Espaciado entre botones
const int PIN_CLOSE_VERTICAL_GAP = 12; // Espacio vertical entre el botón de pin y el de cerrar
const int PIN_TO_POS_BUTTON_SIZE = 26; // Tamaño del botón para fijar a posición

// ===============================
// Variables globales para el manejo de estado
// ===============================

// ===============================
// VARIABLES DE ESTADO DEL OVERLAY PRINCIPAL
// ===============================

// Paso 3: Declaramos variables globales para manejar el scroll, la selección y el estado de la UI.
// Esto nos permite mantener el contexto entre eventos de usuario.

// Variables de desplazamiento horizontal
static int g_scrollX = 0; // Posición actual del scroll horizontal
static int g_scrollMax = 0; // Máximo valor del scroll horizontal
static bool g_scrolling = false; // Indica si se está realizando scroll horizontal
static int g_scrollStartX = 0; // Posición inicial del mouse al comenzar el scroll horizontal
static int g_scrollStartOffset = 0; // Offset inicial del scroll horizontal

// Variables de desplazamiento vertical
static int g_scrollY = 0; // Posición actual del scroll vertical
static int g_scrollMaxY = 0; // Máximo valor del scroll vertical
static bool g_vscrolling = false; // Indica si se está realizando scroll vertical
static int g_scrollStartY = 0; // Posición inicial del mouse al comenzar el scroll vertical
static int g_scrollStartOffsetY = 0; // Offset inicial del scroll vertical

// Variables de selección
static int g_selectedIndex = 0; // Índice de la ventana actualmente seleccionada

// Estado de arrastrar y soltar (no usado actualmente, pero preparado para futuras mejoras)
static bool g_dragging = false; // Indica si se está arrastrando una ventana
static int g_dragIndex = -1; // Índice de la ventana que se está arrastrando
static int g_hoverIndex = -1; // Índice de la ventana sobre la que está el mouse
static int g_dragMouseX = 0; // Posición X del mouse al arrastrar
static int g_dragMouseY = 0; // Posición Y del mouse al arrastrar

// Variables globales para el hover de los íconos
static int g_closeHoverIndex = -1; // Índice de la ventana cuyo botón de cerrar tiene hover
static int g_pinToPosHoverIndex = -1; // Índice de la ventana cuyo botón de pin tiene hover

// Estado del último hotkey presionado
static int g_lastHotkey = 0; // 0 = ninguno, 1 = ctrl+numpaddot, 2 = alt+q

// ===============================
// CONFIGURACIÓN DEL OVERLAY DE ENTRADA DE NÚMEROS (Ctrl)
// ===============================

// Estado del overlay de entrada de números
static bool g_ctrlNumberInputActive = false; // Indica si el overlay de entrada de números está activo
static std::wstring g_ctrlNumberBuffer; // Buffer que almacena el número ingresado por el usuario
static HWND g_ctrlNumberOverlayHwnd = nullptr; // Handle de la ventana del overlay de entrada de números

// Declaraciones adelantadas para el overlay de entrada de números con Ctrl
LRESULT CALLBACK CtrlNumberOverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ShowCtrlNumberOverlay(HWND parent);
void HideCtrlNumberOverlay();

// ===============================
// VARIABLES DE CONFIGURACIÓN DINÁMICA DEL OVERLAY PRINCIPAL
// ===============================

int g_overlayWidth = OVERLAY_WIDTH; // Ancho actual del overlay principal (puede cambiar dinámicamente)
int g_overlayHeight = OVERLAY_HEIGHT; // Alto actual del overlay principal (puede cambiar dinámicamente)

// ===============================
// Estructuras para representar ventanas y persistencia
// ===============================

// Paso 4: Creamos una estructura para guardar la información de cada ventana.
// Esto facilita el manejo de la grilla y la persistencia del estado.

// Estructura que representa una ventana mostrada en el overlay principal.
// Contiene el handle de la ventana (HWND), el título, el nombre de clase, la miniatura DWM (para mostrar vista previa) y si está fijada en la grilla.
// Se usa para manejar y dibujar cada ventana en la UI del overlay.
struct WindowInfo {
    HWND hwnd; // Handle de la ventana
    std::wstring title; // Título de la ventana
    std::wstring className; // Nombre de la clase de la ventana
    HTHUMBNAIL thumbnail = nullptr; // Miniatura DWM
    bool pinned = false; // Si está fijada en la grilla
};

// Paso 5: Estructura para guardar el orden y estado de las ventanas en disco.
// Así el usuario no pierde su configuración al reiniciar.

// Estructura para guardar en disco el estado y orden de una ventana.
// Permite que el usuario mantenga su configuración (orden, título, clase, si está fijada) entre sesiones del programa.
struct PersistedWindow {
    HWND hwnd;
    wchar_t title[256];
    wchar_t className[128];
    bool pinned = false;
};

// ===============================
// Vectores y mapas globales
// ===============================

// Paso 6: Usamos vectores y mapas para manejar las ventanas activas y las miniaturas.
// Esto nos permite acceder rápido a la información y actualizar la UI eficientemente.

// ===============================
// VARIABLES GLOBALES DE ALMACENAMIENTO Y ESTADO
// ===============================

std::vector<WindowInfo> g_windows; // Lista de todas las ventanas disponibles para mostrar en el overlay
std::map<HWND, HTHUMBNAIL> g_thumbnailMap; // Mapa que relaciona cada ventana con su miniatura DWM
std::vector<PersistedWindow> g_gridOrder; // Orden persistente de las ventanas guardado en disco
HWND g_tooltip = nullptr; // Handle del tooltip que se muestra al pasar el mouse sobre las ventanas
IVirtualDesktopManager* g_vdm = nullptr; // Interfaz para manejar escritorios virtuales de Windows
bool g_dynamicOrder = false; // Si es true, las ventanas se ordenan dinámicamente; si es false, se mantiene el orden persistente

// ===============================
// Funciones principales
// ===============================

// Paso 7: Prototipos de funciones para organizar el código y evitar errores de compilación.

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
std::vector<WindowInfo> EnumerateWindows(HWND excludeHwnd);
void RegisterThumbnails(HWND host, std::vector<WindowInfo>& windows);
void UnregisterThumbnails(std::vector<WindowInfo>& windows);
void SaveGridOrder(const std::vector<WindowInfo>& windows);
void LoadGridOrder(std::vector<PersistedWindow>& order);
void ApplyGridOrder(std::vector<WindowInfo>& windows, const std::vector<PersistedWindow>& order);
void DrawWindowIcon(HDC hdc, HWND hwnd, int x, int y, int size);
void DrawTextWithShadow(HDC hdc, LPCWSTR text, RECT* rc, COLORREF color, int glowSize);
void CenterOverlayWindow(HWND hwnd, int width, int height);
void InvalidateGrid(HWND hwnd);
void UnregisterAllThumbnails();

// Cambia el Z-order de las ventanas según el orden de la grilla
void SetWindowsZOrder(const std::vector<WindowInfo>& windows) {
    // Establece el Z-order estrictamente: lleva cada ventana al tope en orden de atrás hacia adelante
    for (int i = (int)windows.size() - 1; i >= 0; --i) {
        HWND hwnd = windows[i].hwnd;
        BOOL res = SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        // Si falla, simplemente ignoramos el error (ya no hay logging)
        (void)res; // Evitar warning de variable no usada si se compila en release
    }
}

// Enumerador de ventanas: filtra solo las visibles, del escritorio actual y que aparecen en la barra de tareas
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    HWND excludeHwnd = (HWND)lParam;
    if (hwnd == excludeHwnd) return TRUE;
    if (!IsWindowVisible(hwnd)) return TRUE;
    // Solo mostrar ventanas del escritorio virtual actual
    if (g_vdm) {
        BOOL onCurrent = FALSE;
        if (FAILED(g_vdm->IsWindowOnCurrentVirtualDesktop(hwnd, &onCurrent)) || !onCurrent)
            return TRUE;
    }
    // Excluir ventanas que no aparecen en la barra de tareas (tool windows)
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return TRUE;
    HWND owner = GetWindow(hwnd, GW_OWNER);
    if (!((exStyle & WS_EX_APPWINDOW) || (!owner && (GetWindowLongPtr(hwnd, GWL_STYLE) & WS_OVERLAPPEDWINDOW))))
        return TRUE;
    wchar_t title[256];
    GetWindowTextW(hwnd, title, 256);
    if (wcslen(title) == 0) return TRUE;
    wchar_t className[128];
    GetClassNameW(hwnd, className, 128);
    g_windows.push_back({hwnd, title, className});
    return TRUE;
}

// Devuelve el vector de ventanas activas, aplicando el orden persistente si corresponde
std::vector<WindowInfo> EnumerateWindows(HWND excludeHwnd) {
    g_windows.clear();
    EnumWindows(EnumWindowsProc, (LPARAM)excludeHwnd);
    if (!g_dynamicOrder && !g_gridOrder.empty()) {
        ApplyGridOrder(g_windows, g_gridOrder);
    }
    return g_windows;
}

// Registra miniaturas DWM para cada ventana
void RegisterThumbnails(HWND host, std::vector<WindowInfo>& windows) {
    for (auto& win : windows) {
        if (!win.thumbnail) {
            DwmRegisterThumbnail(host, win.hwnd, &win.thumbnail);
        }
    }
}

// Libera las miniaturas DWM
void UnregisterThumbnails(std::vector<WindowInfo>& windows) {
    for (auto& win : windows) {
        if (win.thumbnail) {
            DwmUnregisterThumbnail(win.thumbnail);
            win.thumbnail = nullptr;
        }
    }
    g_thumbnailMap.clear();
}

// Guarda el orden y estado de las ventanas en un archivo binario
void SaveGridOrder(const std::vector<WindowInfo>& windows) {
    std::ofstream ofs(GRID_ORDER_FILE, std::ios::binary);
    if (!ofs) return;
    
    for (const auto& win : windows) {
        PersistedWindow pw;
        pw.hwnd = win.hwnd;
        wcsncpy_s(pw.title, win.title.c_str(), 255);
        wcsncpy_s(pw.className, win.className.c_str(), 127);
        pw.pinned = win.pinned;
        ofs.write(reinterpret_cast<const char*>(&pw), sizeof(PersistedWindow));
    }
}

// Carga el orden y estado de las ventanas desde el archivo binario
void LoadGridOrder(std::vector<PersistedWindow>& order) {
    order.clear();
    std::ifstream ifs(GRID_ORDER_FILE, std::ios::binary);
    if (!ifs) return;
    
    PersistedWindow pw;
    while (ifs.read(reinterpret_cast<char*>(&pw), sizeof(PersistedWindow))) {
        order.push_back(pw);
    }
}

// Aplica el orden persistente a la lista de ventanas activas
void ApplyGridOrder(std::vector<WindowInfo>& windows, const std::vector<PersistedWindow>& order) {
    std::vector<WindowInfo> sorted;
    std::vector<bool> used(windows.size(), false);
    
    for (const auto& pw : order) {
        for (size_t i = 0; i < windows.size(); ++i) {
            if (!used[i] && windows[i].hwnd == pw.hwnd) {
                windows[i].pinned = pw.pinned;
                sorted.push_back(windows[i]);
                used[i] = true;
                break;
            }
        }
    }
    
    for (size_t i = 0; i < windows.size(); ++i) {
        if (!used[i]) sorted.push_back(windows[i]);
    }
    
    windows = std::move(sorted);
}

// Dibuja el ícono de la ventana (extrae el ícono si es necesario)
void DrawWindowIcon(HDC hdc, HWND hwnd, int x, int y, int size) {
    HICON hIcon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0);
    if (!hIcon) hIcon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_BIG, 0);
    if (!hIcon) hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM);
    if (!hIcon) hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
    bool extracted = false;
    if (!hIcon) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (hProcess) {
            wchar_t exePath[MAX_PATH] = {0};
            if (GetModuleFileNameExW(hProcess, nullptr, exePath, MAX_PATH)) {
                HICON hExtractedSmall = nullptr;
                UINT count = ExtractIconExW(exePath, 0, nullptr, &hExtractedSmall, 1);
                if (count > 0 && hExtractedSmall) {
                    hIcon = hExtractedSmall;
                    extracted = true;
                }
            }
            CloseHandle(hProcess);
        }
    }
    if (hIcon) {
        DrawIconEx(hdc, x, y, hIcon, size, size, 0, nullptr, DI_NORMAL);
        if (extracted) {
            DestroyIcon(hIcon);
        }
    }
}

// Dibuja texto con sombra/glow para mejor visibilidad
void DrawTextWithShadow(HDC hdc, LPCWSTR text, RECT* rc, COLORREF color, int glowSize) {
    DTTOPTS dtt = { sizeof(DTTOPTS) };
    dtt.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_GLOWSIZE;
    dtt.crText = color;
    dtt.iGlowSize = glowSize;
    
    HTHEME hTheme = OpenThemeData(nullptr, L"WINDOW");
    if (hTheme) {
        DrawThemeTextEx(hTheme, hdc, 0, 0, text, -1, 
                       DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS, 
                       rc, &dtt);
        CloseThemeData(hTheme);
    } else {
        SetTextColor(hdc, color);
        SetBkMode(hdc, TRANSPARENT);
        DrawTextW(hdc, text, -1, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    }
}

// Centra la ventana de superposición en la pantalla
void CenterOverlayWindow(HWND hwnd, int width, int height) {
    RECT rc;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
    int x = rc.left + (rc.right - rc.left - width) / 2;
    int y = rc.top + (rc.bottom - rc.top - height) / 2;
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height, SWP_SHOWWINDOW);
}

// Invalida la grilla para forzar el repintado
void InvalidateGrid(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    InvalidateRect(hwnd, &rc, TRUE);
}

// Registra/desregistra miniaturas solo para las ventanas visibles en la grilla
void RegisterVisibleThumbnails(HWND host, std::vector<WindowInfo>& windows, const std::vector<bool>& visible) {
    for (size_t i = 0; i < windows.size(); ++i) {
        HWND hwnd = windows[i].hwnd;
        if (visible[i]) {
            if (!g_thumbnailMap[hwnd]) {
                HTHUMBNAIL thumb = nullptr;
                if (SUCCEEDED(DwmRegisterThumbnail(host, hwnd, &thumb))) {
                    g_thumbnailMap[hwnd] = thumb;
                    windows[i].thumbnail = thumb;
                }
            } else {
                windows[i].thumbnail = g_thumbnailMap[hwnd];
            }
        } else {
            if (g_thumbnailMap[hwnd]) {
                DwmUnregisterThumbnail(g_thumbnailMap[hwnd]);
                g_thumbnailMap[hwnd] = nullptr;
                windows[i].thumbnail = nullptr;
            }
        }
    }
}

// Imprime el orden de las ventanas en el log para depuración
void DebugOrder(const std::vector<WindowInfo>& windows, int dragIdx, int hoverIdx, int insertIdx) {
    // (Eliminado: logging de depuración innecesario)
}

// Dibuja un ícono de pin simple en la esquina superior derecha de una celda
void DrawPinIcon(HDC hdc, int x, int y, bool pinned) {
    int w = 22, h = 22;
    HPEN pen = CreatePen(PS_SOLID, 2, pinned ? RGB(255, 215, 0) : RGB(180, 180, 180));
    HBRUSH brush = CreateSolidBrush(pinned ? RGB(255, 215, 0) : RGB(220, 220, 220));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    // Cabeza
    Ellipse(hdc, x, y, x + w/2, y + h/2);
    // Cuerpo
    MoveToEx(hdc, x + w/4, y + h/4, NULL);
    LineTo(hdc, x + w/4, y + h);
    // Base
    Rectangle(hdc, x + w/4 - 3, y + h - 4, x + w/4 + 3, y + h);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}

// Dibuja un ícono de cerrar (X) debajo del pin, tamaño pequeño
void DrawCloseIcon(HDC hdc, int x, int y, int size) {
    int w = size, h = size;
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(220, 80, 80));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    // Dibuja la X
    MoveToEx(hdc, x, y, NULL);
    LineTo(hdc, x + w, y + h);
    MoveToEx(hdc, x + w, y, NULL);
    LineTo(hdc, x, y + h);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

// Dibuja un ícono de '#' para el botón de fijar a posición
void DrawPinToPosIcon(HDC hdc, int x, int y, int size) {
    SetBkMode(hdc, TRANSPARENT);
    HFONT hFont = CreateFontW(size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(80, 180, 255));
    TextOutW(hdc, x, y, L"#", 1);
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
}

// Dibuja el número de orden para selección rápida por teclado
void DrawNumberOverlay(HDC hdc, int x, int y, int number) {
    wchar_t buf[4];
    wsprintfW(buf, L"%d", number); // Format number as string (handles 1-999)

    // Adjust font size and positioning based on number of digits
    int fontSize;
    int xOffset;
    
    if (number < 10) {
        // Single digit (1-9)
        fontSize = 18;
        xOffset = 6; // Center single digit
    } 
    else if (number < 100) {
        // Two digits (10-99)
        fontSize = 16;
        xOffset = 9; // Center two digits
    }
    else {
        // Three digits (100-999) - though unlikely to need this many windows
        fontSize = 14;
        xOffset = 12; // Center three digits
    }

    // Create font with appropriate size
    HFONT hFont = CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    
    if (hFont) {
        HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
        SetBkMode(hdc, TRANSPARENT);
        
        // Draw with glow effect for better visibility
        DTTOPTS dttOpts = { sizeof(DTTOPTS) };
        dttOpts.dwFlags = DTT_TEXTCOLOR | DTT_GLOWSIZE;
        dttOpts.crText = RGB(80, 180, 255); // Blueish color
        dttOpts.iGlowSize = 8; // Glow size
        
        RECT rc = { x - xOffset, y, x + 50, y + 50 }; // Large enough rect
        
        HTHEME hTheme = OpenThemeData(nullptr, L"WINDOW");
        if (hTheme) {
            DrawThemeTextEx(hTheme, hdc, 0, 0, buf, -1, 
                           DT_LEFT | DT_TOP | DT_SINGLELINE, 
                           &rc, &dttOpts);
            CloseThemeData(hTheme);
        } else {
            // Fallback if themes aren't available
            SetTextColor(hdc, dttOpts.crText);
            TextOutW(hdc, x - xOffset, y, buf, lstrlenW(buf));
        }

        SelectObject(hdc, oldFont);
        DeleteObject(hFont);
    }
}

// Ordena las ventanas: primero las fijadas (en orden de pin), luego las no fijadas
void SortWindowsForGrid(std::vector<WindowInfo>& windows) {
    std::stable_sort(windows.begin(), windows.end(), [](const WindowInfo& a, const WindowInfo& b) {
        return a.pinned > b.pinned;
    });
}

const int PERSISTENT_CONTOUR_THICKNESS = 1; // Grosor del contorno rojo en modo persistente
const int PERSISTENT_CONTOUR_COLOR = 0x00FF44; // Color del contorno en modo persistente (hexadecimal, 0xRRGGBB)

// Subclase para el campo de entrada numérica: maneja Enter/Esc
LRESULT CALLBACK NumberEditProc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN) {
        if (wParam == VK_RETURN) {
            HWND hParent = GetParent(hEdit);
            SendMessageW(hParent, WM_COMMAND, 12, 0); // OK
            return 0;
        } else if (wParam == VK_ESCAPE) {
            HWND hParent = GetParent(hEdit);
            SendMessageW(hParent, WM_COMMAND, 13, 0); // Cancelar
            return 0;
        }
    }
    return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hEdit, GWLP_USERDATA), hEdit, msg, wParam, lParam);
}

// Ventana modal Win32 pura para ingresar un número, tema negro OLED, Esc cierra, Enter es OK
int ShowNumberInputDialog(HWND parent, const wchar_t* prompt) {
    static int result = 0;
    static HWND sEdit = nullptr;
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        static HWND hEdit, hOK, hCancel;
        static WNDPROC oldEditProc = nullptr;
        switch (msg) {
        case WM_CREATE: {
            // Crea los controles de la ventana modal: texto, campo de entrada, botones
            CreateWindowW(L"STATIC", (LPCWSTR)((CREATESTRUCT*)lParam)->lpCreateParams, WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
                10, 10, 200, 20, hWnd, (HMENU)10, nullptr, nullptr);
            hEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                10, 35, 200, 24, hWnd, (HMENU)11, nullptr, nullptr);
            sEdit = hEdit;
            hOK = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                30, 70, 60, 28, hWnd, (HMENU)12, nullptr, nullptr);
            hCancel = CreateWindowW(L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE,
                120, 70, 60, 28, hWnd, (HMENU)13, nullptr, nullptr);
            // Subclasea el campo de entrada para Enter/Esc
            oldEditProc = (WNDPROC)SetWindowLongPtrW(hEdit, GWLP_WNDPROC, (LONG_PTR)NumberEditProc);
            SetWindowLongPtrW(hEdit, GWLP_USERDATA, (LONG_PTR)oldEditProc);
            break;
        }
        case WM_SHOWWINDOW:
            if (wParam && sEdit) {
                SetTimer(hWnd, 1, 10, NULL); // Timer corto para enfocar
            }
            break;
        case WM_TIMER:
            if (wParam == 1 && sEdit) {
                SetFocus(sEdit);
                SendMessageW(sEdit, EM_SETSEL, 0, -1);
                KillTimer(hWnd, 1);
            }
            break;
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORBTN: {
            // Aplica fondo negro y texto blanco a todos los controles
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, RGB(0,0,0));
            SetTextColor(hdc, RGB(255,255,255));
            static HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
            return (LRESULT)hBrush;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == 12) { // OK
                wchar_t buf[32] = {0};
                GetWindowTextW(hEdit, buf, 31);
                SetWindowLongPtrW(hWnd, GWLP_USERDATA, _wtoi(buf));
                PostMessageW(hWnd, WM_APP + 1, 0, 0);
                return 0;
            } else if (LOWORD(wParam) == 13) { // Cancelar
                SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
                PostMessageW(hWnd, WM_APP + 1, 0, 0);
                return 0;
            }
            break;
        case WM_CLOSE:
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
            PostMessageW(hWnd, WM_APP + 1, 0, 0);
            return 0;
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    };
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"NumInputDlgClass";
    wc.hbrBackground = CreateSolidBrush(RGB(0,0,0));
    RegisterClassW(&wc);
    HWND hDialog = CreateWindowExW(WS_EX_DLGMODALFRAME, wc.lpszClassName, L"Pin a position",
        WS_POPUP | WS_VISIBLE | WS_SYSMENU,
        (GetSystemMetrics(SM_CXSCREEN)-220)/2, (GetSystemMetrics(SM_CYSCREEN)-110)/2, 220, 120, parent, nullptr, wc.hInstance, (LPVOID)prompt);
    SetWindowLongPtrW(hDialog, GWLP_USERDATA, 0);
    MSG msg;
    BOOL done = FALSE;
    while (!done && GetMessageW(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_APP + 1 && msg.hwnd == hDialog) {
            done = TRUE;
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    int ret = (int)GetWindowLongPtrW(hDialog, GWLP_USERDATA);
    DestroyWindow(hDialog);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return ret;
}

#define WM_TRAYICON (WM_USER + 100)
#define ID_TRAY_SUSPEND 2001
#define ID_TRAY_EXIT 2002
#define ID_TRAY_RESTART 2003

static bool g_suspended = false; // Estado de suspensión
static NOTIFYICONDATA nid = {0};

// Función para crear un ícono cuadrado rojo en memoria
HICON CreateRedSquareIcon() {
    HDC hdc = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, 32, 32);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, bmp);
    HBRUSH brush = CreateSolidBrush(RGB(220, 0, 0));
    RECT rc = {0,0,32,32};
    FillRect(memDC, &rc, brush);
    DeleteObject(brush);
    ICONINFO ii = {0};
    ii.fIcon = TRUE;
    ii.hbmMask = bmp;
    ii.hbmColor = bmp;
    HICON hIcon = CreateIconIndirect(&ii);
    SelectObject(memDC, oldBmp);
    DeleteObject(bmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, hdc);
    return hIcon;
}

// registrar programa como tarea programada para ejecutar como admin en inicio de sesion
void RegisterRunAsAdminOnLogin() {
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string taskName = "BetterAltTab_RunAsAdmin";
    std::string checkCmd = "schtasks /Query /TN \"" + taskName + "\" >nul 2>&1";
    if (system(checkCmd.c_str()) == 0) {
        return; // la tarea ya existe
    }
    std::string cmd = "schtasks /Create /F /RL HIGHEST /SC ONLOGON /TN \"" + taskName +
                      "\" /TR \"\\\"" + exePath + "\\\"\"";
    int result = system(cmd.c_str());
    if (result != 0) {
        MessageBoxA(NULL, "Failed to create scheduled task for auto-run as admin.", "Error", MB_OK | MB_ICONERROR);
    }
}

#ifndef __IServiceProvider_INTERFACE_DEFINED__
#define __IServiceProvider_INTERFACE_DEFINED__
EXTERN_C const IID IID_IServiceProvider;
MIDL_INTERFACE("6d5140c1-7436-11ce-8034-00aa006009fa")
IServiceProvider : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE QueryService(
        /* [in] */ REFGUID guidService,
        /* [in] */ REFIID riid,
        /* [out] */ void **ppvObject) = 0;
};
#endif

const CLSID CLSID_ImmersiveShell = {0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};

// Add IVirtualDesktopNotification and NotificationService definitions
const CLSID CLSID_VirtualDesktopNotificationService = {0x0CD45E71, 0xD927, 0x4F15, {0x8B, 0x0A, 0x8F, 0xEF, 0x52, 0x53, 0x37, 0xBF}};
const IID IID_IVirtualDesktopNotification = {0xC179334C, 0x4295, 0x40D3, {0xBE, 0xA1, 0xC6, 0x54, 0xD9, 0x65, 0x60, 0x5A}};
const IID IID_IVirtualDesktopNotificationService = {0x0CD45E71, 0xD927, 0x4F15, {0x8B, 0x0A, 0x8F, 0xEF, 0x52, 0x53, 0x37, 0xBF}};

// Custom message for desktop change
#define WM_VIRTUAL_DESKTOP_CHANGED (WM_USER + 200)

// Forward declaration for hwnd
HWND g_mainHwnd = nullptr;

// Notification implementation
struct IVirtualDesktopNotification : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(void*) { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(void*, void*) { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(void*, void*) { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(void*, void*) { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(void*) { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(void* pDesktopOld, void* pDesktopNew) = 0;
};

struct IVirtualDesktopNotificationService : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Register(IVirtualDesktopNotification* pNotification, DWORD* pdwCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD dwCookie) = 0;
};

// Implementación concreta de la interfaz IVirtualDesktopNotification.
// Se encarga de manejar el evento CurrentVirtualDesktopChanged y notificar al overlay principal para que actualice la lista de ventanas cuando el usuario cambia de escritorio virtual.
// También implementa el conteo de referencias (AddRef/Release) y QueryInterface para el sistema COM de Windows.
class VirtualDesktopNotificationImpl : public IVirtualDesktopNotification {
    LONG m_refCount = 1;
public:
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_refCount); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG res = InterlockedDecrement(&m_refCount);
        if (res == 0) delete this;
        return res;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override {
        if (riid == IID_IUnknown || riid == IID_IVirtualDesktopNotification) {
            *ppvObject = static_cast<IVirtualDesktopNotification*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(void* /*pDesktopOld*/, void* /*pDesktopNew*/) override {
        if (g_mainHwnd) PostMessageW(g_mainHwnd, WM_VIRTUAL_DESKTOP_CHANGED, 0, 0);
        return S_OK;
    }
};

UINT WM_TASKBARCREATED = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    RegisterRunAsAdminOnLogin();
    const wchar_t CLASS_NAME[] = L"BetterAltTab_Unnamed10110Class";
    
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = CLASS_NAME;
    
    RegisterClassEx(&wc);
    
    // Inicializa COM y IVirtualDesktopManager
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    HRESULT hr = CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&g_vdm));
    
    // Establece el ancho de la superposición al 80% del ancho de la pantalla y el alto al 70% del alto de la pantalla
    RECT rc;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
    int screenW = rc.right - rc.left;
    int screenH = rc.bottom - rc.top;
    g_overlayWidth = (int)(screenW * 0.88);
    g_overlayHeight = (int)(screenH * 0.80);
    // Crear ventana como overlay para mayor eficiencia
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED,
        CLASS_NAME, L"BetterAltTab_Unnamed10110",
        WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, g_overlayWidth, g_overlayHeight,
        nullptr, nullptr, hInstance, nullptr);
    
    if (!hwnd) return 0;
    g_mainHwnd = hwnd; // Store for notification
    
    // Configurar ventana como overlay eficiente
    SetLayeredWindowAttributes(hwnd, 0, OVERLAY_BG_ALPHA, LWA_ALPHA);
    
    // Establece una región redondeada para la ventana de superposición
    HRGN rgn = CreateRoundRectRgn(0, 0, g_overlayWidth, g_overlayHeight, 60, 60);
    SetWindowRgn(hwnd, rgn, FALSE);
    CenterOverlayWindow(hwnd, g_overlayWidth, g_overlayHeight);
    LoadGridOrder(g_gridOrder);
    RegisterHotKey(hwnd, HOTKEY_ID, MOD_CONTROL, VK_DECIMAL);
    RegisterHotKey(hwnd, HOTKEY_ID_ALTQ, MOD_ALT, 'Q');
    ShowWindow(hwnd, SW_HIDE);
    
    // Inicializar ícono de bandeja
    HICON hIcon = CreateRedSquareIcon();
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = hIcon;
    lstrcpyW(nid.szTip, L"BetterAltTab_Unnamed10110");
    Shell_NotifyIcon(NIM_ADD, &nid);
    
    // Register IVirtualDesktopNotification (correct way)
    IServiceProvider* pServiceProvider = nullptr;
    IVirtualDesktopNotificationService* pNotificationService = nullptr;
    VirtualDesktopNotificationImpl* pNotification = nullptr;
    DWORD notificationCookie = 0;
    HRESULT hrSP = CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&pServiceProvider));
    if (SUCCEEDED(hrSP) && pServiceProvider) {
        HRESULT hrVS = pServiceProvider->QueryService(CLSID_VirtualDesktopNotificationService, IID_IVirtualDesktopNotificationService, (void**)&pNotificationService);
        if (SUCCEEDED(hrVS) && pNotificationService) {
            pNotification = new VirtualDesktopNotificationImpl();
            if (SUCCEEDED(pNotificationService->Register(pNotification, &notificationCookie))) {
                // Registered successfully
            } else {
                delete pNotification;
                pNotification = nullptr;
            }
        }
        pServiceProvider->Release();
    }
    
    WM_TASKBARCREATED = RegisterWindowMessageW(L"TaskbarCreated");
    
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    if (g_vdm) g_vdm->Release();
    CoUninitialize();
    UnregisterHotKey(hwnd, HOTKEY_ID);
    UnregisterHotKey(hwnd, HOTKEY_ID_ALTQ);
    Shell_NotifyIcon(NIM_DELETE, &nid);
    if (hIcon) DestroyIcon(hIcon);
    // Cleanup notification
    if (pNotificationService && notificationCookie)
        pNotificationService->Unregister(notificationCookie);
    if (pNotificationService)
        pNotificationService->Release();
    if (pNotification)
        pNotification->Release();
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static int lastTooltipIdx = -1;
    
    // Manejo especial para restaurar el icono de la bandeja si Explorer se reinicia
    if (msg == WM_TASKBARCREATED) {
        Shell_NotifyIcon(NIM_ADD, &nid);
        return 0;
    }
    switch (msg) {
        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                // Mostrar menú contextual
                POINT pt;
                GetCursorPos(&pt);
                HMENU hMenu = CreatePopupMenu();
                AppendMenuW(hMenu, MF_STRING | (g_suspended ? MF_CHECKED : 0), ID_TRAY_SUSPEND, g_suspended ? L"Reanudar" : L"Suspend");
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_RESTART, L"Restart");
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
                SetForegroundWindow(hwnd);
                int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(hMenu);
                if (cmd == ID_TRAY_SUSPEND) {
                    g_suspended = !g_suspended;
                } else if (cmd == ID_TRAY_EXIT) {
                    PostQuitMessage(0);
                } else if (cmd == ID_TRAY_RESTART) {
                    wchar_t path[MAX_PATH];
                    GetModuleFileNameW(NULL, path, MAX_PATH);
                    ShellExecuteW(NULL, NULL, path, NULL, NULL, SW_SHOWNORMAL);
                    PostQuitMessage(0);
                }
            }
            break;
        case WM_HOTKEY:
            if (g_suspended) return 0; // Si está suspendido, ignorar hotkeys
            if (wParam == HOTKEY_ID || wParam == HOTKEY_ID_ALTQ) {
                if (IsWindowVisible(hwnd)) {
                    // Si la superposición está abierta, actúa como Tab: mueve la selección a la siguiente miniatura
                    auto& windows = g_windows;
                    int n = (int)windows.size();
                    if (n > 0) {
                        g_selectedIndex = (g_selectedIndex + 1) % n;
                        g_hoverIndex = g_selectedIndex;
                        InvalidateGrid(hwnd);
                    }
                } else {
                    g_scrollX = 0;
                    g_scrollY = 0;
                    g_selectedIndex = 0;
                    g_hoverIndex = 0;
                    SetWindowTextW(hwnd, g_dynamicOrder ? L"BetterAltTab_Unnamed10110 [Dynamic Order]" : L"BetterAltTab_Unnamed10110 [PERSISTENT Z-ORDER MODE]");
                    // Refrescar miniaturas cada vez que se muestra la superposición
                    UnregisterAllThumbnails();
                    g_windows = EnumerateWindows(hwnd);
                    RegisterThumbnails(hwnd, g_windows);
                    ShowWindow(hwnd, SW_SHOW);
                    SetForegroundWindow(hwnd);
                    SetFocus(hwnd); // Asegura que el overlay reciba las teclas
                    // Add a small delay to ensure focus takes effect
                    Sleep(10);
                    g_lastHotkey = (wParam == HOTKEY_ID) ? 1 : 2;
                    // Iniciar timer para Alt+Q para detectar cuando se suelta Alt
                    if (wParam == HOTKEY_ID_ALTQ) {
                        SetTimer(hwnd, 100, 50, NULL); // Timer cada 50ms
                    }
                    // Start timer to check for numpad keys
                    SetTimer(hwnd, 200, 50, NULL); // Timer cada 50ms para numpad keys
                }
                return 0;
            }
            break;
        case WM_KEYUP:
            if (IsWindowVisible(hwnd) && g_lastHotkey == 1 && wParam == VK_CONTROL) {
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
                KillTimer(hwnd, 100); // Limpiar timer de Alt+Q
                return 0;
            }
            // Manejo adicional para Alt+Q: si se suelta Q mientras Alt está presionado
            else if (IsWindowVisible(hwnd) && g_lastHotkey == 2 && wParam == 'Q') {
                // Verificar si Alt sigue presionado
                if (GetAsyncKeyState(VK_MENU) & 0x8000) {
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
                    KillTimer(hwnd, 100); // Limpiar timer de Alt+Q
                    return 0;
                }
            }
            break;
        case WM_SYSKEYUP:
            if (IsWindowVisible(hwnd) && g_lastHotkey == 2 && wParam == VK_MENU) {
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
                KillTimer(hwnd, 100); // Limpiar timer de Alt+Q
                return 0;
            }
            break;
            
        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (g_scrollMaxY > 0) {
                g_scrollY -= delta / 2;
                g_scrollY = max(0, min(g_scrollY, g_scrollMaxY));
                InvalidateGrid(hwnd);
            } else if (g_scrollMax > 0) {
                g_scrollX -= delta / 2;
                g_scrollX = max(0, min(g_scrollX, g_scrollMax));
                InvalidateGrid(hwnd);
            }
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            
            auto& windows = g_windows;
            SortWindowsForGrid(windows);
            int n = (int)windows.size();
            // Calcula el área de la cuadrícula (resta el relleno de ambos lados)
            int gridAreaW = clientRect.right - 2 * GRID_PADDING_X;
            int gridAreaH = clientRect.bottom - 2 * GRID_PADDING_Y;
            int cols = FIXED_COLS;
            int rows = (n + cols - 1) / cols;
            
            int gridW = cols * PREVIEW_WIDTH + (cols - 1) * PREVIEW_MARGIN;
            int gridH = rows * PREVIEW_HEIGHT + (rows - 1) * PREVIEW_MARGIN;
            
            int startX = GRID_PADDING_X - g_scrollX;
            int startY = GRID_PADDING_Y - g_scrollY;
            
            g_scrollMax = (gridW > gridAreaW) ? (gridW - gridAreaW) : 0;
            g_scrollMaxY = (gridH > gridAreaH) ? (gridH - gridAreaH) : 0;
            
            if (g_scrollMax > 0 && y > clientRect.bottom - SCROLLBAR_HEIGHT) {
                g_scrolling = true;
                g_scrollStartX = x;
                g_scrollStartOffset = g_scrollX;
                SetCapture(hwnd);
                return 0;
            }
            
            if (g_scrollMaxY > 0 && x > clientRect.right - SCROLLBAR_HEIGHT) {
                g_vscrolling = true;
                g_scrollStartY = y;
                g_scrollStartOffsetY = g_scrollY;
                SetCapture(hwnd);
                return 0;
            }
            
            int col = (x - startX) / (PREVIEW_WIDTH + PREVIEW_MARGIN);
            int row = (y - startY) / (PREVIEW_HEIGHT + PREVIEW_MARGIN);
            int idx = row * cols + col;
            
            if (idx >= 0 && idx < n) {
                int cellX = startX + col * (PREVIEW_WIDTH + PREVIEW_MARGIN);
                int cellY = startY + row * (PREVIEW_HEIGHT + PREVIEW_MARGIN);
                
                // En WM_LBUTTONDOWN, verifica el clic en el ícono de cerrar antes de cualquier otro lógica
                const int pinSize = 22;
                const int closeSize = 14;
                if (!g_dynamicOrder) {
                    // Modo persistente: ícono de cerrar debajo del pin, ícono de pin presente
                    const int closeY = cellY + 8 + pinSize + PIN_CLOSE_VERTICAL_GAP; // 8px de espacio vertical
                    RECT closeRect = { cellX + PREVIEW_WIDTH - 28, closeY, cellX + PREVIEW_WIDTH - 28 + closeSize, closeY + closeSize };
                    if (x >= closeRect.left && x < closeRect.right && y >= closeRect.top && y < closeRect.bottom) {
                        PostMessage(windows[idx].hwnd, WM_CLOSE, 0, 0);
                        return 0;
                    }
                    // Área del ícono de pin
                    RECT pinRect = { cellX + PREVIEW_WIDTH - 36, cellY + 8, cellX + PREVIEW_WIDTH - 8, cellY + 30 };
                    if (x >= pinRect.left && x < pinRect.right && y >= pinRect.top && y < pinRect.bottom) {
                        windows[idx].pinned = !windows[idx].pinned; // Solo cambia esta ventana
                        SaveGridOrder(windows);
                        LoadGridOrder(g_gridOrder);
                        ApplyGridOrder(windows, g_gridOrder);
                        InvalidateGrid(hwnd);
                        return 0;
                    }
                    // Área del botón para fijar a posición
                    int pinToPosY = cellY + 8 + pinSize + PIN_CLOSE_VERTICAL_GAP + closeSize + PIN_CLOSE_VERTICAL_GAP;
                    RECT pinToPosRect = { cellX + PREVIEW_WIDTH - 28, pinToPosY, cellX + PREVIEW_WIDTH - 28 + PIN_TO_POS_BUTTON_SIZE, pinToPosY + PIN_TO_POS_BUTTON_SIZE };
                    if (x >= pinToPosRect.left && x < pinToPosRect.right && y >= pinToPosRect.top && y < pinToPosRect.bottom) {
                        // Muestra el diálogo de entrada para la posición (1-based)
                        int pos = ShowNumberInputDialog(hwnd, L"Pin a position (1-based):");
                        if (pos > 0) {
                            // Mueve esta ventana a la posición dada en el grupo fijado
                            // Cuenta las ventanas fijadas
                            int pinnedCount = 0;
                            for (auto& w : windows) if (w.pinned) ++pinnedCount;
                            if (!windows[idx].pinned) {
                                windows[idx].pinned = true;
                                ++pinnedCount;
                            }
                            int target = min(max(1, pos), pinnedCount) - 1; // 1-based to 0-based
                            // Elimina y vuelve a insertar en la posición objetivo entre las fijadas
                            WindowInfo win = windows[idx];
                            windows.erase(windows.begin() + idx);
                            int insertIdx = 0;
                            for (int i = 0, count = 0; i < (int)windows.size(); ++i) {
                                if (windows[i].pinned) {
                                    if (count == target) { insertIdx = i; break; }
                                    ++count;
                                }
                                if (i == (int)windows.size() - 1) insertIdx = windows.size();
                            }
                            windows.insert(windows.begin() + insertIdx, win);
                            SaveGridOrder(windows);
                            LoadGridOrder(g_gridOrder);
                            ApplyGridOrder(windows, g_gridOrder);
                            InvalidateGrid(hwnd);
                        }
                        return 0;
                    }
                } else {
                    // Modo dinámico: ícono de cerrar en la posición del pin, sin ícono de pin
                    RECT closeRect = { cellX + PREVIEW_WIDTH - 28, cellY + 8, cellX + PREVIEW_WIDTH - 28 + closeSize, cellY + 8 + closeSize };
                    if (x >= closeRect.left && x < closeRect.right && y >= closeRect.top && y < closeRect.bottom) {
                        PostMessage(windows[idx].hwnd, WM_CLOSE, 0, 0);
                        return 0;
                    }
                }
                // No arrastrar y soltar: solo enfoca la ventana al hacer clic
                if (IsIconic(windows[idx].hwnd)) {
                    ShowWindow(windows[idx].hwnd, SW_RESTORE);
                }
                AllowSetForegroundWindow(ASFW_ANY);
                if (!SetForegroundWindow(windows[idx].hwnd)) {
                    SwitchToThisWindow(windows[idx].hwnd, TRUE);
                }
                ShowWindow(hwnd, SW_HIDE);
            }
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            
            if (g_scrolling) {
                int dx = x - g_scrollStartX;
                g_scrollX = g_scrollStartOffset - dx;
                g_scrollX = max(0, min(g_scrollX, g_scrollMax));
                InvalidateGrid(hwnd);
                return 0;
            }
            
            if (g_vscrolling) {
                int dy = y - g_scrollStartY;
                g_scrollY = g_scrollStartOffsetY - dy;
                g_scrollY = max(0, min(g_scrollY, g_scrollMaxY));
                InvalidateGrid(hwnd);
                return 0;
            }
            
            // Detección de hover para el ícono de cerrar
            auto& windows = g_windows;
            SortWindowsForGrid(windows);
            int n = (int)windows.size();
            int cols = FIXED_COLS;
            int gridAreaW = g_overlayWidth - 2 * GRID_PADDING_X;
            int gridAreaH = g_overlayHeight - 2 * GRID_PADDING_Y;
            int rows = (n + cols - 1) / cols;
            int startX = GRID_PADDING_X - g_scrollX;
            int startY = GRID_PADDING_Y - g_scrollY;
            int col = (x - startX) / (PREVIEW_WIDTH + PREVIEW_MARGIN);
            int row = (y - startY) / (PREVIEW_HEIGHT + PREVIEW_MARGIN);
            int idx = row * cols + col;
            int oldCloseHover = g_closeHoverIndex;
            g_closeHoverIndex = -1;
            if (idx >= 0 && idx < n) {
                int cellX = startX + col * (PREVIEW_WIDTH + PREVIEW_MARGIN);
                int cellY = startY + row * (PREVIEW_HEIGHT + PREVIEW_MARGIN);
                const int pinSize = 22;
                const int closeSize = 14;
                if (!g_dynamicOrder) {
                    const int closeY = cellY + 8 + pinSize + PIN_CLOSE_VERTICAL_GAP; // 8px de espacio vertical
                    RECT closeRect = { cellX + PREVIEW_WIDTH - 28, closeY, cellX + PREVIEW_WIDTH - 28 + closeSize, closeY + closeSize };
                    if (x >= closeRect.left && x < closeRect.right && y >= closeRect.top && y < closeRect.bottom) {
                        g_closeHoverIndex = idx;
                    }
                } else {
                    RECT closeRect = { cellX + PREVIEW_WIDTH - 28, cellY + 8, cellX + PREVIEW_WIDTH - 28 + closeSize, cellY + 8 + closeSize };
                    if (x >= closeRect.left && x < closeRect.right && y >= closeRect.top && y < closeRect.bottom) {
                        g_closeHoverIndex = idx;
                    }
                }
            }
            if (g_closeHoverIndex != oldCloseHover) InvalidateGrid(hwnd);

            // Detección de hover para el botón de fijar a posición
            int oldPinToPosHover = g_pinToPosHoverIndex;
            g_pinToPosHoverIndex = -1;
            if (idx >= 0 && idx < n) {
                int cellX = startX + col * (PREVIEW_WIDTH + PREVIEW_MARGIN);
                int cellY = startY + row * (PREVIEW_HEIGHT + PREVIEW_MARGIN);
                const int pinSize = 22;
                const int closeSize = 14;
                if (!g_dynamicOrder) {
                    int pinToPosY = cellY + 8 + pinSize + PIN_CLOSE_VERTICAL_GAP + closeSize + PIN_CLOSE_VERTICAL_GAP;
                    RECT pinToPosRect = { cellX + PREVIEW_WIDTH - 28, pinToPosY, cellX + PREVIEW_WIDTH - 28 + PIN_TO_POS_BUTTON_SIZE, pinToPosY + PIN_TO_POS_BUTTON_SIZE };
                    if (x >= pinToPosRect.left && x < pinToPosRect.right && y >= pinToPosRect.top && y < pinToPosRect.bottom) {
                        g_pinToPosHoverIndex = idx;
                    }
                }
            }
            if (g_pinToPosHoverIndex != oldPinToPosHover) InvalidateGrid(hwnd);
            return 0;
        }
        
        case WM_LBUTTONUP:
            if (g_scrolling || g_vscrolling) {
                g_scrolling = false;
                g_vscrolling = false;
                ReleaseCapture();
                return 0;
            }
            return 0;
            
        case WM_RBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            auto& windows = g_windows;
            SortWindowsForGrid(windows);
            int n = (int)windows.size();
            int cols = FIXED_COLS;
            int gridAreaW = clientRect.right - 2 * GRID_PADDING_X;
            int gridAreaH = clientRect.bottom - 2 * GRID_PADDING_Y;
            int rows = (n + cols - 1) / cols;
            int gridW = cols * PREVIEW_WIDTH + (cols - 1) * PREVIEW_MARGIN;
            int gridH = rows * PREVIEW_HEIGHT + (rows - 1) * PREVIEW_MARGIN;
            int startX = GRID_PADDING_X - g_scrollX;
            int startY = GRID_PADDING_Y - g_scrollY;
            int col = (x - startX) / (PREVIEW_WIDTH + PREVIEW_MARGIN);
            int row = (y - startY) / (PREVIEW_HEIGHT + PREVIEW_MARGIN);
            int idx = row * cols + col;
            if (idx >= 0 && idx < n) {
                int cellX = startX + col * (PREVIEW_WIDTH + PREVIEW_MARGIN);
                int cellY = startY + row * (PREVIEW_HEIGHT + PREVIEW_MARGIN);
                RECT cellRect = { cellX, cellY, cellX + PREVIEW_WIDTH, cellY + PREVIEW_HEIGHT };
                if (x >= cellRect.left && x < cellRect.right && y >= cellRect.top && y < cellRect.bottom) {
                    if (!g_dynamicOrder) {
                        HMENU hMenu = CreatePopupMenu();
                        AppendMenuW(hMenu, MF_STRING, 1, windows[idx].pinned ? L"Despin" : L"Pin");
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
                        }
                    }
                }
            }
            return 0;
        }
        
        case WM_KEYDOWN: {
            // Debug: Log all key presses when overlay is visible
            if (IsWindowVisible(hwnd)) {
                wchar_t debugMsg[256];
                wsprintfW(debugMsg, L"WM_KEYDOWN received: wParam=%d, lParam=%08X\n", wParam, lParam);
                OutputDebugStringW(debugMsg);
            }
            
            // Handle numpad numbers for window focus
            if (IsWindowVisible(hwnd) && wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9) {
                // Debug: Log the key press
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
                    int cols = FIXED_COLS;
                    int rows = (n + cols - 1) / cols;
                    int row = g_selectedIndex / cols;
                    int col = g_selectedIndex % cols;
                    int gridAreaW = g_overlayWidth - 2 * GRID_PADDING_X;
                    int gridAreaH = g_overlayHeight - 2 * GRID_PADDING_Y;
                    int gridW = cols * PREVIEW_WIDTH + (cols - 1) * PREVIEW_MARGIN;
                    int gridH = rows * PREVIEW_HEIGHT + (rows - 1) * PREVIEW_MARGIN;
                    int startX = GRID_PADDING_X - g_scrollX;
                    int startY = GRID_PADDING_Y - g_scrollY;
                    auto scrollToSelected = [&]() {
                        int selX = startX + col * (PREVIEW_WIDTH + PREVIEW_MARGIN);
                        int selY = startY + row * (PREVIEW_HEIGHT + PREVIEW_MARGIN);
                        // Horizontal
                        if (selX < GRID_PADDING_X) g_scrollX = max(0, g_scrollX - (GRID_PADDING_X - selX));
                        else if (selX + PREVIEW_WIDTH > g_overlayWidth - GRID_PADDING_X) g_scrollX = min(g_scrollMax, g_scrollX + (selX + PREVIEW_WIDTH - (g_overlayWidth - GRID_PADDING_X)));
                        // Vertical
                        if (selY < GRID_PADDING_Y) g_scrollY = max(0, g_scrollY - (GRID_PADDING_Y - selY));
                        else if (selY + PREVIEW_HEIGHT > g_overlayHeight - GRID_PADDING_Y) g_scrollY = min(g_scrollMaxY, g_scrollY + (selY + PREVIEW_HEIGHT - (g_overlayHeight - GRID_PADDING_Y)));
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
                        else g_selectedIndex = (col < n % cols) ? (rows - 1) * cols + col : (rows - 2) * cols + col;
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
            // Si no está Alt presionado, ignorar las flechas
            if (wParam == VK_ESCAPE) {
                ShowWindow(hwnd, SW_HIDE);
                KillTimer(hwnd, 100);
                KillTimer(hwnd, 200);
                return 0;
            }
            // Alterna el orden dinámico/persistente con F2
            if (wParam == VK_F2) {
                g_dynamicOrder = !g_dynamicOrder;
                SetWindowTextW(hwnd, g_dynamicOrder ? L"BetterAltTab_Unnamed10110 [Dynamic Order]" : L"BetterAltTab_Unnamed10110 [PERSISTENT Z-ORDER MODE]");
                InvalidateGrid(hwnd);
                return 0;
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
                    int cols = FIXED_COLS;
                    int rows = (n + cols - 1) / cols;
                    int row = g_selectedIndex / cols;
                    int col = g_selectedIndex % cols;
                    int gridAreaW = g_overlayWidth - 2 * GRID_PADDING_X;
                    int gridAreaH = g_overlayHeight - 2 * GRID_PADDING_Y;
                    int gridW = cols * PREVIEW_WIDTH + (cols - 1) * PREVIEW_MARGIN;
                    int gridH = rows * PREVIEW_HEIGHT + (rows - 1) * PREVIEW_MARGIN;
                    int startX = GRID_PADDING_X - g_scrollX;
                    int startY = GRID_PADDING_Y - g_scrollY;
                    auto scrollToSelected = [&]() {
                        int selX = startX + col * (PREVIEW_WIDTH + PREVIEW_MARGIN);
                        int selY = startY + row * (PREVIEW_HEIGHT + PREVIEW_MARGIN);
                        if (selX < GRID_PADDING_X) g_scrollX = max(0, g_scrollX - (GRID_PADDING_X - selX));
                        else if (selX + PREVIEW_WIDTH > g_overlayWidth - GRID_PADDING_X) g_scrollX = min(g_scrollMax, g_scrollX + (selX + PREVIEW_WIDTH - (g_overlayWidth - GRID_PADDING_X)));
                        if (selY < GRID_PADDING_Y) g_scrollY = max(0, g_scrollY - (GRID_PADDING_Y - selY));
                        else if (selY + PREVIEW_HEIGHT > g_overlayHeight - GRID_PADDING_Y) g_scrollY = min(g_scrollMaxY, g_scrollY + (selY + PREVIEW_HEIGHT - (g_overlayHeight - GRID_PADDING_Y)));
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
                        else g_selectedIndex = (col < n % cols) ? (rows - 1) * cols + col : (rows - 2) * cols + col;
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
            break;
        }
        
     
        case WM_CHAR:
    if (IsWindowVisible(hwnd)) {
        int n = (int)g_windows.size();
        // Remove numpad number handling from here
        if (wParam == VK_ESCAPE) {
            ShowWindow(hwnd, SW_HIDE);
            KillTimer(hwnd, 100);
            KillTimer(hwnd, 200);
            return 0;
        }
    }
    return 0;
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            
            // Crear DC compatible con transparencia para overlay
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBM = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
            HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);
            
            // Fondo transparente para overlay eficiente
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, OVERLAY_BG_ALPHA, 0 };
            HBRUSH bgBrush = CreateSolidBrush(OVERLAY_BG_COLOR);
            FillRect(memDC, &clientRect, bgBrush);
            DeleteObject(bgBrush);
            
            auto& windows = EnumerateWindows(hwnd);
            SortWindowsForGrid(windows);
            int n = (int)windows.size();
            int cols = FIXED_COLS;
            int gridAreaW = clientRect.right - 2 * GRID_PADDING_X;
            int gridAreaH = clientRect.bottom - 2 * GRID_PADDING_Y;
            int rows = (n + cols - 1) / cols;
            int gridW = cols * PREVIEW_WIDTH + (cols - 1) * PREVIEW_MARGIN;
            int gridH = rows * PREVIEW_HEIGHT + (rows - 1) * PREVIEW_MARGIN;
            int startX = GRID_PADDING_X - g_scrollX;
            int startY = GRID_PADDING_Y - g_scrollY;
            g_scrollMax = (gridW > gridAreaW) ? (gridW - gridAreaW) : 0;
            g_scrollMaxY = (gridH > gridAreaH) ? (gridH - gridAreaH) : 0;
            int firstCol = max(0, g_scrollX / (PREVIEW_WIDTH + PREVIEW_MARGIN));
            int lastCol = min(cols - 1, (g_scrollX + gridAreaW) / (PREVIEW_WIDTH + PREVIEW_MARGIN));
            int firstRow = max(0, g_scrollY / (PREVIEW_HEIGHT + PREVIEW_MARGIN));
            int lastRow = min(rows - 1, (g_scrollY + gridAreaH) / (PREVIEW_HEIGHT + PREVIEW_MARGIN));
            std::vector<bool> visible(n, false);
            for (int r = firstRow; r <= lastRow; ++r) {
                for (int c = firstCol; c <= lastCol; ++c) {
                    int idx = r * cols + c;
                    if (idx >= n) continue;
                    int x = startX + c * (PREVIEW_WIDTH + PREVIEW_MARGIN);
                    int y = startY + r * (PREVIEW_HEIGHT + PREVIEW_MARGIN);
                    if (x + PREVIEW_WIDTH < GRID_PADDING_X || x > clientRect.right - GRID_PADDING_X ||
                        y + PREVIEW_HEIGHT < GRID_PADDING_Y || y > clientRect.bottom - GRID_PADDING_Y)
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
                    int x = startX + c * (PREVIEW_WIDTH + PREVIEW_MARGIN);
                    int y = startY + r * (PREVIEW_HEIGHT + PREVIEW_MARGIN);
                    if (x + PREVIEW_WIDTH < GRID_PADDING_X || x > clientRect.right - GRID_PADDING_X ||
                        y + PREVIEW_HEIGHT < GRID_PADDING_Y || y > clientRect.bottom - GRID_PADDING_Y)
                        continue;
                    // Dibuja el espacio/resaltado en el destino de soltado
                    if (g_dragging && idx == g_hoverIndex) {
                        RECT gapRect = { x-4, y-4, x + PREVIEW_WIDTH+4, y + PREVIEW_HEIGHT+4 };
                        HBRUSH gapBrush = CreateSolidBrush(RGB(0,255,68));
                        FrameRect(memDC, &gapRect, gapBrush);
                        DeleteObject(gapBrush);
                    }
                    RECT cellRect = { x, y, x + PREVIEW_WIDTH, y + PREVIEW_HEIGHT };
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
                    RoundRect(memDC, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 40, 40);
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
                int dragX = g_dragMouseX - PREVIEW_WIDTH / 2;
                int dragY = g_dragMouseY - PREVIEW_HEIGHT / 2;
                RECT cellRect = { dragX, dragY, dragX + PREVIEW_WIDTH, dragY + PREVIEW_HEIGHT };
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
            // Usar AlphaBlend para overlay eficiente con transparencia
            AlphaBlend(hdc, 0, 0, clientRect.right, clientRect.bottom, 
                      memDC, 0, 0, clientRect.right, clientRect.bottom, blend);
            SelectObject(memDC, oldBM);
            DeleteObject(memBM);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_DESTROY:
            UnregisterThumbnails(g_windows);
            PostQuitMessage(0);
            return 0;
            
        case WM_ACTIVATE:
            if (wParam == WA_INACTIVE) {
                ShowWindow(hwnd, SW_HIDE);
                KillTimer(hwnd, 100); // Limpiar timer de Alt+Q
                KillTimer(hwnd, 200); // Limpiar timer de numpad keys
                return 0;
            }
            break;
        case WM_KILLFOCUS:
            ShowWindow(hwnd, SW_HIDE);
            KillTimer(hwnd, 100); // Limpiar timer de Alt+Q
            KillTimer(hwnd, 200); // Limpiar timer de numpad keys
            return 0;
        case WM_VIRTUAL_DESKTOP_CHANGED:
            // Reload window list and UI, but do NOT reload persistent data
            InvalidateGrid(hwnd);
            break;
            
        case WM_TIMER:
            // Timer para detectar cuando se suelta Alt después de Alt+Q
            if (wParam == 100 && g_lastHotkey == 2) {
                // Check for numpad keys using GetAsyncKeyState
                for (int i = VK_NUMPAD0; i <= VK_NUMPAD9; i++) {
                    if (GetAsyncKeyState(i) & 0x8000) {
                        // Numpad key is pressed
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
                // Verificar overlay de entrada de números con Ctrl
                static bool ctrlWasDown = false;
                bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
                if (ctrlDown && !ctrlWasDown && !g_ctrlNumberInputActive) {
                    // Ctrl presionado: mostrar overlay
                    g_ctrlNumberInputActive = true;
                    g_ctrlNumberBuffer.clear();
                    ShowCtrlNumberOverlay(hwnd);
                }
                ctrlWasDown = ctrlDown;
                
                // Si el overlay de Ctrl está activo, solo mantenerlo abierto (la entrada la maneja el control de edición)
                if (g_ctrlNumberInputActive && g_ctrlNumberOverlayHwnd) {
                    // El control de edición maneja toda la entrada, así que no necesitamos hacer nada aquí
                }
                
                // Original numpad key handling (only if Ctrl overlay is not active)
                if (!g_ctrlNumberInputActive) {
                    for (int i = VK_NUMPAD0; i <= VK_NUMPAD9; i++) {
                        if (GetAsyncKeyState(i) & 0x8000) {
                            // Numpad key is pressed
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
}

// ===============================
// VARIABLES DE LOGGING Y DEPURACIÓN
// ===============================

std::mutex g_logMutex; // Mutex para proteger la escritura concurrente de logs de depuración
// Escribe mensajes de depuración en un archivo
void LogDebugMessage(const wchar_t* msg) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    std::wofstream logFile(L"debug_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << msg;
        logFile.flush();
    }
}

// ===============================
// Extra overlay for Ctrl+number input
// ===============================

// Forward declaration
LRESULT CALLBACK CtrlNumberOverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Procedimiento de subclase para el control de edición
WNDPROC g_oldEditProc = nullptr;

LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN) {
        if (wParam == VK_RETURN) {
            // Enter presionado - obtener texto y enfocar ventana
            wchar_t buffer[16];
            GetWindowTextW(hwnd, buffer, 16);
            g_ctrlNumberBuffer = buffer;
            
            // Debug: Registrar el número
            wchar_t debugMsg[256];
            wsprintfW(debugMsg, L"Enter presionado, número: %s\n", g_ctrlNumberBuffer.c_str());
            OutputDebugStringW(debugMsg);
            
            int n = (int)g_windows.size();
            int num = 0;
            if (!g_ctrlNumberBuffer.empty()) {
                try {
                    num = std::stoi(g_ctrlNumberBuffer);
                } catch (...) { num = 0; }
            }
            
            // Debug: Registrar el número parseado y cantidad de ventanas
            wsprintfW(debugMsg, L"Número parseado: %d, total de ventanas: %d\n", num, n);
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
                
                // Debug: Registrar éxito
                OutputDebugStringW(L"Ventana enfocada exitosamente\n");
            } else {
                // Debug: Registrar fallo
                wsprintfW(debugMsg, L"Número inválido: %d (rango válido: 1-%d)\n", num, n);
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
    
    // ===============================
    // CONFIGURACIÓN DE TAMAÑO Y POSICIÓN DEL OVERLAY DE ENTRADA
    // ===============================
    int width = 80, height = 44; // Tamaño del overlay de entrada de números (ancho x alto en píxeles)
    int x = rc.left + ((rc.right - rc.left) - width) / 2; // Posición X centrada respecto al overlay principal
    int y = rc.top + ((rc.bottom - rc.top) - height) / 2; // Posición Y centrada respecto al overlay principal
    g_ctrlNumberOverlayHwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED,
        wc.lpszClassName, L"",
        WS_POPUP | WS_BORDER,
        x, y, width, height,
        nullptr, nullptr, wc.hInstance, nullptr); // Sin ventana padre
    SetLayeredWindowAttributes(g_ctrlNumberOverlayHwnd, 0, 255, LWA_ALPHA); // Negro puro, completamente opaco
    
    // Esquinas redondeadas
    HRGN rgn = CreateRoundRectRgn(0, 0, width, height, 16, 16);
    SetWindowRgn(g_ctrlNumberOverlayHwnd, rgn, TRUE);
    
    // ===============================
    // CONFIGURACIÓN DEL CONTROL DE EDICIÓN
    // ===============================
    
    // Crear control de edición para entrada de números, centrado, texto blanco, fondo negro
    HWND hEdit = CreateWindowExW(
        0, // Sin estilo extendido
        L"EDIT", L"", // Clase EDIT, sin texto inicial
        WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER, // Estilos: hijo, visible, texto centrado, solo números
        4, 4, width-8, height-8, // Posición y tamaño del control de edición (con margen de 4px)
        g_ctrlNumberOverlayHwnd, (HMENU)1001, wc.hInstance, nullptr);
    
    // Configuración de la fuente del control de edición
    HFONT hFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE); // Aplicar fuente al control
    
    // Configuración adicional del control de edición
    SendMessage(hEdit, EM_SETSEL, 0, -1); // Seleccionar todo el texto inicial
    SendMessage(hEdit, EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, MAKELPARAM(2,2)); // Márgenes internos de 2px
    SendMessage(hEdit, EM_SETLIMITTEXT, 3, 0); // Limitar a máximo 3 caracteres (para números del 1-999)
    // Subclasificar el control de edición para manejar la tecla Enter
    g_oldEditProc = (WNDPROC)SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
    ShowWindow(g_ctrlNumberOverlayHwnd, SW_SHOW);
    SetForegroundWindow(g_ctrlNumberOverlayHwnd); // Traer al frente
    SetFocus(hEdit); // Enfocar el control de edición
    UpdateWindow(g_ctrlNumberOverlayHwnd);
}

void HideCtrlNumberOverlay() {
    if (g_ctrlNumberOverlayHwnd) {
        DestroyWindow(g_ctrlNumberOverlayHwnd);
        g_ctrlNumberOverlayHwnd = nullptr;
    }
    g_ctrlNumberInputActive = false;
    g_ctrlNumberBuffer.clear();
}

LRESULT CALLBACK CtrlNumberOverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        // Fondo negro puro OLED, sin borde, sin texto
        HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rc, bgBrush);
        DeleteObject(bgBrush);
        // Dibujar borde blanco para visibilidad
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
        // Limpiar cuando se destruye la ventana
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