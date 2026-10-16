# Waterfox Lite

> Navegador web ultra-optimizado diseñado específicamente para PCs con procesadores de 2 núcleos (Intel Pentium / Core 2 / Sandy Bridge) y gráficos integrados heredados (Intel HD Graphics).

---

## 🚀 Problemas Resueltos en Esta Edición

### 1. 𝕏 (Twitter) Pantalla en Negro Solucionada
* **Causa:** En procesadores como el **Intel Pentium G620** con GPU **Intel HD Graphics (Sandy Bridge)**, WebRender por hardware y DirectComposition sufren fallos de contexto y dejan el viewport de `x.com` completamente en negro debido a filtros CSS pesados y aceleración fallida.
* **Solución Waterfox Lite:**
  - Habilita **Software WebRender** (`gfx.webrender.software = true`), garantizando 100% de estabilidad y renderizado sin cuelgues ni pantallas negras.
  - Desactiva aceleración de canvas inestable (`gfx.canvas.accelerated = false`).
  - Inyecta hojas de estilo que anulan `backdrop-filter: blur(...)` sin romper el diseño.

### 2. 🎵 YouTube Music sin Lagg ni Tirones
* **Causa:** El procesador Pentium G620 carece de decodificación por hardware para los códecs modernos **VP9** y **AV1**. YouTube Music intenta decodificar video VP9 por software en segundo plano, saturando ambos núcleos de CPU al **100%** y provocando cortes de audio y lagg en el sistema.
* **Solución Waterfox Lite:**
  - Desactiva forzosamente VP9 y AV1 (`media.mediasource.vp9.enabled = false`, `media.av1.enabled = false`), obligando a YouTube a enviar streams **H.264** acelerados por hardware por Intel QuickSync o audio nativo ligero.
  - Aumenta el búfer de caché de medios a 100 MB para evitar micro-cortes durante picos de disco.
  - Bloquea telemetría intensiva de estadísticas (`/api/stats/qoe`, `play.google.com/log`).

### 3. ⚡ Compatibilidad Mejorada con CPUs Viejas
* **Limitación de Procesos:** Ajustado a **2 procesos de contenido** (`dom.ipc.processCount = 2`) para coincidir exactamente con los 2 núcleos y 2 hilos del Pentium G620, eliminando la sobrecarga del planificador de Windows.
* **Descarga de Pestañas Inactivas:** Libera automáticamente de la memoria RAM las pestañas que no estás usando (`browser.tabs.unloadOnLowMemory = true`).
* **Telemetría Anulada:** Cero conexiones de fondo de telemetría de Mozilla, Normandy, Pocket o experimentos.

### 4. 🛡️ Integración Nativa de los Motores `xapp` y `ytm`
* **Módulo XApp:** Bloquea llamadas a `/i/api/1.1/jot` (Scribe), suprime Sentry Tracing y recicla elementos antiguos del DOM al hacer scroll largo para evitar fugas de memoria.
* **Módulo YTM:** Bloqueo y salto de anuncios en YouTube Music, eliminación de avisos emergentes y ahorro de CPU.

---

## 📦 Estructura del Repositorio

```
WaterfoxL/
├── .github/workflows/
│   └── build.yml               # Flujo de compilación y empaquetado automático en GitHub Actions
├── core/
│   ├── prefs/
│   │   └── user.js             # Preferencias de bajo consumo y estabilidad gráfica
│   ├── chrome/
│   │   ├── userChrome.css      # UI minimalista y ágil
│   │   └── userContent.css     # Supresión de blur y anuncios a nivel de motor
│   └── modules/
│       ├── xapp-engine.js      # Debloater y reciclador DOM para x.com
│       └── ytm-engine.js       # Optimizador y supresor de telemetría para YTM
└── scripts/
    └── apply_to_local_waterfox.py # Instalador de 1 clic para tus perfiles locales
```

---

## 🛠️ Cómo Usar en tu PC

### Opción 1: Aplicar a tus perfiles actuales de Waterfox
Ejecuta en consola:
```powershell
python scripts/apply_to_local_waterfox.py
```
El script detectará automáticamente tus perfiles existentes (incluyendo perfiles de KizzyBrowser) y aplicará las configuraciones `user.js` y `chrome/` al instante.

### Opción 2: Compilación y Descarga con GitHub Actions
Este repositorio cuenta con un flujo automatizado en `.github/workflows/build.yml` que:
1. Ensambla la versión portable de Waterfox x64.
2. Inyecta todas las políticas, preferencias y módulos de `Waterfox Lite`.
3. Genera un archivo ZIP portable listo para descomprimir y usar en cualquier PC.

---

## 📜 Licencia
Basado en Waterfox bajo la Licencia Pública de Mozilla (MPL 2.0). Optimizaciones desarrolladas para máxima eficiencia en hardware modesto.
