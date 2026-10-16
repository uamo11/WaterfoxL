// ============================================================================
// Waterfox Lite - Perfil de Optimización para CPUs Antiguas y GPUs Heredadas
// Diseñado para: Intel Pentium G620 / Core 2 Duo / Sandy Bridge & Intel HD Graphics
// ============================================================================

// ----------------------------------------------------------------------------
// 1. SOLUCIÓN AL PROBLEMA DE PANTALLA EN NEGRO EN X (TWITTER) Y WEBGPU/CANVAS
// ----------------------------------------------------------------------------
user_pref("gfx.webrender.all", false);
user_pref("gfx.webrender.software", true);
user_pref("gfx.webrender.software.opengl", false);
user_pref("gfx.canvas.accelerated", false);
user_pref("layers.acceleration.disabled", true);
user_pref("gfx.direct2d.disabled", true);
user_pref("dom.webgpu.enabled", false);
user_pref("webgl.disabled", false);
user_pref("webgl.force-enabled", false);
user_pref("layout.css.backdrop-filter.enabled", true);
user_pref("widget.windows.window_occlusion_tracking.enabled", false);

// ----------------------------------------------------------------------------
// 2. SOLUCIÓN AL LAGG EN YOUTUBE MUSIC Y VIDEOS (FORZAR H.264 / EVITAR VP9/AV1)
// ----------------------------------------------------------------------------
user_pref("media.mediasource.vp9.enabled", false);
user_pref("media.av1.enabled", false);
user_pref("media.hardware-video-decoding.enabled", true);
user_pref("media.hardware-video-decoding.failed", false);
user_pref("media.ffmpeg.vaapi.enabled", false);
user_pref("media.rdd-vpx.enabled", false);
user_pref("media.rdd-ffmpeg.enabled", false);
user_pref("media.cache_size", 102400); // 100MB buffer para evitar micro-cortes
user_pref("media.memory_cache_max_size", 65536);
user_pref("media.autoplay.default", 0);

// ----------------------------------------------------------------------------
// 3. OPTIMIZACIÓN MULTIPROCESO PARA DUAL-CORE (PENTIUM G620: 2 NÚCLEOS / 2 HILOS)
// ----------------------------------------------------------------------------
user_pref("dom.ipc.processCount", 2);
user_pref("dom.ipc.processCount.webIsolated", 1);
user_pref("dom.ipc.keepProcessesAlive.web", 1);
user_pref("dom.ipc.processPrelaunch.enabled", false);

// ----------------------------------------------------------------------------
// 4. GESTIÓN AGRESIVA DE MEMORIA RAM
// ----------------------------------------------------------------------------
user_pref("browser.tabs.unloadOnLowMemory", true);
user_pref("browser.cache.memory.enable", true);
user_pref("browser.cache.memory.capacity", 65536); // Máximo 64 MB de caché en RAM
user_pref("browser.sessionhistory.max_entries", 10);
user_pref("browser.sessionhistory.max_total_viewers", 1);
user_pref("image.mem.decode_bytes_at_a_time", 32768);
user_pref("image.mem.surfacecache.max_size_kb", 524288);

// ----------------------------------------------------------------------------
// 5. REDUCCIÓN DE TELEMETRÍA Y PROCESOS EN SEGUNDO PLANO
// ----------------------------------------------------------------------------
user_pref("toolkit.telemetry.enabled", false);
user_pref("toolkit.telemetry.unified", false);
user_pref("toolkit.telemetry.archive.enabled", false);
user_pref("experiments.supported", false);
user_pref("experiments.enabled", false);
user_pref("network.prefetch-next", false);
user_pref("network.dns.disablePrefetch", true);
user_pref("network.http.speculative-parallel-limit", 0);
user_pref("browser.newtabpage.activity-stream.feeds.telemetry", false);
user_pref("browser.newtabpage.activity-stream.telemetry", false);
user_pref("browser.ping-centre.telemetry", false);

// ----------------------------------------------------------------------------
// 6. HABILITAR SOPORTE PARA userChrome.css Y userContent.css
// ----------------------------------------------------------------------------
user_pref("toolkit.legacyUserProfileCustomizations.stylesheets", true);

// ----------------------------------------------------------------------------
// 7. PÁGINA DE INICIO Y NUEVA VENTANA/PESTAÑA CON ACCESO DIRECTO A APPS REA
// ----------------------------------------------------------------------------
user_pref("browser.startup.homepage", "file:///C:/Users/Usuario/Documents/REA/Waterfox%20Lite/core/pages/newtab.html");
user_pref("browser.startup.page", 1);
user_pref("browser.newtabpage.enabled", true);
user_pref("browser.newtab.url", "file:///C:/Users/Usuario/Documents/REA/Waterfox%20Lite/core/pages/newtab.html");
