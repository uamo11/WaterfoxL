import os
import shutil

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
USER_JS = os.path.join(BASE_DIR, "core", "prefs", "user.js")
CHROME_DIR = os.path.join(BASE_DIR, "core", "chrome")
PAGES_DIR = os.path.join(BASE_DIR, "core", "pages")

TARGET_DIRS = [
    # Custom Kizzy profiles
    r"C:\Users\Usuario\Documents\SCRIPT\KizzyBrowser Script\Waterfoc Browser\Waterfox_Profiles",
    # Standard Waterfox AppData profiles
    os.path.expandvars(r"%APPDATA%\Waterfox\Profiles"),
    # Local Waterfox Lite Portable
    os.path.join(BASE_DIR, "WaterfoxLite_Portable", "profile")
]

def apply_to_profile(profile_path):
    print(f"\n[+] Aplicando optimizaciones a perfil: {profile_path}")
    os.makedirs(profile_path, exist_ok=True)
    
    # 1. Copiar user.js
    dest_user_js = os.path.join(profile_path, "user.js")
    shutil.copy2(USER_JS, dest_user_js)
    print("    -> user.js copiado (pantalla en negro resuelta, lag YTM resuelto, nueva pestana REA)")

    # 2. Copiar carpeta chrome (userChrome.css y userContent.css)
    dest_chrome = os.path.join(profile_path, "chrome")
    os.makedirs(dest_chrome, exist_ok=True)
    for f in os.listdir(CHROME_DIR):
        src_f = os.path.join(CHROME_DIR, f)
        dest_f = os.path.join(dest_chrome, f)
        shutil.copy2(src_f, dest_f)
    print("    -> userChrome.css y userContent.css copiados exitosamente")

    # 3. Copiar newtab.html
    dest_newtab = os.path.join(profile_path, "newtab.html")
    src_newtab = os.path.join(PAGES_DIR, "newtab.html")
    if os.path.exists(src_newtab):
        shutil.copy2(src_newtab, dest_newtab)
        print("    -> newtab.html copiado (acceso directo a Apps REA al abrir ventana)")

def main():
    print("=" * 65)
    print("  Waterfox Lite - Instalador Local de Optimizaciones & Apps REA")
    print("=" * 65)
    
    found_any = False
    for tdir in TARGET_DIRS:
        if os.path.exists(tdir):
            # Check if tdir is itself a profile folder (e.g. WaterfoxLite_Portable\profile)
            if os.path.basename(tdir).lower() == "profile":
                apply_to_profile(tdir)
                found_any = True
                continue
            for entry in os.listdir(tdir):
                full_p = os.path.join(tdir, entry)
                if os.path.isdir(full_p):
                    apply_to_profile(full_p)
                    found_any = True
                    
    if not found_any:
        print("\n[!] No se detectaron perfiles en las rutas habituales.")
        portable_p = os.path.join(BASE_DIR, "portable_profile")
        apply_to_profile(portable_p)
        print(f"[*] Creado perfil portable listo en: {portable_p}")
    else:
        print("\n[OK] Todas las optimizaciones fueron aplicadas a tus perfiles de Waterfox!")
        print("    - Al abrir nueva ventana/pestana se muestran los iconos de las Apps REA.")
        print("    - Pantalla en negro en x.com corregida.")
        print("    - Lag en YouTube / YT Music eliminado.")
        print("    - Debloat de Facebook y Gemini integrado.")

if __name__ == '__main__':
    main()
