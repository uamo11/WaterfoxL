// ============================================================================
// Waterfox Lite - Perfil de Optimización para CPUs Antiguas y GPUs Heredadas
// Diseñado para: Intel Pentium G620 / Core 2 Duo / Sandy Bridge & Intel HD Graphics
// ============================================================================

// ----------------------------------------------------------------------------
// 1. SOLUCIÓN AL PROBLEMA DE PANTALLA EN NEGRO EN X (TWITTER) Y WEBGPU/CANVAS
// ----------------------------------------------------------------------------
// En GPUs Intel HD Graphics de 2ª generación (Sandy Bridge), el hardware
// WebRender y DirectComposition causan fallos de contexto y pantallas negras.
// La solución definitiva es Software WebRender (rápido, estable y sin cuelgues).
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
// Los procesadores Pentium G620 / viejos NO tienen decodificación por hardware
// de VP9 ni AV1. Forzar su decodificación por software satura los 2 núcleos al 100%.
// Al desactivar VP9 y AV1, YouTube envía H.264 (acelerado por Intel QuickSync) o audio directo.
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
// Por defecto, Waterfox/Firefox lanza 8-10 procesos de contenido, saturando la
// cola del planificador de la CPU. Reducir a 2 procesos balancea memoria y CPU.
user_pref("dom.ipc.processCount", 2);
user_pref("dom.ipc.processCount.webIsolated", 1);
user_pref("dom.ipc.keepProcessesAlive.web", 1);
user_pref("dom.ipc.processPrelaunch.enabled", false);

// ----------------------------------------------------------------------------
// 4. GESTIÓN AGRESIVA DE MEMORIA RAM
// ----------------------------------------------------------------------------
// Evita fugas de memoria y descarga pestañas en segundo plano cuando la RAM escasea.
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
