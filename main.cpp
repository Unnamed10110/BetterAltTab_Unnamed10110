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
#define GRID_ORDER_FILE L"grid_order.bin" // Archivo donde guardamos el orden de las ventanas.

#define ID_TRAY_SETTINGS 2004
#define IDC_COLUMNS_LABEL 3001
#define IDC_COLUMNS_EDIT 3002
#define IDC_APPLY_BUTTON 3003

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
const BYTE OVERLAY_BG_ALPHA = 230; // Transparencia del overlay (0 = invisible, 255 = opaco).
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

void LoadConfiguration()
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    wcscat_s(exePath, L"\\BetterAltTab.ini");

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
void DrawWindowIcon(HDC hdc, HWND hwnd, int x, int y, int size); // Dibuja el ícono de una ventana.
void DrawTextWithShadow(HDC hdc, LPCWSTR text, RECT* rc, COLORREF color, int glowSize); // Dibuja texto con sombra.
void CenterOverlayWindow(HWND hwnd, int width, int height); // Centra la ventana del overlay en la pantalla.
void InvalidateGrid(HWND hwnd); // Le dice a Windows que redibuje la grilla.
void UnregisterAllThumbnails(); // Borra todas las miniaturas de una vez.
void ShowSettingsDialog(HWND parent); // <-- Forward declaration

// Esta función cambia el orden de las ventanas en la pantalla (Z-order).
// Es como when you put one sheet on top of another: the one on top is seen first.
void SetWindowsZOrder(const std::vector<WindowInfo>& windows) {
    // Recorremos las ventanas de atrás hacia adelante para que queden en el orden correcto.
    // Es como apilar cartas: la última que ponés queda arriba.
    for (int i = (int)windows.size() - 1; i >= 0; --i) {
        HWND hwnd = windows[i].hwnd; // Agarramos el handle de la ventana.
        BOOL res = SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); // La ponemos arriba de todo.
        // Si falla, simplemente ignoramos el error (ya no hay logging).
        (void)res; // Evitar warning de variable no usada si se compila en release.
    }
}

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
    // Excluir ventanas que no aparecen en la barra de tareas (tool windows).
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE); // Estilos extendidos de la ventana.
    if (exStyle & WS_EX_TOOLWINDOW) return TRUE; // Si es una tool window, la saltamos.
    HWND owner = GetWindow(hwnd, GW_OWNER); // Dueño de la ventana.
    if (!((exStyle & WS_EX_APPWINDOW) || (!owner && (GetWindowLongPtr(hwnd, GWL_STYLE) & WS_OVERLAPPEDWINDOW))))
        return TRUE; // Si no aparece en la barra de tareas, la saltamos.
    wchar_t title[256]; // Buffer para el título.
    GetWindowTextW(hwnd, title, 256); // Sacamos el título de la ventana.
    if (wcslen(title) == 0) return TRUE; // Si no tiene título, la saltamos.
    wchar_t className[128]; // Buffer para la clase.
    GetClassNameW(hwnd, className, 128); // Sacamos la clase de la ventana.
    g_windows.push_back({hwnd, title, className}); // La agregamos a nuestra lista.
    return TRUE; // Seguimos buscando más ventanas.
}

// Esta función busca todas las ventanas abiertas y las devuelve en una lista.
// También aplica el orden guardado si existe.
std::vector<WindowInfo> EnumerateWindows(HWND excludeHwnd) {
    g_windows.clear(); // Limpiamos la lista anterior.
    EnumWindows(EnumWindowsProc, (LPARAM)excludeHwnd); // Buscamos todas las ventanas.
    if (!g_dynamicOrder && !g_gridOrder.empty()) { // Si tenemos un orden guardado y no es dinámico.
        ApplyGridOrder(g_windows, g_gridOrder); // Aplicamos el orden guardado.
    }
    return g_windows; // Devolvemos la lista de ventanas.
}

