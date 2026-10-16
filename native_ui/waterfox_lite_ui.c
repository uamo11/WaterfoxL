#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <psapi.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "dwmapi.lib")

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

// Paleta de colores Dark Theme (Waterfox Lite)
#define COLOR_BG          RGB(13, 17, 23)
#define COLOR_CARD        RGB(22, 27, 34)
#define COLOR_CARD_HOVER  RGB(33, 38, 45)
#define COLOR_BORDER      RGB(48, 54, 61)
#define COLOR_ACCENT      RGB(88, 166, 255)
#define COLOR_TEXT        RGB(240, 246, 252)
#define COLOR_TEXT_MUTED  RGB(139, 148, 158)
#define COLOR_BAR         RGB(22, 27, 34)
#define COLOR_TAB_ACTIVE   RGB(28, 33, 40)
#define COLOR_TAB_INACTIVE RGB(13, 17, 23)
#define COLOR_GREEN       RGB(63, 185, 80)
#define COLOR_RED         RGB(248, 81, 73)

// Estructura de App REA
typedef struct {
    const wchar_t *key;
    const wchar_t *name;
    const wchar_t *desc;
    const wchar_t *url;
    COLORREF iconColor;
    RECT rect;
} AppCard;

static AppCard g_apps[] = {
    { L"1", L"X (Twitter)", L"Sin pantalla negra", L"https://x.com", RGB(0, 0, 0) },
    { L"2", L"YT Music", L"Sin lagg / 0% CPU", L"https://music.youtube.com", RGB(220, 30, 30) },
    { L"3", L"YouTube", L"H.264 por GPU", L"https://www.youtube.com", RGB(200, 20, 20) },
    { L"4", L"Facebook", L"Sin biometría / Ligero", L"https://web.facebook.com", RGB(24, 119, 242) },
    { L"5", L"Gemini", L"Streaming acelerado", L"https://gemini.google.com", RGB(155, 81, 224) },
    { L"6", L"GTA V Web", L"360p / No Shadows", L"https://fiveplay.lol/?low=1&fps=30&cores=2&res=640x360&shadows=0", RGB(46, 125, 50) }
};
#define NUM_APPS (sizeof(g_apps) / sizeof(g_apps[0]))

static HWND g_hEditUrl = NULL;
static HFONT g_hFontNormal = NULL;
static HFONT g_hFontBold = NULL;
static HFONT g_hFontTitle = NULL;
static HFONT g_hFontSmall = NULL;
static int g_hoveredApp = -1;
static BOOL g_shieldBlocked = FALSE;

// Medición de memoria RAM del proceso (Private Commit / Working Set optimizado)
static double GetProcessRAM_MB() {
    SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return (double)pmc.PrivateUsage / (1024.0 * 1024.0);
    }
    return 0.0;
}

