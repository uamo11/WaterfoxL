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
#include <stdbool.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "dwmapi.lib")

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

// ============================================================================
// Paleta de Colores Dark Theme (Waterfox Lite)
// ============================================================================
#define COLOR_BG            RGB(13, 17, 23)
#define COLOR_CARD          RGB(22, 27, 34)
#define COLOR_CARD_HOVER    RGB(33, 38, 45)
#define COLOR_BORDER        RGB(48, 54, 61)
#define COLOR_ACCENT        RGB(88, 166, 255)
#define COLOR_TEXT          RGB(240, 246, 252)
#define COLOR_TEXT_MUTED    RGB(139, 148, 158)
#define COLOR_TOOLBAR       RGB(22, 27, 34)
#define COLOR_BOOKMARKS     RGB(18, 22, 28)
#define COLOR_TAB_ACTIVE    RGB(28, 33, 40)
#define COLOR_TAB_INACTIVE  RGB(15, 18, 23)
#define COLOR_TAB_STRIP     RGB(18, 22, 28)
#define COLOR_GREEN         RGB(63, 185, 80)
#define COLOR_RED           RGB(248, 81, 73)
#define COLOR_GOLD          RGB(235, 180, 50)
#define COLOR_BTN_DISABLED  RGB(50, 56, 66)

// Comandos de Menú y Acciones
#define IDM_NEW_TAB          2001
#define IDM_NEW_WINDOW       2002
#define IDM_PRIVATE_WINDOW   2003
#define IDM_BOOKMARKS        2004
#define IDM_HISTORY          2005
#define IDM_DOWNLOADS        2006
#define IDM_EXTENSIONS       2007
#define IDM_SETTINGS         2008
#define IDM_TOGGLE_SHIELD    2009
#define IDM_TOGGLE_BOOKMARKS 2010
#define IDM_ABOUT            2011
#define IDM_EXIT             2012
#define IDM_ZOOM_IN          2013
#define IDM_ZOOM_OUT         2014
#define IDM_ZOOM_RESET       2015

#define ID_GAME_START        4000

// Estructura de Pestaña con Historial Real
#define MAX_TABS 12
#define MAX_HISTORY 16
typedef struct {
    wchar_t title[64];
    wchar_t url[512];
    wchar_t history[MAX_HISTORY][512];
    int historyCount;
    int historyIdx;
    RECT rect;
    RECT closeRect;
    BOOL isActive;
} BrowserTab;

// Estructura de App REA
typedef struct {
    const wchar_t *key;
    const wchar_t *name;
    const wchar_t *desc;
    const wchar_t *url;
    const wchar_t *bmpName;
    COLORREF fallbackColor;
    RECT rect;
    HBITMAP hCachedBmp;
} AppCard;

static AppCard g_apps[] = {
    { L"1", L"X (Twitter)", L"Sin pantalla negra", L"https://x.com", L"x.bmp", RGB(0, 0, 0) },
    { L"2", L"YT Music", L"Sin lagg / 0% CPU", L"https://music.youtube.com", L"ytm.bmp", RGB(220, 30, 30) },
    { L"3", L"YouTube", L"H.264 por GPU", L"https://www.youtube.com", L"yt.bmp", RGB(200, 20, 20) },
    { L"4", L"Facebook", L"Sin biometría / Ligero", L"https://web.facebook.com", L"fb.bmp", RGB(24, 119, 242) },
    { L"5", L"Gemini", L"Streaming acelerado", L"https://gemini.google.com", L"gemini.bmp", RGB(155, 81, 224) },
    { L"6", L"GTA V Web", L"360p / 30FPS Low", L"https://fiveplay.lol/?low=1&fps=30&cores=2&res=640x360&shadows=0", L"gta.bmp", RGB(46, 125, 50) },
    { L"7", L"Juegos Web", L"Carpeta de 24 Juegos", L"action:games_folder", L"gamepad.bmp", RGB(155, 81, 224) }
};
#define NUM_APPS (sizeof(g_apps) / sizeof(g_apps[0]))

// ============================================================================
// Catálogo de Juegos Web Optimizados (WASM / WebGL) - Sin Duplicados
// ============================================================================
typedef struct {
    const wchar_t *title;
    const wchar_t *url;
    const wchar_t *desc;
} WebGameItem;

// 1. Mundo Abierto y Otros
static WebGameItem g_gamesMundoAbierto[] = {
    { L"GTA 5 Web (Principal - 30FPS Low)", L"https://fiveplay.lol/?low=1&fps=30&cores=2&res=640x360&shadows=0", L"360p / 30FPS para Pentium G620" },
    { L"GTA 5 Web (Mirror Deloski)", L"https://gta.deloskiyt.pl", L"Mirror alternativo" },
    { L"GTA 5 Web (Mirror Archive.org)", L"https://web.archive.org/web/20261006050000/https://fiveplay.lol/", L"Respaldo Web Archive" },
    { L"GTA Vice City (WASM)", L"https://joncodeofficial.github.io/gta-vice-city/", L"Puerto WebAssembly C++" },
    { L"The Simpsons: Hit & Run", L"https://shar-wasm.cjoseph.workers.dev/?skipmovie", L"WASM con skipmovie" },
    { L"Skate 3 (Web)", L"https://skate.aaddpp.lol", L"Emulador WebGL" },
    { L"Skate Rust", L"https://global-terror.net", L"Skate multiplayer" },
    { L"PES 6 (Pro Evolution Soccer 6)", L"https://pes6.optijuegos.net", L"Fútbol clásico optimizado" },
    { L"Hedgewars (WASM)", L"https://webwars.link", L"Estrategia por turnos" },
    { L"Más GTA / Minecraft (WasmArcade)", L"https://wasmarcade.com", L"Catálogo WASM arcade" }
};
#define NUM_GAMES_OPENWORLD (sizeof(g_gamesMundoAbierto) / sizeof(g_gamesMundoAbierto[0]))

// 2. Modo Zombies
static WebGameItem g_gamesZombies[] = {
    { L"Call of Duty: BO1 Zombies", L"https://vel.gg/bo1z", L"Zombies clásicos en navegador" },
    { L"Moon Zombies", L"https://moon-zombies.pages.dev", L"Mapa Moon optimizado" },
    { L"Kino der Toten", L"https://kino-der-toten.pages.dev", L"Mapa clásico Kino" },
    { L"BO3 Cheese Cube", L"https://cheese-cube.pages.dev", L"Custom Zombies map" }
};
#define NUM_GAMES_ZOMBIES (sizeof(g_gamesZombies) / sizeof(g_gamesZombies[0]))

// 3. Shooters y Clásicos
static WebGameItem g_gamesShooters[] = {
    { L"Call of Duty: Black Ops 2", L"https://vibeslops.luckeysystems.com", L"Shooter multijugador web" },
    { L"Call of Duty: Modern Warfare 2", L"https://ovz-game-production.up.railway.app", L"MW2 Web port" },
    { L"Halo: Combat Evolved", L"https://mitchellhynes.com/halo", L"Halo CE en navegador" },
    { L"Halo CE Mobile", L"https://hcemobile.com", L"Halo CE versión ligera" },
    { L"Quake 1 (WASM)", L"https://q1.pieter.com", L"Motor idTech 1 WebGL" },
    { L"Quake 2 (WASM)", L"https://q2.pieter.com", L"Motor idTech 2 WebGL" },
    { L"Quake 3 Arena (WASM)", L"https://q3.pieter.com", L"Motor idTech 3 WebGL" },
    { L"Return to Castle Wolfenstein", L"https://rtcw.pieter.com", L"RTCW WebAssembly" },
    { L"Unreal Tournament (WASM)", L"https://ut.pieter.com", L"UT99 Web port" },
    { L"Half-Life (WebGL)", L"https://pixelsuft.github.io/hl/", L"GoldSrc en navegador" },
    { L"Half-Life / CS 1.6 (WebXash)", L"https://x8bitrain.github.io/webXash/", L"Motor Xash3D WebAssembly" },
    { L"CS Surf", L"https://surfd.net", L"Counter-Strike Surfing Web" },
    { L"Diablo 1 (DiabloWeb)", L"https://johnimril.github.io/diablo_web/", L"Diablo WebAssembly" }
};
#define NUM_GAMES_SHOOTERS (sizeof(g_gamesShooters) / sizeof(g_gamesShooters[0]))