// Esta función crea las miniaturas (imágenes chiquitas) de todas las ventanas.
// Es como sacar una foto de cada ventana para mostrarla en nuestra grilla.
void RegisterThumbnails(HWND host, std::vector<WindowInfo>& windows) {
    for (auto& win : windows) { // Recorremos cada ventana.
        if (!win.thumbnail) { // Si no tiene miniatura todavía.
            DwmRegisterThumbnail(host, win.hwnd, &win.thumbnail); // Creamos la miniatura.
        }
    }
}

// Esta función borra todas las miniaturas cuando no las necesitamos más.
// Es como limpiar la memoria para que no se llene.
void UnregisterThumbnails(std::vector<WindowInfo>& windows) {
    for (auto& win : windows) { // Recorremos cada ventana.
        if (win.thumbnail) { // Si tiene miniatura.
            DwmUnregisterThumbnail(win.thumbnail); // La borramos.
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
    DTTOPTS dtt = { sizeof(DTTOPTS) }; // Opciones para el texto con tema.
    dtt.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_GLOWSIZE; // Activamos composición, color y resplandor.
    dtt.crText = color; // Color del texto.
    dtt.iGlowSize = glowSize; // Tamaño del resplandor.
    
    HTHEME hTheme = OpenThemeData(nullptr, L"WINDOW"); // Abrimos el tema de Windows.
    if (hTheme) { // Si pudimos abrir el tema.
        DrawThemeTextEx(hTheme, hdc, 0, 0, text, -1, // Dibujamos el texto con el tema.
                       DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS, // Centrado, una línea, con puntos si es muy largo.
                       rc, &dtt);
        CloseThemeData(hTheme); // Cerramos el tema.
    } else { // Si no pudimos abrir el tema.
        SetTextColor(hdc, color); // Ponemos el color del texto.
        SetBkMode(hdc, TRANSPARENT); // Fondo transparente.
        DrawTextW(hdc, text, -1, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS); // Dibujamos el texto normal.
    }
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
        
        // Dibujamos con efecto de resplandor para mejor visibilidad.
        DTTOPTS dttOpts = { sizeof(DTTOPTS) }; // Opciones para el texto con tema.
        dttOpts.dwFlags = DTT_TEXTCOLOR | DTT_GLOWSIZE; // Activamos color y resplandor.
        dttOpts.crText = RGB(80, 180, 255); // Color azulado.
        dttOpts.iGlowSize = 8; // Tamaño del resplandor.
        
        RECT rc = { x - xOffset, y, x + 50, y + 50 }; // Rectángulo suficientemente grande para el texto.
        
        HTHEME hTheme = OpenThemeData(nullptr, L"WINDOW"); // Abrimos el tema de Windows.
        if (hTheme) { // Si pudimos abrir el tema.
            DrawThemeTextEx(hTheme, hdc, 0, 0, buf, -1, // Dibujamos el texto con el tema.
                           DT_LEFT | DT_TOP | DT_SINGLELINE, // Alineado a la izquierda y arriba.
                           &rc, &dttOpts);
            CloseThemeData(hTheme); // Cerramos el tema.
        } else { // Si no pudimos abrir el tema.
            // Alternativa si los temas no están disponibles.
            SetTextColor(hdc, dttOpts.crText); // Ponemos el color del texto.
            TextOutW(hdc, x - xOffset, y, buf, lstrlenW(buf)); // Dibujamos el texto normal.
        }

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
        WS_EX_TOPMOST | WS_EX_LAYERED, // Estilos extendidos: siempre arriba y con capas.
        CLASS_NAME, L"BetterAltTab_Unnamed10110", // Clase y título de la ventana.
        WS_POPUP, // Estilo de ventana: popup (sin bordes).
        CW_USEDEFAULT, CW_USEDEFAULT, g_overlayWidth, g_overlayHeight, // Posición y tamaño.
        nullptr, nullptr, hInstance, nullptr); // Parámetros adicionales.
    
    if (!hwnd) return 0; // Si no se pudo crear la ventana, salimos.
    g_mainHwnd = hwnd; // Guardamos el handle para las notificaciones.
    
    // Configuramos la ventana como overlay eficiente.
    SetLayeredWindowAttributes(hwnd, 0, g_overlayAlpha, LWA_ALPHA); // Establecemos la transparencia con config.
    
    // Establecemos una región redondeada para la ventana de superposición.
    HRGN rgn = CreateRoundRectRgn(0, 0, g_overlayWidth, g_overlayHeight, 60, 60); // Región redondeada de 60x60 píxeles.
    SetWindowRgn(hwnd, rgn, FALSE); // Aplicamos la región redondeada a la ventana.
    CenterOverlayWindow(hwnd, g_overlayWidth, g_overlayHeight); // Centramos la ventana en la pantalla.
    LoadGridOrder(g_gridOrder); // Cargamos el orden guardado de las ventanas.
    RegisterHotKey(hwnd, HOTKEY_ID, MOD_CONTROL, VK_DECIMAL); // Registramos el atajo Ctrl+Numpad.
    RegisterHotKey(hwnd, HOTKEY_ID_ALTQ, MOD_ALT, 'Q'); // Registramos el atajo Alt+Q.
    ShowWindow(hwnd, SW_HIDE); // Ocultamos la ventana al inicio.
    
    // Inicializamos el ícono de la bandeja del sistema.
    HICON hIcon = CreateRedSquareIcon(); // Creamos el ícono rojo cuadrado.
    nid.cbSize = sizeof(nid); // Tamaño de la estructura.
    nid.hWnd = hwnd; // Handle de la ventana.
    nid.uID = 1; // ID del ícono.
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; // Flags: ícono, mensaje y tooltip.
    nid.uCallbackMessage = WM_TRAYICON; // Mensaje que se envía cuando se hace clic.
    nid.hIcon = hIcon; // El ícono que creamos.
    lstrcpyW(nid.szTip, L"BetterAltTab_Unnamed10110"); // Texto del tooltip.
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
    UnregisterHotKey(hwnd, HOTKEY_ID); // Desregistramos el atajo Ctrl+Numpad.
    UnregisterHotKey(hwnd, HOTKEY_ID_ALTQ); // Desregistramos el atajo Alt+Q.
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
                AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenuW(hMenu, MF_STRING | (g_suspended ? MF_CHECKED : 0), ID_TRAY_SUSPEND, g_suspended ? L"Reanudar" : L"Suspend"); // Opción de suspender/reanudar.
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_RESTART, L"Restart"); // Opción de reiniciar.
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit"); // Opción de salir.
                SetForegroundWindow(hwnd); // Ponemos nuestra ventana al frente.
                int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL); // Mostramos el menú y obtenemos la selección.
                DestroyMenu(hMenu); // Destruimos el menú.
                if (cmd == ID_TRAY_SETTINGS) {
                    ShowSettingsDialog(hwnd);
                } else if (cmd == ID_TRAY_SUSPEND) { // Si eligieron suspender.
                    g_suspended = !g_suspended; // Cambiamos el estado de suspensión.
                } else if (cmd == ID_TRAY_EXIT) { // Si eligieron salir.
                    PostQuitMessage(0); // Enviamos mensaje para salir.
                } else if (cmd == ID_TRAY_RESTART) { // Si eligieron reiniciar.
                    wchar_t path[MAX_PATH]; // Buffer para la ruta del ejecutable.
                    GetModuleFileNameW(NULL, path, MAX_PATH); // Obtenemos la ruta del ejecutable actual.
                    ShellExecuteW(NULL, NULL, path, NULL, NULL, SW_SHOWNORMAL); // Ejecutamos una nueva instancia.
                    PostQuitMessage(0); // Salimos de la instancia actual.
                }
            }
            break;
        case WM_HOTKEY: // Mensaje cuando se presiona un atajo de teclado registrado.
            if (g_suspended) return 0; // Si está suspendido, ignoramos los atajos.
            if (wParam == HOTKEY_ID || wParam == HOTKEY_ID_ALTQ) { // Si es nuestro atajo (Ctrl+Numpad o Alt+Q).
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
                    UnregisterAllThumbnails(); // Borramos todas las miniaturas anteriores.
                    g_windows = EnumerateWindows(hwnd); // Enumeramos las ventanas actuales.
                    RegisterThumbnails(hwnd, g_windows); // Creamos las nuevas miniaturas.
                    ShowWindow(hwnd, SW_SHOW); // Mostramos la superposición.
                    SetForegroundWindow(hwnd); // La ponemos al frente.
                    SetFocus(hwnd); // Aseguramos que el overlay reciba las teclas.
                    // Agregamos un pequeño delay para asegurar que el foco tome efecto.
                    Sleep(10); // Esperamos 10 milisegundos.
                    g_lastHotkey = (wParam == HOTKEY_ID) ? 1 : 2; // Guardamos qué atajo se usó.
                    // Iniciamos timer para Alt+Q para detectar cuando se suelta Alt.
                    if (wParam == HOTKEY_ID_ALTQ) { // Si fue Alt+Q.
                        SetTimer(hwnd, 100, 50, NULL); // Timer cada 50ms.
                    }
                    // Iniciamos timer para verificar teclas del numpad.
                    SetTimer(hwnd, 200, 50, NULL); // Timer cada 50ms para teclas del numpad.
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
                ShowWindow(hwnd, SW_HIDE); // Ocultamos la superposición.
                g_lastHotkey = 0; // Reseteamos el último atajo.
                KillTimer(hwnd, 100); // Limpiamos el timer de Alt+Q.
                return 0; // No procesamos más.
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
                    
                    // Si el clic fue en la miniatura, la seleccionamos.
                    g_selectedIndex = i; // Seleccionamos la ventana.
                    g_hoverIndex = i; // El hover va a la misma ventana.
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
        // Remover el manejo de numpad number desde aquí
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
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, g_overlayAlpha, 0 };
            HBRUSH bgBrush = CreateSolidBrush(OVERLAY_BG_COLOR);
            FillRect(memDC, &clientRect, bgBrush);
            DeleteObject(bgBrush);
            
            auto& windows = EnumerateWindows(hwnd);
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
            // Recargar la lista de ventanas y la UI, pero NO recargar los datos persistentes
            InvalidateGrid(hwnd);
            break;
            
        case WM_TIMER:
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
    switch (msg) {
        case WM_CREATE: {
            CreateWindowW(L"STATIC", L"Columns:", WS_CHILD | WS_VISIBLE, 10, 10, 100, 20, hwnd, (HMENU)IDC_COLUMNS_LABEL, nullptr, nullptr);
            wchar_t currentCols[4];
            _itow_s(g_fixedCols, currentCols, 4, 10);
            CreateWindowW(L"EDIT", currentCols, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 120, 10, 50, 20, hwnd, (HMENU)IDC_COLUMNS_EDIT, nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Apply", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 80, 50, 80, 25, hwnd, (HMENU)IDC_APPLY_BUTTON, nullptr, nullptr);
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == IDC_APPLY_BUTTON) {
                wchar_t newColsStr[4];
                GetDlgItemTextW(hwnd, IDC_COLUMNS_EDIT, newColsStr, 4);
                int newCols = _wtoi(newColsStr);
                if (newCols > 0 && newCols < 20) { 
                    g_fixedCols = newCols;
                    
                    wchar_t exePath[MAX_PATH];
                    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
                    PathRemoveFileSpecW(exePath);
                    wcscat_s(exePath, L"\\BetterAltTab.ini");
                    WritePrivateProfileStringW(L"General", L"Columns", newColsStr, exePath);
                    
                    UpdatePreviewSize();
                    DestroyWindow(hwnd);
                } else {
                    MessageBoxW(hwnd, L"Please enter a number between 1 and 19.", L"Invalid Input", MB_OK | MB_ICONEXCLAMATION);
                }
            }
            return 0;
        }
        case WM_CLOSE: {
            DestroyWindow(hwnd);
            return 0;
        }
        case WM_DESTROY: {
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
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, SETTINGS_CLASS_NAME, L"Settings",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 250, 120,
        parent, nullptr, wc.hInstance, nullptr);

    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }
}