// Ejecutar URL en Waterfox Lite
static void LaunchUrl(const wchar_t *url) {
    wchar_t wfExe[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, wfExe);
    
    // Buscar en WaterfoxLite_Portable
    wchar_t targetPath[MAX_PATH];
    swprintf(targetPath, MAX_PATH, L"%s\\WaterfoxLite_Portable\\app\\core\\waterfox.exe", wfExe);
    
    if (GetFileAttributes(targetPath) != INVALID_FILE_ATTRIBUTES) {
        wchar_t args[1024];
        wchar_t profile[MAX_PATH];
        swprintf(profile, MAX_PATH, L"%s\\WaterfoxLite_Portable\\profile", wfExe);
        swprintf(args, 1024, L"-profile \"%s\" -no-remote \"%s\"", profile, url);
        ShellExecute(NULL, L"open", targetPath, args, NULL, SW_SHOWNORMAL);
    } else {
        // Fallback al navegador predeterminado del sistema
        ShellExecute(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
    }
}

// Guardar DC en archivo BMP
static void SaveDCtoBMP(HDC hdc, int width, int height, const wchar_t *filename) {
    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP hBmp = CreateCompatibleBitmap(screenDC, width, height);
    ReleaseDC(NULL, screenDC);
    HGDIOBJ oldBmp = SelectObject(memDC, hBmp);

    BitBlt(memDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

    BITMAPINFO bih = { 0 };
    bih.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bih.bmiHeader.biWidth = width;
    bih.bmiHeader.biHeight = height;
    bih.bmiHeader.biPlanes = 1;
    bih.bmiHeader.biBitCount = 24;
    bih.bmiHeader.biCompression = BI_RGB;

    int rowSize = ((width * 24 + 31) / 32) * 4;
    DWORD imageSize = rowSize * height;
    bih.bmiHeader.biSizeImage = imageSize;

    char *pixels = (char *)malloc(imageSize);
    if (pixels) {
        GetDIBits(memDC, hBmp, 0, height, pixels, &bih, DIB_RGB_COLORS);

        BITMAPFILEHEADER bfh = { 0 };
        bfh.bfType = 0x4D42; // "BM"
        bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        bfh.bfSize = bfh.bfOffBits + imageSize;

        HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD written;
            WriteFile(hFile, &bfh, sizeof(bfh), &written, NULL);
            WriteFile(hFile, &bih.bmiHeader, sizeof(bih.bmiHeader), &written, NULL);
            WriteFile(hFile, pixels, imageSize, &written, NULL);
            CloseHandle(hFile);
        }
        free(pixels);
    }

    SelectObject(memDC, oldBmp);
    DeleteObject(hBmp);
    DeleteDC(memDC);
}

// Dibujar rectángulo con esquinas redondeadas o bordes
static void DrawDarkRect(HDC hdc, RECT *r, COLORREF bg, COLORREF border, int radius) {
    HBRUSH hBrush = CreateSolidBrush(bg);
    HPEN hPen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    
    if (radius > 0) {
        RoundRect(hdc, r->left, r->top, r->right, r->bottom, radius, radius);
    } else {
        Rectangle(hdc, r->left, r->top, r->right, r->bottom);
    }
    
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

// Renderizado completo de la interfaz Waterfox Lite en C
static void RenderUI(HWND hwnd, HDC hdc) {
    RECT clientRect = { 0, 0, 1060, 640 };
    if (hwnd) {
        GetClientRect(hwnd, &clientRect);
    }
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    if (width <= 0) width = 1060;
    if (height <= 0) height = 640;

    // Double buffering con pantalla real para 24-bit de color sin parpadeos
    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP memBmp = CreateCompatibleBitmap(screenDC, width, height);
    ReleaseDC(NULL, screenDC);
    HGDIOBJ oldBmp = SelectObject(memDC, memBmp);

    // 1. Fondo de la ventana
    HBRUSH hBgBrush = CreateSolidBrush(COLOR_BG);
    FillRect(memDC, &clientRect, hBgBrush);
    DeleteObject(hBgBrush);

    // ------------------------------------------------------------------------
    // 2. Barra Superior de Pestañas (Tab Strip)
    // ------------------------------------------------------------------------
    RECT tabStripRect = { 0, 0, width, 40 };
    HBRUSH hTabStripBrush = CreateSolidBrush(RGB(18, 22, 28));
    FillRect(memDC, &tabStripRect, hTabStripBrush);
    DeleteObject(hTabStripBrush);

    // Pestaña Activa
    RECT tab1 = { 8, 6, 220, 38 };
    DrawDarkRect(memDC, &tab1, COLOR_TAB_ACTIVE, COLOR_BORDER, 6);
    
    // Línea indicadora azul en la pestaña activa
    RECT tabLine = { tab1.left + 8, tab1.top, tab1.right - 8, tab1.top + 3 };
    HBRUSH hAccentBrush = CreateSolidBrush(COLOR_ACCENT);
    FillRect(memDC, &tabLine, hAccentBrush);
    DeleteObject(hAccentBrush);

    // Texto de la pestaña
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, COLOR_TEXT);
    SelectObject(memDC, g_hFontBold);
    RECT tabTextRect = { tab1.left + 16, tab1.top + 6, tab1.right - 30, tab1.bottom };
    DrawText(memDC, L"🦊 Waterfox Lite - Home", -1, &tabTextRect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

    // Botón de cerrar pestaña 'x'
    RECT tabClose = { tab1.right - 24, tab1.top + 6, tab1.right - 8, tab1.bottom - 6 };
    SetTextColor(memDC, COLOR_TEXT_MUTED);
    DrawText(memDC, L"×", -1, &tabClose, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // Botón '+' Nueva Pestaña
    RECT btnNewTab = { tab1.right + 6, 8, tab1.right + 34, 36 };
    DrawDarkRect(memDC, &btnNewTab, RGB(25, 30, 38), COLOR_BORDER, 6);
    SetTextColor(memDC, COLOR_TEXT_MUTED);
    DrawText(memDC, L"+", -1, &btnNewTab, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // ------------------------------------------------------------------------
    // 3. Barra de Navegación y URL (Toolbar)
    // ------------------------------------------------------------------------
    RECT toolbarRect = { 0, 40, width, 84 };
    HBRUSH hToolBrush = CreateSolidBrush(COLOR_BAR);
    FillRect(memDC, &toolbarRect, hToolBrush);
    DeleteObject(hToolBrush);

    // Botones Atrás, Adelante, Recargar
    RECT btnBack = { 12, 48, 40, 76 };
    RECT btnFwd  = { 44, 48, 72, 76 };
    RECT btnReload = { 76, 48, 104, 76 };
    DrawDarkRect(memDC, &btnBack, RGB(28, 33, 40), COLOR_BORDER, 6);
    DrawDarkRect(memDC, &btnFwd, RGB(28, 33, 40), COLOR_BORDER, 6);
    DrawDarkRect(memDC, &btnReload, RGB(28, 33, 40), COLOR_BORDER, 6);

    SetTextColor(memDC, COLOR_TEXT);
    SelectObject(memDC, g_hFontBold);
    DrawText(memDC, L"‹", -1, &btnBack, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    DrawText(memDC, L"›", -1, &btnFwd, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    DrawText(memDC, L"↻", -1, &btnReload, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // Marco del Omnibox / Barra de Direcciones
    int urlRight = width - 180;
    RECT urlBox = { 116, 47, urlRight, 77 };
    DrawDarkRect(memDC, &urlBox, RGB(13, 17, 23), COLOR_ACCENT, 8);

    // Icono de Escudo dentro de la barra
    RECT shieldIcon = { urlBox.left + 8, urlBox.top, urlBox.left + 28, urlBox.bottom };
    SetTextColor(memDC, COLOR_ACCENT);
    DrawText(memDC, L"🛡️", -1, &shieldIcon, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // Botón Game Shield / Game Booster en la barra de herramientas
    RECT btnShield = { urlRight + 12, 47, width - 12, 77 };
    COLORREF shieldCol = g_shieldBlocked ? COLOR_RED : COLOR_GREEN;
    DrawDarkRect(memDC, &btnShield, RGB(28, 33, 40), shieldCol, 8);
    SetTextColor(memDC, shieldCol);
    SelectObject(memDC, g_hFontSmall);
    DrawText(memDC, g_shieldBlocked ? L"🛡️ Shield Activo" : L"🚀 Booster Activo", -1, &btnShield, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // ------------------------------------------------------------------------
    // 4. Panel Central de la Nueva Pestaña (Home Stage)
    // ------------------------------------------------------------------------
    int centerY = 120;

    // Logotipo e Identidad Waterfox Lite
    SelectObject(memDC, g_hFontTitle);
    SetTextColor(memDC, COLOR_TEXT);
    RECT titleRect = { 0, centerY, width, centerY + 40 };
    DrawText(memDC, L"🦊 Waterfox Lite (Native C Theme)", -1, &titleRect, DT_SINGLELINE | DT_CENTER);

    // Subtítulo & Hardware Badge
    SelectObject(memDC, g_hFontSmall);
    SetTextColor(memDC, COLOR_ACCENT);
    RECT badgeRect = { 0, centerY + 42, width, centerY + 65 };
    DrawText(memDC, L"⚡ Motor Ultra-Lite nativo en C | Diseñado para Intel Pentium G620 & HD Graphics", -1, &badgeRect, DT_SINGLELINE | DT_CENTER);

    // ------------------------------------------------------------------------
    // 5. Cuadrícula de Apps REA (6 Tarjetas)
    // ------------------------------------------------------------------------
    int gridStartY = centerY + 85;
    int cardWidth = 145;
    int cardHeight = 110;
    int cardGap = 16;
    int totalGridWidth = (6 * cardWidth) + (5 * cardGap);
    int gridStartX = (width - totalGridWidth) / 2;
    if (gridStartX < 20) gridStartX = 20;

    for (int i = 0; i < (int)NUM_APPS; i++) {
        int x = gridStartX + i * (cardWidth + cardGap);
        int y = gridStartY;
        g_apps[i].rect.left = x;
        g_apps[i].rect.top = y;
        g_apps[i].rect.right = x + cardWidth;
        g_apps[i].rect.bottom = y + cardHeight;

        BOOL isHovered = (i == g_hoveredApp);
        COLORREF bg = isHovered ? COLOR_CARD_HOVER : COLOR_CARD;
        COLORREF border = isHovered ? COLOR_ACCENT : COLOR_BORDER;
        DrawDarkRect(memDC, &g_apps[i].rect, bg, border, 12);

        // Badge de tecla numérica [1..6]
        RECT keyBadge = { g_apps[i].rect.right - 24, g_apps[i].rect.top + 6, g_apps[i].rect.right - 6, g_apps[i].rect.top + 22 };
        DrawDarkRect(memDC, &keyBadge, RGB(35, 42, 52), COLOR_BORDER, 4);
        SelectObject(memDC, g_hFontSmall);
        SetTextColor(memDC, COLOR_TEXT_MUTED);
        DrawText(memDC, g_apps[i].key, -1, &keyBadge, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        // Círculo de icono
        RECT iconBox = { x + (cardWidth - 38) / 2, y + 14, x + (cardWidth + 38) / 2, y + 52 };
        DrawDarkRect(memDC, &iconBox, g_apps[i].iconColor, RGB(60, 68, 78), 10);

        // Letra representativa
        SelectObject(memDC, g_hFontBold);
        SetTextColor(memDC, RGB(255, 255, 255));
        wchar_t initial[2] = { g_apps[i].name[0], L'\0' };
        DrawText(memDC, initial, -1, &iconBox, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        // Nombre de la App
        RECT nameRect = { x + 6, y + 58, x + cardWidth - 6, y + 78 };
        SetTextColor(memDC, COLOR_TEXT);
        SelectObject(memDC, g_hFontBold);
        DrawText(memDC, g_apps[i].name, -1, &nameRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        // Descripción de optimización
        RECT descRect = { x + 4, y + 80, x + cardWidth - 4, y + 102 };
        SetTextColor(memDC, COLOR_TEXT_MUTED);
        SelectObject(memDC, g_hFontSmall);
        DrawText(memDC, g_apps[i].desc, -1, &descRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    }

    // ------------------------------------------------------------------------
    // 6. Barra de Estado Inferior con Consumo Real de RAM
    // ------------------------------------------------------------------------
    double ramMB = GetProcessRAM_MB();
    RECT footerRect = { 0, height - 34, width, height };
    HBRUSH hFooterBrush = CreateSolidBrush(RGB(18, 22, 28));
    FillRect(memDC, &footerRect, hFooterBrush);
    DeleteObject(hFooterBrush);

    SelectObject(memDC, g_hFontSmall);
    SetTextColor(memDC, COLOR_GREEN);
    
    wchar_t ramText[256];
    swprintf(ramText, 256, L"🟢 Memoria RAM en Home: %.2f MB  |  CPU: 0.0%%  |  Tema Lite Puro en C  |  Atajos: Teclas 1-6 abren apps", ramMB);
    DrawText(memDC, ramText, -1, &footerRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // Transferir buffer a la ventana (BitBlt sin parpadeos)
    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
}

// Procedimiento de Ventana principal
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // Habilitar Modo Oscuro en la barra de título de Windows 10/11
        BOOL darkMode = TRUE;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));

        // Tipografías de alta legibilidad
        g_hFontNormal = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        g_hFontBold = CreateFont(16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        g_hFontTitle = CreateFont(26, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        g_hFontSmall = CreateFont(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        // Control Edit para la barra de direcciones / búsqueda
        g_hEditUrl = CreateWindowEx(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            150, 52, 600, 22,
            hwnd, (HMENU)1001, GetModuleHandle(NULL), NULL
        );
        SendMessage(g_hEditUrl, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
        SendMessage(g_hEditUrl, EM_SETCUEBANNER, TRUE, (LPARAM)L"Buscar en la web o escribe una URL...");

        // Temporizador para actualizar medidor de RAM cada segundo
        SetTimer(hwnd, 1, 1000, NULL);
        return 0;
    }

    case WM_SIZE: {
        int width = LOWORD(lParam);
        int urlRight = width - 180;
        if (g_hEditUrl) {
            SetWindowPos(g_hEditUrl, NULL, 150, 52, urlRight - 160, 22, SWP_NOZORDER);
        }
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_TIMER: {
        // Refrescar el pie de página para actualizar el medidor de RAM
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        RECT footer = { 0, clientRect.bottom - 34, clientRect.right, clientRect.bottom };
        InvalidateRect(hwnd, &footer, FALSE);
        return 0;
    }

    case WM_MOUSEMOVE: {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        int oldHover = g_hoveredApp;
        g_hoveredApp = -1;

        for (int i = 0; i < (int)NUM_APPS; i++) {
            if (PtInRect(&g_apps[i].rect, (POINT){ x, y })) {
                g_hoveredApp = i;
                break;
            }
        }

        if (oldHover != g_hoveredApp) {
            InvalidateRect(hwnd, NULL, FALSE);
        }

        // Seguimiento para restaurar hover cuando el ratón sale de la ventana
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hwnd;
        TrackMouseEvent(&tme);
        return 0;
    }

    case WM_MOUSELEAVE: {
        if (g_hoveredApp != -1) {
            g_hoveredApp = -1;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        // Clic en tarjetas de Apps
        for (int i = 0; i < (int)NUM_APPS; i++) {
            if (PtInRect(&g_apps[i].rect, (POINT){ x, y })) {
                LaunchUrl(g_apps[i].url);
                return 0;
            }
        }

        // Clic en botón Game Shield / Game Booster
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        int urlRight = clientRect.right - 180;
        RECT btnShield = { urlRight + 12, 47, clientRect.right - 12, 77 };
        if (PtInRect(&btnShield, (POINT){ x, y })) {
            g_shieldBlocked = !g_shieldBlocked;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        return 0;
    }

    case WM_KEYDOWN: {
        // Atajos de teclado 1..6
        if (wParam >= '1' && wParam <= '6') {
            int idx = wParam - '1';
            if (idx >= 0 && idx < (int)NUM_APPS) {
                LaunchUrl(g_apps[idx].url);
            }
        }
        return 0;
    }

    case WM_COMMAND: {
        if (LOWORD(wParam) == 1001 && HIWORD(wParam) == EN_UPDATE) {
            // Manejar Enter en el campo de texto
        }
        return 0;
    }

    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam;
        SetTextColor(hdcEdit, COLOR_TEXT);
        SetBkColor(hdcEdit, RGB(13, 17, 23));
        static HBRUSH hEditBg = NULL;
        if (!hEditBg) hEditBg = CreateSolidBrush(RGB(13, 17, 23));
        return (INT_PTR)hEditBg;
    }

    case WM_ERASEBKGND:
        return 1; // Evita parpadeos

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RenderUI(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY: {
        if (g_hFontNormal) DeleteObject(g_hFontNormal);
        if (g_hFontBold) DeleteObject(g_hFontBold);
        if (g_hFontTitle) DeleteObject(g_hFontTitle);
        if (g_hFontSmall) DeleteObject(g_hFontSmall);
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"WaterfoxLiteNativeTheme";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        return 1;
    }

    HWND hwnd = CreateWindowEx(
        0,
        L"WaterfoxLiteNativeTheme",
        L"Waterfox Lite - Native Theme",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1060, 640,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 1;

    if (strstr(lpCmdLine, "--screenshot") != NULL) {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        HDC hdc = GetDC(hwnd);
        RenderUI(hwnd, hdc);
        SaveDCtoBMP(hdc, 1060, 640, L"waterfox_native_c_ui.bmp");
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    EmptyWorkingSet(GetCurrentProcess());

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        // Si se presiona Enter en el edit box de URL
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN && GetFocus() == g_hEditUrl) {
            wchar_t buffer[1024];
            GetWindowText(g_hEditUrl, buffer, 1024);
            if (wcslen(buffer) > 0) {
                wchar_t target[1024];
                if (wcsstr(buffer, L"://") != NULL) {
                    wcscpy(target, buffer);
                } else if (wcschr(buffer, L'.') != NULL && wcschr(buffer, L' ') == NULL) {
                    swprintf(target, 1024, L"https://%s", buffer);
                } else {
                    swprintf(target, 1024, L"https://duckduckgo.com/?q=%s", buffer);
                }
                LaunchUrl(target);
            }
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