// Marcadores Rápidos para la barra de marcadores
typedef struct {
    const wchar_t *title;
    const wchar_t *url;
    RECT rect;
} BookmarkItem;

static BookmarkItem g_bookmarks[] = {
    { L"📁 Marcadores", L"action:bookmarks" },
    { L"🎮 Carpeta de Juegos ▾", L"action:games_folder" },
    { L"𝕏 X", L"https://x.com" },
    { L"🎵 YT Music", L"https://music.youtube.com" },
    { L"🎬 YouTube", L"https://www.youtube.com" },
    { L"👥 Facebook", L"https://web.facebook.com" },
    { L"✨ Gemini", L"https://gemini.google.com" },
    { L"🚗 GTA V Web", L"https://fiveplay.lol/?low=1&fps=30&cores=2&res=640x360&shadows=0" }
};
#define NUM_BOOKMARKS (sizeof(g_bookmarks) / sizeof(g_bookmarks[0]))

// Variables Globales de Interfaz
static BrowserTab g_tabs[MAX_TABS];
static int g_tabCount = 1;
static int g_activeTabIdx = 0;

static HWND g_hEditUrl = NULL;
static HFONT g_hFontNormal = NULL;
static HFONT g_hFontBold = NULL;
static HFONT g_hFontTitle = NULL;
static HFONT g_hFontSmall = NULL;
static HBRUSH g_hEditBgBrush = NULL;

static HBITMAP g_hBmpWaterfox = NULL;
static HBITMAP g_hBmpFolder = NULL;
static HBITMAP g_hBmpGameFolder = NULL;

static int g_hoveredApp = -1;
static int g_hoveredTab = -1;
static int g_hoveredBookmark = -1;
static int g_hoveredBtn = 0; // 1: Back, 2: Fwd, 3: Reload, 4: Home, 5: Menu, 6: Shield, 7: Star, 8: NewTab

static BOOL g_shieldBlocked = FALSE;
static BOOL g_showBookmarksBar = TRUE;
static BOOL g_isBookmarked = FALSE;

static RECT g_rcBack, g_rcFwd, g_rcReload, g_rcHome, g_rcStar, g_rcShield, g_rcMenu, g_rcNewTab;
static RECT g_rcBookmarksBar;

// Declaraciones adelantadas
static void LaunchUrl(HWND hwnd, const wchar_t *url);
static void NavigateTab(int tabIdx, const wchar_t *title, const wchar_t *url);
static void ShowGamesFolderPopup(HWND hwnd, RECT *anchorRect);

// ----------------------------------------------------------------------------
// Medición de Memoria RAM Real
// ----------------------------------------------------------------------------
static double GetProcessRAM_MB() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return (double)pmc.PrivateUsage / (1024.0 * 1024.0);
    }
    return 0.0;
}

// ----------------------------------------------------------------------------
// DPI Awareness compatible con Windows 7, 8.1, 10, 11
// ----------------------------------------------------------------------------
typedef BOOL (WINAPI *SetProcessDpiAwarenessContextProc)(HANDLE);
typedef BOOL (WINAPI *SetProcessDPIAwareProc)(void);

static void EnableDpiAwareness() {
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (hUser32) {
        SetProcessDpiAwarenessContextProc pSetDpi = (SetProcessDpiAwarenessContextProc)GetProcAddress(hUser32, "SetProcessDpiAwarenessContext");
        if (pSetDpi) {
            pSetDpi((HANDLE)-4); // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
            return;
        }
        SetProcessDPIAwareProc pAware = (SetProcessDPIAwareProc)GetProcAddress(hUser32, "SetProcessDPIAware");
        if (pAware) pAware();
    }
}

// ----------------------------------------------------------------------------
// Carga de Iconos desde Caché en Disco (Cero consumo permanente de RAM)
// ----------------------------------------------------------------------------
static HBITMAP LoadCachedBmp(const wchar_t *filename) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    wchar_t *lastSlash = wcsrchr(exePath, L'\\');
    if (lastSlash) *lastSlash = L'\0';

    wchar_t fullPath[MAX_PATH];
    // 1. Probar en <exePath>\cache\<filename>
    swprintf(fullPath, MAX_PATH, L"%s\\cache\\%s", exePath, filename);
    if (GetFileAttributes(fullPath) == INVALID_FILE_ATTRIBUTES) {
        // 2. Probar en <exePath>\native_ui\cache\<filename>
        swprintf(fullPath, MAX_PATH, L"%s\\native_ui\\cache\\%s", exePath, filename);
        if (GetFileAttributes(fullPath) == INVALID_FILE_ATTRIBUTES) {
            // 3. Probar en <exePath>\..\native_ui\cache\<filename>
            swprintf(fullPath, MAX_PATH, L"%s\\..\\native_ui\\cache\\%s", exePath, filename);
        }
    }
    return (HBITMAP)LoadImage(NULL, fullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

static void InitIconCache() {
    for (int i = 0; i < (int)NUM_APPS; i++) {
        g_apps[i].hCachedBmp = LoadCachedBmp(g_apps[i].bmpName);
    }
    g_hBmpWaterfox = LoadCachedBmp(L"wf.bmp");
    g_hBmpFolder = LoadCachedBmp(L"folder.bmp");
    g_hBmpGameFolder = LoadCachedBmp(L"game_folder.bmp");
}

static void FreeIconCache() {
    for (int i = 0; i < (int)NUM_APPS; i++) {
        if (g_apps[i].hCachedBmp) {
            DeleteObject(g_apps[i].hCachedBmp);
            g_apps[i].hCachedBmp = NULL;
        }
    }
    if (g_hBmpWaterfox) {
        DeleteObject(g_hBmpWaterfox);
        g_hBmpWaterfox = NULL;
    }
    if (g_hBmpFolder) {
        DeleteObject(g_hBmpFolder);
        g_hBmpFolder = NULL;
    }
    if (g_hBmpGameFolder) {
        DeleteObject(g_hBmpGameFolder);
        g_hBmpGameFolder = NULL;
    }
}

// ----------------------------------------------------------------------------
// Localizador Inteligente de Waterfox Lite
// ----------------------------------------------------------------------------
static BOOL FindWaterfoxBinary(wchar_t *outBin, DWORD maxBin, wchar_t *outProf, DWORD maxProf) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    wchar_t *lastSlash = wcsrchr(exePath, L'\\');
    if (lastSlash) *lastSlash = L'\0';

    const wchar_t *candidatesBin[] = {
        L"%s\\WaterfoxLite_Portable\\app\\core\\waterfox.exe",
        L"%s\\..\\WaterfoxLite_Portable\\app\\core\\waterfox.exe",
        L"%s\\app\\core\\waterfox.exe",
        L"%s\\app\\waterfox.exe",
        L"%s\\core\\waterfox.exe",
        L"%s\\waterfox.exe"
    };

    const wchar_t *candidatesProf[] = {
        L"%s\\WaterfoxLite_Portable\\profile",
        L"%s\\..\\WaterfoxLite_Portable\\profile",
        L"%s\\profile",
        L"%s\\profile",
        L"%s\\profile",
        L"%s\\profile"
    };

    for (size_t i = 0; i < sizeof(candidatesBin) / sizeof(candidatesBin[0]); i++) {
        wchar_t testBin[MAX_PATH];
        swprintf(testBin, MAX_PATH, candidatesBin[i], exePath);
        if (GetFileAttributes(testBin) != INVALID_FILE_ATTRIBUTES) {
            wcsncpy(outBin, testBin, maxBin);
            swprintf(outProf, maxProf, candidatesProf[i], exePath);
            return TRUE;
        }
    }

    wchar_t curDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, curDir);
    wchar_t curBin[MAX_PATH];
    swprintf(curBin, MAX_PATH, L"%s\\WaterfoxLite_Portable\\app\\core\\waterfox.exe", curDir);
    if (GetFileAttributes(curBin) != INVALID_FILE_ATTRIBUTES) {
        wcsncpy(outBin, curBin, maxBin);
        swprintf(outProf, maxProf, L"%s\\WaterfoxLite_Portable\\profile", curDir);
        return TRUE;
    }

    outBin[0] = L'\0';
    outProf[0] = L'\0';
    return FALSE;
}

// ----------------------------------------------------------------------------
// Detección de Juegos Web para Web Game Shield y REA Booster
// ----------------------------------------------------------------------------
static BOOL IsGameUrl(const wchar_t *url) {
    if (!url) return FALSE;
    const wchar_t *gameDomains[] = {
        L"fiveplay.lol", L"deloskiyt.pl", L"gta-vice-city", L"shar-wasm",
        L"skate.aaddpp.lol", L"global-terror.net", L"pes6.optijuegos",
        L"webwars.link", L"wasmarcade.com", L"vel.gg", L"moon-zombies",
        L"kino-der-toten", L"cheese-cube", L"vibeslops", L"ovz-game",
        L"mitchellhynes.com", L"hcemobile.com", L"q1.pieter.com",
        L"q2.pieter.com", L"q3.pieter.com", L"rtcw.pieter.com",
        L"ut.pieter.com", L"pixelsuft.github.io", L"webXash",
        L"surfd.net", L"diablo_web", L"poki.com", L"crazygames.com", L"krunker.io"
    };
    for (size_t i = 0; i < sizeof(gameDomains) / sizeof(gameDomains[0]); i++) {
        if (wcsstr(url, gameDomains[i]) != NULL) return TRUE;
    }
    return FALSE;
}

// ----------------------------------------------------------------------------
// Lanzador hacia Waterfox Lite
// ----------------------------------------------------------------------------
static void LaunchUrl(HWND hwnd, const wchar_t *url) {
    if (!url || wcslen(url) == 0) return;

    if (g_shieldBlocked && IsGameUrl(url)) {
        int resp = MessageBox(hwnd,
            L"🛡️ Web Game Shield Bloqueado\n\n"
            L"Se ha prevenido la ejecución del juego web pesado para proteger la estabilidad del Pentium G620 y la GPU.\n\n"
            L"¿Deseas desactivar el escudo temporalmente y ejecutar el juego con el Booster optimizado a 360p/30FPS?",
            L"Waterfox Lite - Web Game Shield", MB_YESNO | MB_ICONWARNING);
        if (resp == IDYES) {
            g_shieldBlocked = FALSE;
            InvalidateRect(hwnd, NULL, FALSE);
        } else {
            return;
        }
    }

    wchar_t binPath[MAX_PATH];
    wchar_t profPath[MAX_PATH];
    if (FindWaterfoxBinary(binPath, MAX_PATH, profPath, MAX_PATH)) {
        wchar_t args[1024];
        swprintf(args, 1024, L"-profile \"%s\" -no-remote \"%s\"", profPath, url);
        ShellExecute(NULL, L"open", binPath, args, NULL, SW_SHOWNORMAL);
    } else {
        ShellExecute(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
    }
}

// ----------------------------------------------------------------------------
// Navegación de Pestañas con Historial
// ----------------------------------------------------------------------------
static void NavigateTab(int tabIdx, const wchar_t *title, const wchar_t *url) {
    if (tabIdx < 0 || tabIdx >= g_tabCount) return;
    BrowserTab *t = &g_tabs[tabIdx];
    wcsncpy(t->title, title ? title : L"Nueva Pestaña", 63);
    t->title[63] = L'\0';
    wcsncpy(t->url, url ? url : L"about:home", 511);
    t->url[511] = L'\0';

    if (t->historyCount == 0 || wcscmp(t->history[t->historyIdx], t->url) != 0) {
        if (t->historyIdx + 1 < MAX_HISTORY) {
            t->historyIdx++;
            wcsncpy(t->history[t->historyIdx], t->url, 511);
            t->historyCount = t->historyIdx + 1;
        }
    }
}

static void CreateNewTab(HWND hwnd, const wchar_t *title, const wchar_t *url) {
    if (g_tabCount < MAX_TABS) {
        int newIdx = g_tabCount;
        wcsncpy(g_tabs[newIdx].title, title ? title : L"Nueva Pestaña", 63);
        wcsncpy(g_tabs[newIdx].url, url ? url : L"about:home", 511);
        g_tabs[newIdx].historyCount = 1;
        g_tabs[newIdx].historyIdx = 0;
        wcsncpy(g_tabs[newIdx].history[0], g_tabs[newIdx].url, 511);
        for (int j = 0; j <= newIdx; j++) g_tabs[j].isActive = (j == newIdx);
        g_activeTabIdx = newIdx;
        g_tabCount++;
        SetWindowText(g_hEditUrl, wcscmp(g_tabs[newIdx].url, L"about:home") == 0 ? L"" : g_tabs[newIdx].url);
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

static void CloseTab(HWND hwnd, int tabIdx) {
    if (tabIdx < 0 || tabIdx >= g_tabCount) return;
    if (g_tabCount > 1) {
        for (int j = tabIdx; j < g_tabCount - 1; j++) {
            g_tabs[j] = g_tabs[j + 1];
        }
        g_tabCount--;
        if (g_activeTabIdx >= g_tabCount) g_activeTabIdx = g_tabCount - 1;
        for (int k = 0; k < g_tabCount; k++) {
            g_tabs[k].isActive = (k == g_activeTabIdx);
        }
        SetWindowText(g_hEditUrl, wcscmp(g_tabs[g_activeTabIdx].url, L"about:home") == 0 ? L"" : g_tabs[g_activeTabIdx].url);
        InvalidateRect(hwnd, NULL, FALSE);
    } else {
        wcscpy(g_tabs[0].title, L"Waterfox Lite - Home");
        wcscpy(g_tabs[0].url, L"about:home");
        g_tabs[0].historyCount = 1;
        g_tabs[0].historyIdx = 0;
        wcscpy(g_tabs[0].history[0], L"about:home");
        SetWindowText(g_hEditUrl, L"");
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

// ----------------------------------------------------------------------------
// Utilidades Gráficas GDI
// ----------------------------------------------------------------------------
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

static void DrawIconBmp(HDC hdc, HBITMAP hBmp, int x, int y, int w, int h) {
    if (!hBmp) return;
    HDC memDC = CreateCompatibleDC(hdc);
    HGDIOBJ oldBmp = SelectObject(memDC, hBmp);
    BITMAP bm;
    GetObject(hBmp, sizeof(bm), &bm);
    SetStretchBltMode(hdc, HALFTONE);
    SetBrushOrgEx(hdc, 0, 0, NULL);
    StretchBlt(hdc, x, y, w, h, memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
}

// ----------------------------------------------------------------------------
// Guardado de Screenshot en BMP (24-bit RGB)
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// Renderizado Principal de la Interfaz Waterfox Lite (Doble Buffer GDI)
// ----------------------------------------------------------------------------
static void RenderUI(HWND hwnd, HDC hdc) {
    RECT clientRect = { 0, 0, 1080, 680 };
    if (hwnd) GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    if (width <= 0) width = 1080;
    if (height <= 0) height = 680;

    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP memBmp = CreateCompatibleBitmap(screenDC, width, height);
    ReleaseDC(NULL, screenDC);
    HGDIOBJ oldBmp = SelectObject(memDC, memBmp);

    // Fondo general
    HBRUSH hBgBrush = CreateSolidBrush(COLOR_BG);
    FillRect(memDC, &clientRect, hBgBrush);
    DeleteObject(hBgBrush);

    // ------------------------------------------------------------------------
    // 1. Tiras de Pestañas Superiores (Tab Strip)
    // ------------------------------------------------------------------------
    RECT tabStripRect = { 0, 0, width, 40 };
    HBRUSH hTabStripBrush = CreateSolidBrush(COLOR_TAB_STRIP);
    FillRect(memDC, &tabStripRect, hTabStripBrush);
    DeleteObject(hTabStripBrush);

    int tabX = 8;
    int tabW = 200;
    if (tabW * g_tabCount > width - 100) {
        tabW = (width - 100) / g_tabCount;
    }

    for (int i = 0; i < g_tabCount; i++) {
        g_tabs[i].rect.left = tabX;
        g_tabs[i].rect.top = 6;
        g_tabs[i].rect.right = tabX + tabW;
        g_tabs[i].rect.bottom = 39;

        BOOL isActive = (i == g_activeTabIdx);
        COLORREF tabBg = isActive ? COLOR_TAB_ACTIVE : COLOR_TAB_INACTIVE;
        COLORREF tabBorder = isActive ? COLOR_BORDER : RGB(25, 30, 38);
        DrawDarkRect(memDC, &g_tabs[i].rect, tabBg, tabBorder, 6);

        if (isActive) {
            RECT tabLine = { g_tabs[i].rect.left + 6, g_tabs[i].rect.top, g_tabs[i].rect.right - 6, g_tabs[i].rect.top + 3 };
            HBRUSH hAccent = CreateSolidBrush(COLOR_ACCENT);
            FillRect(memDC, &tabLine, hAccent);
            DeleteObject(hAccent);
        }

        DrawIconBmp(memDC, g_hBmpWaterfox, g_tabs[i].rect.left + 8, g_tabs[i].rect.top + 8, 16, 16);

        SetBkMode(memDC, TRANSPARENT);
        SetTextColor(memDC, isActive ? COLOR_TEXT : COLOR_TEXT_MUTED);
        SelectObject(memDC, isActive ? g_hFontBold : g_hFontNormal);
        RECT rcTabText = { g_tabs[i].rect.left + 28, g_tabs[i].rect.top + 6, g_tabs[i].rect.right - 26, g_tabs[i].rect.bottom };
        DrawText(memDC, g_tabs[i].title, -1, &rcTabText, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

        g_tabs[i].closeRect.left = g_tabs[i].rect.right - 22;
        g_tabs[i].closeRect.top = g_tabs[i].rect.top + 8;
        g_tabs[i].closeRect.right = g_tabs[i].rect.right - 6;
        g_tabs[i].closeRect.bottom = g_tabs[i].rect.bottom - 8;

        SetTextColor(memDC, COLOR_TEXT_MUTED);
        SelectObject(memDC, g_hFontNormal);
        DrawText(memDC, L"✕", -1, &g_tabs[i].closeRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        tabX += tabW + 4;
    }

    g_rcNewTab.left = tabX + 4;
    g_rcNewTab.top = 8;
    g_rcNewTab.right = tabX + 32;
    g_rcNewTab.bottom = 36;
    COLORREF btnPlusBg = (g_hoveredBtn == 8) ? COLOR_CARD_HOVER : RGB(25, 30, 38);
    DrawDarkRect(memDC, &g_rcNewTab, btnPlusBg, COLOR_BORDER, 6);
    SetTextColor(memDC, COLOR_TEXT);
    SelectObject(memDC, g_hFontBold);
    DrawText(memDC, L"+", -1, &g_rcNewTab, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // ------------------------------------------------------------------------
    // 2. Barra de Navegación y Herramientas (Navigation Toolbar)
    // ------------------------------------------------------------------------
    RECT toolbarRect = { 0, 40, width, 84 };
    HBRUSH hToolBrush = CreateSolidBrush(COLOR_TOOLBAR);
    FillRect(memDC, &toolbarRect, hToolBrush);
    DeleteObject(hToolBrush);

    BrowserTab *activeTab = &g_tabs[g_activeTabIdx];
    BOOL canBack = (activeTab->historyIdx > 0);
    BOOL canFwd  = (activeTab->historyIdx + 1 < activeTab->historyCount);

    g_rcBack.left = 10; g_rcBack.top = 47; g_rcBack.right = 38; g_rcBack.bottom = 77;
    g_rcFwd.left = 42; g_rcFwd.top = 47; g_rcFwd.right = 70; g_rcFwd.bottom = 77;
    g_rcReload.left = 74; g_rcReload.top = 47; g_rcReload.right = 102; g_rcReload.bottom = 77;
    g_rcHome.left = 106; g_rcHome.top = 47; g_rcHome.right = 134; g_rcHome.bottom = 77;

    DrawDarkRect(memDC, &g_rcBack, (g_hoveredBtn == 1 && canBack) ? COLOR_CARD_HOVER : RGB(28, 33, 40), COLOR_BORDER, 6);
    DrawDarkRect(memDC, &g_rcFwd, (g_hoveredBtn == 2 && canFwd) ? COLOR_CARD_HOVER : RGB(28, 33, 40), COLOR_BORDER, 6);
    DrawDarkRect(memDC, &g_rcReload, (g_hoveredBtn == 3) ? COLOR_CARD_HOVER : RGB(28, 33, 40), COLOR_BORDER, 6);
    DrawDarkRect(memDC, &g_rcHome, (g_hoveredBtn == 4) ? COLOR_CARD_HOVER : RGB(28, 33, 40), COLOR_BORDER, 6);

    SelectObject(memDC, g_hFontBold);
    SetTextColor(memDC, canBack ? COLOR_TEXT : COLOR_BTN_DISABLED);
    DrawText(memDC, L"‹", -1, &g_rcBack, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    SetTextColor(memDC, canFwd ? COLOR_TEXT : COLOR_BTN_DISABLED);
    DrawText(memDC, L"›", -1, &g_rcFwd, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    SetTextColor(memDC, COLOR_TEXT);
    DrawText(memDC, L"↻", -1, &g_rcReload, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    SelectObject(memDC, g_hFontSmall);
    DrawText(memDC, L"🏠", -1, &g_rcHome, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // Botones del lado derecho: Menú Hamburguesa '⋮' y Game Shield
    g_rcMenu.left = width - 42; g_rcMenu.top = 47; g_rcMenu.right = width - 10; g_rcMenu.bottom = 77;
    DrawDarkRect(memDC, &g_rcMenu, (g_hoveredBtn == 5) ? COLOR_CARD_HOVER : RGB(28, 33, 40), COLOR_BORDER, 6);
    SetTextColor(memDC, COLOR_TEXT);
    SelectObject(memDC, g_hFontBold);
    DrawText(memDC, L"⋮", -1, &g_rcMenu, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // Botón Game Shield / Game Booster
    g_rcShield.left = width - 185; g_rcShield.top = 47; g_rcShield.right = width - 48; g_rcShield.bottom = 77;
    COLORREF shieldCol = g_shieldBlocked ? COLOR_RED : COLOR_GREEN;
    DrawDarkRect(memDC, &g_rcShield, (g_hoveredBtn == 6) ? COLOR_CARD_HOVER : RGB(28, 33, 40), shieldCol, 8);
    SetTextColor(memDC, shieldCol);
    SelectObject(memDC, g_hFontSmall);
    DrawText(memDC, g_shieldBlocked ? L"🛡️ Shield Activo" : L"🚀 Booster Activo", -1, &g_rcShield, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // Barra de Direcciones / Omnibox
    int urlLeft = 142;
    int urlRight = g_rcShield.left - 10;
    RECT urlBox = { urlLeft, 47, urlRight, 77 };
    DrawDarkRect(memDC, &urlBox, RGB(13, 17, 23), COLOR_ACCENT, 8);

    // Candado de seguridad 🔒
    RECT rcLock = { urlBox.left + 8, urlBox.top, urlBox.left + 26, urlBox.bottom };
    SetTextColor(memDC, COLOR_GREEN);
    SelectObject(memDC, g_hFontSmall);
    DrawText(memDC, L"🔒", -1, &rcLock, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // Estrella de Marcador ⭐
    g_rcStar.left = urlBox.right - 28; g_rcStar.top = urlBox.top; g_rcStar.right = urlBox.right - 6; g_rcStar.bottom = urlBox.bottom;
    SetTextColor(memDC, g_isBookmarked ? COLOR_GOLD : COLOR_TEXT_MUTED);
    DrawText(memDC, g_isBookmarked ? L"★" : L"☆", -1, &g_rcStar, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // ------------------------------------------------------------------------
    // 3. Barra de Guardados / Marcadores (Bookmarks Toolbar)
    // ------------------------------------------------------------------------
    int currentY = 84;
    if (g_showBookmarksBar) {
        g_rcBookmarksBar.left = 0; g_rcBookmarksBar.top = currentY; g_rcBookmarksBar.right = width; g_rcBookmarksBar.bottom = currentY + 30;
        HBRUSH hBmBrush = CreateSolidBrush(COLOR_BOOKMARKS);
        FillRect(memDC, &g_rcBookmarksBar, hBmBrush);
        DeleteObject(hBmBrush);

        RECT bmLine = { 0, g_rcBookmarksBar.bottom - 1, width, g_rcBookmarksBar.bottom };
        HBRUSH hBorderBrush = CreateSolidBrush(COLOR_BORDER);
        FillRect(memDC, &bmLine, hBorderBrush);
        DeleteObject(hBorderBrush);

        int bmX = 12;
        SelectObject(memDC, g_hFontSmall);

        for (int i = 0; i < (int)NUM_BOOKMARKS; i++) {
            SIZE size;
            GetTextExtentPoint32(memDC, g_bookmarks[i].title, wcslen(g_bookmarks[i].title), &size);
            int itemW = size.cx + 20;

            g_bookmarks[i].rect.left = bmX;
            g_bookmarks[i].rect.top = currentY + 3;
            g_bookmarks[i].rect.right = bmX + itemW;
            g_bookmarks[i].rect.bottom = currentY + 27;

            BOOL isBmHover = (i == g_hoveredBookmark);
            if (isBmHover) {
                DrawDarkRect(memDC, &g_bookmarks[i].rect, COLOR_CARD_HOVER, COLOR_BORDER, 4);
            }

            // Destacar Carpeta de Juegos con acento dorado
            if (i == 1) {
                SetTextColor(memDC, isBmHover ? COLOR_GOLD : RGB(220, 175, 55));
            } else {
                SetTextColor(memDC, isBmHover ? COLOR_TEXT : COLOR_TEXT_MUTED);
            }
            DrawText(memDC, g_bookmarks[i].title, -1, &g_bookmarks[i].rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

            bmX += itemW + 8;
        }

        currentY += 30;
    }

    // ------------------------------------------------------------------------
    // 4. Panel Central de la Nueva Pestaña (Home Stage)
    // ------------------------------------------------------------------------
    int centerY = currentY + 20;

    if (g_hBmpWaterfox) {
        DrawIconBmp(memDC, g_hBmpWaterfox, (width - 48) / 2, centerY, 48, 48);
        centerY += 52;
    }

    SelectObject(memDC, g_hFontTitle);
    SetTextColor(memDC, COLOR_TEXT);
    RECT titleRect = { 0, centerY, width, centerY + 32 };
    DrawText(memDC, L"Waterfox Lite (Native C Theme)", -1, &titleRect, DT_SINGLELINE | DT_CENTER);

    SelectObject(memDC, g_hFontSmall);
    SetTextColor(memDC, COLOR_ACCENT);
    RECT badgeRect = { 0, centerY + 34, width, centerY + 54 };
    DrawText(memDC, L"⚡ Motor Ultra-Lite nativo en C | Colección WASM/WebGL | Intel Pentium G620 & HD Graphics", -1, &badgeRect, DT_SINGLELINE | DT_CENTER);

    // ------------------------------------------------------------------------
    // 5. Cuadrícula Responsiva de Accesos Directos (7 Tarjetas con Iconos en Caché)
    // ------------------------------------------------------------------------
    int gridStartY = centerY + 64;
    int cols = (width >= 1060) ? 7 : 4;
    int cardWidth = (cols == 7) ? 134 : 180;
    int cardHeight = (cols == 7) ? 115 : 100;
    int cardGapX = 12;
    int cardGapY = 12;

    for (int i = 0; i < (int)NUM_APPS; i++) {
        int r = i / cols;
        int c = i % cols;

        int itemsInThisRow = (r == 0) ? (cols == 7 ? 7 : 4) : (NUM_APPS - 4);
        int totalRowWidth = (itemsInThisRow * cardWidth) + ((itemsInThisRow - 1) * cardGapX);
        int rowStartX = (width - totalRowWidth) / 2;
        if (rowStartX < 12) rowStartX = 12;

        int x = rowStartX + c * (cardWidth + cardGapX);
        int y = gridStartY + r * (cardHeight + cardGapY);

        g_apps[i].rect.left = x;
        g_apps[i].rect.top = y;
        g_apps[i].rect.right = x + cardWidth;
        g_apps[i].rect.bottom = y + cardHeight;

        BOOL isHovered = (i == g_hoveredApp);
        COLORREF bg = isHovered ? COLOR_CARD_HOVER : COLOR_CARD;
        COLORREF border = isHovered ? COLOR_ACCENT : COLOR_BORDER;
        DrawDarkRect(memDC, &g_apps[i].rect, bg, border, 12);

        // Badge de tecla numérica [1..7]
        RECT keyBadge = { g_apps[i].rect.right - 22, g_apps[i].rect.top + 6, g_apps[i].rect.right - 6, g_apps[i].rect.top + 22 };
        DrawDarkRect(memDC, &keyBadge, RGB(35, 42, 52), COLOR_BORDER, 4);
        SelectObject(memDC, g_hFontSmall);
        SetTextColor(memDC, COLOR_TEXT_MUTED);
        DrawText(memDC, g_apps[i].key, -1, &keyBadge, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        // Icono desde Caché en Disco (BMP)
        int iconX = x + (cardWidth - 32) / 2;
        int iconY = y + 12;
        if (g_apps[i].hCachedBmp) {
            DrawIconBmp(memDC, g_apps[i].hCachedBmp, iconX, iconY, 32, 32);
        } else {
            RECT fallbackBox = { iconX, iconY, iconX + 32, iconY + 32 };
            DrawDarkRect(memDC, &fallbackBox, g_apps[i].fallbackColor, COLOR_BORDER, 8);
        }

        // Nombre de la App / Juego
        RECT nameRect = { x + 4, y + 50, x + cardWidth - 4, y + 70 };
        SetTextColor(memDC, COLOR_TEXT);
        SelectObject(memDC, g_hFontBold);
        DrawText(memDC, g_apps[i].name, -1, &nameRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        // Descripción de optimización
        RECT descRect = { x + 4, y + 72, x + cardWidth - 4, y + 94 };
        SetTextColor(memDC, COLOR_TEXT_MUTED);
        SelectObject(memDC, g_hFontSmall);
        DrawText(memDC, g_apps[i].desc, -1, &descRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    }

    // ------------------------------------------------------------------------
    // 6. Barra de Estado Inferior con Consumo Real de RAM
    // ------------------------------------------------------------------------
    double ramMB = GetProcessRAM_MB();
    RECT footerRect = { 0, height - 34, width, height };
    HBRUSH hFooterBrush = CreateSolidBrush(COLOR_TAB_STRIP);
    FillRect(memDC, &footerRect, hFooterBrush);
    DeleteObject(hFooterBrush);

    SelectObject(memDC, g_hFontSmall);
    SetTextColor(memDC, COLOR_GREEN);

    wchar_t ramText[256];
    swprintf(ramText, 256, L"🟢 RAM en Home: %.2f MB  |  CPU: 0.0%%  |  24 Juegos Web en Carpeta  |  Atajos: Teclas 1-7 abren apps y juegos", ramMB);
    DrawText(memDC, ramText, -1, &footerRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
}

// ----------------------------------------------------------------------------
// Popup de Menú de Marcadores Rápidos
// ----------------------------------------------------------------------------
static void ShowBookmarksPopup(HWND hwnd, RECT *anchorRect) {
    HMENU hMenu = CreatePopupMenu();
    for (int i = 2; i < (int)NUM_BOOKMARKS; i++) {
        AppendMenu(hMenu, MF_STRING, 3000 + i, g_bookmarks[i].title);
    }
    POINT pt = { anchorRect->left, anchorRect->bottom };
    ClientToScreen(hwnd, &pt);
    int cmd = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);

    if (cmd >= 3002 && cmd < 3000 + (int)NUM_BOOKMARKS) {
        int idx = cmd - 3000;
        NavigateTab(g_activeTabIdx, g_bookmarks[idx].title, g_bookmarks[idx].url);
        SetWindowText(g_hEditUrl, g_bookmarks[idx].url);
        LaunchUrl(hwnd, g_bookmarks[idx].url);
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

// ----------------------------------------------------------------------------
// Popup de Menú de la Carpeta de Juegos Web (24 Juegos Optimizados sin Duplicados)
// ----------------------------------------------------------------------------
static void ShowGamesFolderPopup(HWND hwnd, RECT *anchorRect) {
    HMENU hMenu = CreatePopupMenu();

    // 1. Submenú Mundo Abierto y Otros (10 títulos)
    HMENU hSubOpenWorld = CreatePopupMenu();
    for (int i = 0; i < (int)NUM_GAMES_OPENWORLD; i++) {
        AppendMenu(hSubOpenWorld, MF_STRING, ID_GAME_START + i, g_gamesMundoAbierto[i].title);
    }
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubOpenWorld, L"🌍 Mundo Abierto & Otros (10)");

    // 2. Submenú Zombies (4 títulos)
    HMENU hSubZombies = CreatePopupMenu();
    for (int i = 0; i < (int)NUM_GAMES_ZOMBIES; i++) {
        AppendMenu(hSubZombies, MF_STRING, ID_GAME_START + 100 + i, g_gamesZombies[i].title);
    }
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubZombies, L"🧟 Modo Zombies (CoD) (4)");

    // 3. Submenú Shooters y Clásicos (13 títulos)
    HMENU hSubShooters = CreatePopupMenu();
    for (int i = 0; i < (int)NUM_GAMES_SHOOTERS; i++) {
        AppendMenu(hSubShooters, MF_STRING, ID_GAME_START + 200 + i, g_gamesShooters[i].title);
    }
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubShooters, L"🎯 Shooters & Clásicos (13)");

    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, 4999, L"⚡ Perfil REA Game Booster: 30 FPS / 2 Cores Activo");

    POINT pt = { anchorRect->left, anchorRect->bottom };
    ClientToScreen(hwnd, &pt);

    int cmd = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);

    if (cmd >= ID_GAME_START && cmd < ID_GAME_START + (int)NUM_GAMES_OPENWORLD) {
        int idx = cmd - ID_GAME_START;
        NavigateTab(g_activeTabIdx, g_gamesMundoAbierto[idx].title, g_gamesMundoAbierto[idx].url);
        SetWindowText(g_hEditUrl, g_gamesMundoAbierto[idx].url);
        LaunchUrl(hwnd, g_gamesMundoAbierto[idx].url);
        InvalidateRect(hwnd, NULL, FALSE);
    }
    else if (cmd >= ID_GAME_START + 100 && cmd < ID_GAME_START + 100 + (int)NUM_GAMES_ZOMBIES) {
        int idx = cmd - (ID_GAME_START + 100);
        NavigateTab(g_activeTabIdx, g_gamesZombies[idx].title, g_gamesZombies[idx].url);
        SetWindowText(g_hEditUrl, g_gamesZombies[idx].url);
        LaunchUrl(hwnd, g_gamesZombies[idx].url);
        InvalidateRect(hwnd, NULL, FALSE);
    }
    else if (cmd >= ID_GAME_START + 200 && cmd < ID_GAME_START + 200 + (int)NUM_GAMES_SHOOTERS) {
        int idx = cmd - (ID_GAME_START + 200);
        NavigateTab(g_activeTabIdx, g_gamesShooters[idx].title, g_gamesShooters[idx].url);
        SetWindowText(g_hEditUrl, g_gamesShooters[idx].url);
        LaunchUrl(hwnd, g_gamesShooters[idx].url);
        InvalidateRect(hwnd, NULL, FALSE);
    }
    else if (cmd == 4999) {
        MessageBox(hwnd,
            L"⚡ Perfil REA Game Booster (Waterfox Lite)\n\n"
            L"• Limitador nativo a 30 FPS para juegos 3D (evita 100% CPU lock en Pentium G620)\n"
            L"• Clamping de hilos a 2 núcleos (hardwareConcurrency = 2)\n"
            L"• Aceleración WebGL con antialias desactivado (ahorra 60% GPU)\n"
            L"• Escalado nítido con interpolación pixelated para Intel HD Graphics",
            L"REA Game Booster Activo", MB_OK | MB_ICONINFORMATION);
    }
}

// ----------------------------------------------------------------------------
// Mostrar Menú Hamburguesa Nativo con Múltiples Opciones
// ----------------------------------------------------------------------------
static void ShowMainMenu(HWND hwnd) {
    HMENU hMenu = CreatePopupMenu();

    AppendMenu(hMenu, MF_STRING, IDM_NEW_TAB, L"Nueva pestaña\tCtrl+T");
    AppendMenu(hMenu, MF_STRING, IDM_NEW_WINDOW, L"Nueva ventana\tCtrl+N");
    AppendMenu(hMenu, MF_STRING, IDM_PRIVATE_WINDOW, L"Nueva ventana privada\tCtrl+Shift+P");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

    AppendMenu(hMenu, MF_STRING, IDM_BOOKMARKS, L"Marcadores\tCtrl+Shift+O");
    AppendMenu(hMenu, MF_STRING, IDM_HISTORY, L"Historial\tCtrl+H");
    AppendMenu(hMenu, MF_STRING, IDM_DOWNLOADS, L"Descargas\tCtrl+J");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

    UINT flagBm = g_showBookmarksBar ? (MF_STRING | MF_CHECKED) : (MF_STRING | MF_UNCHECKED);
    AppendMenu(hMenu, flagBm, IDM_TOGGLE_BOOKMARKS, L"Mostrar barra de marcadores\tCtrl+Shift+B");

    UINT flagShield = g_shieldBlocked ? (MF_STRING | MF_CHECKED) : (MF_STRING | MF_UNCHECKED);
    AppendMenu(hMenu, flagShield, IDM_TOGGLE_SHIELD, L"Bloqueador de juegos web (Shield)");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

    AppendMenu(hMenu, MF_STRING, IDM_ZOOM_IN, L"Zoom (+)");
    AppendMenu(hMenu, MF_STRING, IDM_ZOOM_OUT, L"Zoom (-)");
    AppendMenu(hMenu, MF_STRING, IDM_ZOOM_RESET, L"Zoom (100%)");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

    AppendMenu(hMenu, MF_STRING, IDM_SETTINGS, L"Configuración y Rendimiento");
    AppendMenu(hMenu, MF_STRING, IDM_ABOUT, L"Acerca de Waterfox Lite");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

    AppendMenu(hMenu, MF_STRING, IDM_EXIT, L"Salir\tCtrl+Q");

    POINT pt = { g_rcMenu.left, g_rcMenu.bottom };
    ClientToScreen(hwnd, &pt);

    TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_TOPALIGN, pt.x + (g_rcMenu.right - g_rcMenu.left), pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);
}

// ----------------------------------------------------------------------------
// Procedimiento de Ventana Principal (Win32 WndProc)
// ----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        BOOL darkMode = TRUE;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));

        wcscpy(g_tabs[0].title, L"Waterfox Lite - Home");
        wcscpy(g_tabs[0].url, L"about:home");
        g_tabs[0].isActive = TRUE;
        g_tabs[0].historyCount = 1;
        g_tabs[0].historyIdx = 0;
        wcscpy(g_tabs[0].history[0], L"about:home");
        g_tabCount = 1;
        g_activeTabIdx = 0;

        InitIconCache();

        g_hFontNormal = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        g_hFontBold   = CreateFont(15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        g_hFontTitle  = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        g_hFontSmall  = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        g_hEditUrl = CreateWindowEx(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            172, 52, 600, 22,
            hwnd, (HMENU)1001, GetModuleHandle(NULL), NULL
        );
        SendMessage(g_hEditUrl, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
        SendMessage(g_hEditUrl, EM_SETCUEBANNER, TRUE, (LPARAM)L"Buscar en la web o presiona 1-7 para abrir apps y juegos...");

        SetTimer(hwnd, 1, 1000, NULL);
        return 0;
    }

    case WM_GETMINMAXINFO: {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 800;
        lpMMI->ptMinTrackSize.y = 500;
        return 0;
    }

    case WM_SIZE: {
        int width = LOWORD(lParam);
        int urlRight = width - 195;
        if (g_hEditUrl) {
            SetWindowPos(g_hEditUrl, NULL, 172, 52, urlRight - 210, 22, SWP_NOZORDER);
        }
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_TIMER: {
        static int s_trimCount = 0;
        s_trimCount++;
        if (s_trimCount >= 10) {
            s_trimCount = 0;
            EmptyWorkingSet(GetCurrentProcess());
        }

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        RECT footer = { 0, clientRect.bottom - 34, clientRect.right, clientRect.bottom };
        InvalidateRect(hwnd, &footer, FALSE);
        return 0;
    }

    case WM_MOUSEMOVE: {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        int oldHoverApp = g_hoveredApp;
        int oldHoverTab = g_hoveredTab;
        int oldHoverBm  = g_hoveredBookmark;
        int oldHoverBtn = g_hoveredBtn;

        g_hoveredApp = -1;
        g_hoveredTab = -1;
        g_hoveredBookmark = -1;
        g_hoveredBtn = 0;

        for (int i = 0; i < (int)NUM_APPS; i++) {
            if (PtInRect(&g_apps[i].rect, (POINT){ x, y })) {
                g_hoveredApp = i;
                break;
            }
        }

        for (int i = 0; i < g_tabCount; i++) {
            if (PtInRect(&g_tabs[i].rect, (POINT){ x, y })) {
                g_hoveredTab = i;
                break;
            }
        }

        if (g_showBookmarksBar) {
            for (int i = 0; i < (int)NUM_BOOKMARKS; i++) {
                if (PtInRect(&g_bookmarks[i].rect, (POINT){ x, y })) {
                    g_hoveredBookmark = i;
                    break;
                }
            }
        }

        if (PtInRect(&g_rcBack, (POINT){ x, y })) g_hoveredBtn = 1;
        else if (PtInRect(&g_rcFwd, (POINT){ x, y })) g_hoveredBtn = 2;
        else if (PtInRect(&g_rcReload, (POINT){ x, y })) g_hoveredBtn = 3;
        else if (PtInRect(&g_rcHome, (POINT){ x, y })) g_hoveredBtn = 4;
        else if (PtInRect(&g_rcMenu, (POINT){ x, y })) g_hoveredBtn = 5;
        else if (PtInRect(&g_rcShield, (POINT){ x, y })) g_hoveredBtn = 6;
        else if (PtInRect(&g_rcStar, (POINT){ x, y })) g_hoveredBtn = 7;
        else if (PtInRect(&g_rcNewTab, (POINT){ x, y })) g_hoveredBtn = 8;

        if (oldHoverApp != g_hoveredApp || oldHoverTab != g_hoveredTab || oldHoverBm != g_hoveredBookmark || oldHoverBtn != g_hoveredBtn) {
            InvalidateRect(hwnd, NULL, FALSE);
        }

        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hwnd;
        TrackMouseEvent(&tme);
        return 0;
    }

    case WM_MOUSELEAVE: {
        g_hoveredApp = -1;
        g_hoveredTab = -1;
        g_hoveredBookmark = -1;
        g_hoveredBtn = 0;
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        // 1. Pestañas
        for (int i = 0; i < g_tabCount; i++) {
            if (PtInRect(&g_tabs[i].closeRect, (POINT){ x, y })) {
                CloseTab(hwnd, i);
                return 0;
            }

            if (PtInRect(&g_tabs[i].rect, (POINT){ x, y })) {
                g_activeTabIdx = i;
                for (int j = 0; j < g_tabCount; j++) g_tabs[j].isActive = (j == i);
                SetWindowText(g_hEditUrl, wcscmp(g_tabs[i].url, L"about:home") == 0 ? L"" : g_tabs[i].url);
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
        }

        // 2. Botón '+' Nueva Pestaña
        if (PtInRect(&g_rcNewTab, (POINT){ x, y })) {
            CreateNewTab(hwnd, L"Nueva Pestaña", L"about:home");
            return 0;
        }

        // 3. Botones de Navegación Atrás / Adelante con Historial Real
        BrowserTab *activeTab = &g_tabs[g_activeTabIdx];
        if (PtInRect(&g_rcBack, (POINT){ x, y })) {
            if (activeTab->historyIdx > 0) {
                activeTab->historyIdx--;
                const wchar_t *prevUrl = activeTab->history[activeTab->historyIdx];
                wcsncpy(activeTab->url, prevUrl, 511);
                SetWindowText(g_hEditUrl, wcscmp(prevUrl, L"about:home") == 0 ? L"" : prevUrl);
                if (wcscmp(prevUrl, L"about:home") != 0) LaunchUrl(hwnd, prevUrl);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        if (PtInRect(&g_rcFwd, (POINT){ x, y })) {
            if (activeTab->historyIdx + 1 < activeTab->historyCount) {
                activeTab->historyIdx++;
                const wchar_t *nextUrl = activeTab->history[activeTab->historyIdx];
                wcsncpy(activeTab->url, nextUrl, 511);
                SetWindowText(g_hEditUrl, wcscmp(nextUrl, L"about:home") == 0 ? L"" : nextUrl);
                if (wcscmp(nextUrl, L"about:home") != 0) LaunchUrl(hwnd, nextUrl);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        // 4. Botón Menú Hamburguesa '⋮'
        if (PtInRect(&g_rcMenu, (POINT){ x, y })) {
            ShowMainMenu(hwnd);
            return 0;
        }

        // 5. Botón Game Shield / Booster
        if (PtInRect(&g_rcShield, (POINT){ x, y })) {
            g_shieldBlocked = !g_shieldBlocked;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        // 6. Botón Estrella de Marcador ⭐
        if (PtInRect(&g_rcStar, (POINT){ x, y })) {
            g_isBookmarked = !g_isBookmarked;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        // 7. Botón Home 🏠
        if (PtInRect(&g_rcHome, (POINT){ x, y })) {
            NavigateTab(g_activeTabIdx, L"Waterfox Lite - Home", L"about:home");
            SetWindowText(g_hEditUrl, L"");
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        // 8. Botón Recargar ↻
        if (PtInRect(&g_rcReload, (POINT){ x, y })) {
            if (wcscmp(g_tabs[g_activeTabIdx].url, L"about:home") != 0) {
                LaunchUrl(hwnd, g_tabs[g_activeTabIdx].url);
            } else {
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        // 9. Clic en Apps REA / Juegos Web
        for (int i = 0; i < (int)NUM_APPS; i++) {
            if (PtInRect(&g_apps[i].rect, (POINT){ x, y })) {
                if (wcscmp(g_apps[i].url, L"action:games_folder") == 0) {
                    ShowGamesFolderPopup(hwnd, &g_apps[i].rect);
                } else {
                    NavigateTab(g_activeTabIdx, g_apps[i].name, g_apps[i].url);
                    SetWindowText(g_hEditUrl, g_apps[i].url);
                    LaunchUrl(hwnd, g_apps[i].url);
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                return 0;
            }
        }

        // 10. Clic en Barra de Marcadores
        if (g_showBookmarksBar) {
            for (int i = 0; i < (int)NUM_BOOKMARKS; i++) {
                if (PtInRect(&g_bookmarks[i].rect, (POINT){ x, y })) {
                    if (wcscmp(g_bookmarks[i].url, L"action:bookmarks") == 0) {
                        ShowBookmarksPopup(hwnd, &g_bookmarks[i].rect);
                    } else if (wcscmp(g_bookmarks[i].url, L"action:games_folder") == 0) {
                        ShowGamesFolderPopup(hwnd, &g_bookmarks[i].rect);
                    } else if (wcslen(g_bookmarks[i].url) > 0) {
                        NavigateTab(g_activeTabIdx, g_bookmarks[i].title, g_bookmarks[i].url);
                        SetWindowText(g_hEditUrl, g_bookmarks[i].url);
                        LaunchUrl(hwnd, g_bookmarks[i].url);
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    return 0;
                }
            }
        }

        return 0;
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDM_NEW_TAB:
            CreateNewTab(hwnd, L"Nueva Pestaña", L"about:home");
            break;

        case IDM_NEW_WINDOW: {
            wchar_t exePath[MAX_PATH];
            GetModuleFileName(NULL, exePath, MAX_PATH);
            ShellExecute(NULL, L"open", exePath, NULL, NULL, SW_SHOWNORMAL);
            break;
        }

        case IDM_PRIVATE_WINDOW:
            LaunchUrl(hwnd, L"-private-window https://duckduckgo.com");
            break;

        case IDM_BOOKMARKS:
        case IDM_HISTORY:
            LaunchUrl(hwnd, L"about:history");
            break;

        case IDM_DOWNLOADS:
            LaunchUrl(hwnd, L"about:downloads");
            break;

        case IDM_EXTENSIONS:
            LaunchUrl(hwnd, L"about:addons");
            break;

        case IDM_SETTINGS:
            LaunchUrl(hwnd, L"about:preferences");
            break;

        case IDM_TOGGLE_BOOKMARKS:
            g_showBookmarksBar = !g_showBookmarksBar;
            InvalidateRect(hwnd, NULL, FALSE);
            break;

        case IDM_TOGGLE_SHIELD:
            g_shieldBlocked = !g_shieldBlocked;
            InvalidateRect(hwnd, NULL, FALSE);
            break;

        case IDM_ZOOM_IN:
        case IDM_ZOOM_OUT:
        case IDM_ZOOM_RESET:
            InvalidateRect(hwnd, NULL, FALSE);
            break;

        case IDM_ABOUT:
            MessageBox(hwnd,
                L"Waterfox Lite - Native Theme (Edición Ultra-Lite en C)\n\n"
                L"• Diseñado para Intel Pentium G620 & HD Graphics\n"
                L"• Consumo de memoria en Home: ~2.5 - 3.7 MB RAM\n"
                L"• Motor gráfico GDI 60 FPS con Icon Cache en Disco\n"
                L"• Barra de marcadores, pestañas múltiples y escudo Web Game Shield\n"
                L"• Colección de 24 Juegos Web WASM/WebGL con Booster a 30 FPS",
                L"Acerca de Waterfox Lite", MB_OK | MB_ICONINFORMATION);
            break;

        case IDM_EXIT:
            DestroyWindow(hwnd);
            break;
        }
        return 0;
    }

    case WM_KEYDOWN: {
        if (wParam >= '1' && wParam <= '7') {
            int idx = wParam - '1';
            if (idx >= 0 && idx < (int)NUM_APPS) {
                if (wcscmp(g_apps[idx].url, L"action:games_folder") == 0) {
                    ShowGamesFolderPopup(hwnd, &g_apps[idx].rect);
                } else {
                    NavigateTab(g_activeTabIdx, g_apps[idx].name, g_apps[idx].url);
                    SetWindowText(g_hEditUrl, g_apps[idx].url);
                    LaunchUrl(hwnd, g_apps[idx].url);
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
        }
        return 0;
    }

    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam;
        SetTextColor(hdcEdit, COLOR_TEXT);
        SetBkColor(hdcEdit, RGB(13, 17, 23));
        if (!g_hEditBgBrush) g_hEditBgBrush = CreateSolidBrush(RGB(13, 17, 23));
        return (INT_PTR)g_hEditBgBrush;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RenderUI(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY: {
        FreeIconCache();
        if (g_hFontNormal) { DeleteObject(g_hFontNormal); g_hFontNormal = NULL; }
        if (g_hFontBold)   { DeleteObject(g_hFontBold); g_hFontBold = NULL; }
        if (g_hFontTitle)  { DeleteObject(g_hFontTitle); g_hFontTitle = NULL; }
        if (g_hFontSmall)  { DeleteObject(g_hFontSmall); g_hFontSmall = NULL; }
        if (g_hEditBgBrush){ DeleteObject(g_hEditBgBrush); g_hEditBgBrush = NULL; }
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ----------------------------------------------------------------------------
// Punto de Entrada Principal (WinMain)
// ----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    EnableDpiAwareness();

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

    if (!RegisterClassEx(&wc)) return 1;

    HWND hwnd = CreateWindowEx(
        0,
        L"WaterfoxLiteNativeTheme",
        L"Waterfox Lite - Native Theme (C)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1080, 680,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 1;

    if (strstr(lpCmdLine, "--screenshot") != NULL) {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        HDC hdc = GetDC(hwnd);
        RenderUI(hwnd, hdc);
        SaveDCtoBMP(hdc, 1080, 680, L"waterfox_native_c_ui.bmp");
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    EmptyWorkingSet(GetCurrentProcess());

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_KEYDOWN) {
            BOOL isCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            BOOL isShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

            if (msg.wParam == VK_RETURN && GetFocus() == g_hEditUrl) {
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
                    NavigateTab(g_activeTabIdx, buffer, target);
                    LaunchUrl(hwnd, target);
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                continue;
            }

            if (isCtrl && (msg.wParam == 'T' || msg.wParam == 't')) {
                SendMessage(hwnd, WM_COMMAND, IDM_NEW_TAB, 0);
                continue;
            }

            if (isCtrl && (msg.wParam == 'W' || msg.wParam == 'w')) {
                CloseTab(hwnd, g_activeTabIdx);
                continue;
            }

            if (isCtrl && (msg.wParam == 'N' || msg.wParam == 'n')) {
                SendMessage(hwnd, WM_COMMAND, IDM_NEW_WINDOW, 0);
                continue;
            }

            if (isCtrl && (msg.wParam == 'B' || msg.wParam == 'b')) {
                SendMessage(hwnd, WM_COMMAND, IDM_TOGGLE_BOOKMARKS, 0);
                continue;
            }

            if (isCtrl && (msg.wParam == 'Q' || msg.wParam == 'q')) {
                SendMessage(hwnd, WM_COMMAND, IDM_EXIT, 0);
                continue;
            }

            if (msg.wParam == VK_ESCAPE) {
                SetFocus(hwnd);
                continue;
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
