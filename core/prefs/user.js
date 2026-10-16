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
user_pref("webgl.msaa-force", false);
user_pref("webgl.max-contexts-per-principal", 8);
user_pref("javascript.options.wasm_simd", true);
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
user_pref("media.wmf.vp9.enabled", false);
user_pref("dom.webcodecs.enabled", false);
user_pref("media.cache_size", 102400); // 100MB buffer para evitar micro-cortes
user_pref("media.memory_cache_max_size", 65536);
user_pref("media.autoplay.default", 0);

// ----------------------------------------------------------------------------
// 3. AISLAMIENTO Y ESTABILIDAD DE PROCESOS GRÁFICOS (PREVENCIÓN DE CRASHES)
// ----------------------------------------------------------------------------
user_pref("webgl.out-of-process", true);
user_pref("dom.ipc.processHangMonitor", true);
user_pref("dom.ipc.processHangTimeoutSecs", 15);
user_pref("dom.ipc.reportProcessHangs", false);
user_pref("gfx.direct3d11.reuse-decoder-device", false);

// ----------------------------------------------------------------------------
// 4. AFINACIÓN DE JIT SPIDERMONKEY PARA CPU DUAL-CORE (PENTIUM G620)
// ----------------------------------------------------------------------------
// Limita los hilos ayudantes de compilación JS en segundo plano para no
// competir con el hilo principal de renderizado y reproducción de audio.
user_pref("javascript.options.parallel_parsing", false);
user_pref("javascript.options.mem.gc_incremental", true);
user_pref("javascript.options.mem.gc_compacting", true);
user_pref("javascript.options.mem.gc_high_frequency_time_limit_ms", 1000);
user_pref("javascript.options.mem.gc_incremental_slice_ms", 5);
user_pref("javascript.options.mem.high_water_mark", 128);

// ----------------------------------------------------------------------------
// 5. PIPELINE DE RED Y ACELERACIÓN DE RESOLUCIÓN DNS
// ----------------------------------------------------------------------------
user_pref("network.http.max-connections", 128);
user_pref("network.http.max-persistent-connections-per-server", 6);
user_pref("network.http.max-urgent-start-connections", 3);
user_pref("network.ssl_tokens_cache_capacity", 2048);
user_pref("network.dns.disableIPv6", true);
user_pref("network.http.http3.enable", true);

// ----------------------------------------------------------------------------
// 6. SCROLL CINEMÁTICO DE BAJO CONSUMO (SIN CARGA GPU)
// ----------------------------------------------------------------------------
user_pref("general.smoothScroll", true);
user_pref("general.smoothScroll.lines.durationMaxMS", 120);
user_pref("general.smoothScroll.mouseWheel.durationMaxMS", 180);
user_pref("general.smoothScroll.currentVelocityWeighting", "0.25");
user_pref("general.smoothScroll.stopDecelerationWeighting", "0.4");

// ----------------------------------------------------------------------------
// 7. OPTIMIZACIÓN MULTIPROCESO PARA DUAL-CORE (PENTIUM G620: 2 NÚCLEOS / 2 HILOS)
// ----------------------------------------------------------------------------
user_pref("dom.ipc.processCount", 2);
user_pref("dom.ipc.processCount.webIsolated", 1);
user_pref("dom.ipc.keepProcessesAlive.web", 1);
user_pref("dom.ipc.processPrelaunch.enabled", false);

// ----------------------------------------------------------------------------
// 8. GESTIÓN AGRESIVA DE MEMORIA RAM Y PREVENCIÓN DE OOM
// ----------------------------------------------------------------------------
user_pref("browser.tabs.unloadOnLowMemory", true);
user_pref("browser.low_commit_space_threshold_mb", 1024);
user_pref("browser.tabs.min_inactive_duration_before_unload", 300000); // 5 min
user_pref("browser.cache.memory.enable", true);
user_pref("browser.cache.memory.capacity", 65536); // 64 MB de caché en RAM
user_pref("browser.cache.disk.smart_size.enabled", true);
user_pref("browser.cache.disk.capacity", 358400); // 350 MB en disco
user_pref("browser.sessionhistory.max_entries", 10);
user_pref("browser.sessionhistory.max_total_viewers", 1);
user_pref("image.mem.decode_bytes_at_a_time", 32768);
user_pref("image.mem.surfacecache.max_size_kb", 524288);

// ----------------------------------------------------------------------------
// 9. REDUCCIÓN DE TELEMETRÍA Y PROCESOS EN SEGUNDO PLANO
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
// 10. HABILITAR SOPORTE PARA userChrome.css Y userContent.css
// ----------------------------------------------------------------------------
user_pref("toolkit.legacyUserProfileCustomizations.stylesheets", true);

// ----------------------------------------------------------------------------
// 11. PÁGINA DE INICIO Y NUEVA VENTANA/PESTAÑA CON ACCESO DIRECTO A APPS REA
// ----------------------------------------------------------------------------
user_pref("browser.startup.homepage", "file:///C:/Users/Usuario/Documents/REA/Waterfox%20Lite/core/pages/newtab.html");
user_pref("browser.startup.page", 1);
user_pref("browser.newtabpage.enabled", true);
user_pref("browser.newtab.url", "file:///C:/Users/Usuario/Documents/REA/Waterfox%20Lite/core/pages/newtab.html");

// ----------------------------------------------------------------------------
// 12. SOPORTE WEBGPU & SHAREDARRAYBUFFER (FIVEPLAY / GTA V WEB)
// ----------------------------------------------------------------------------
user_pref("dom.webgpu.enabled", true);
user_pref("dom.webgpu.workers.enabled", true);
user_pref("gfx.webgpu.force-enabled", true);
user_pref("javascript.options.shared_memory", true);
user_pref("dom.postMessage.sharedArrayBuffer.bypassCOOP_COEP.insecure.enabled", true);
user_pref("dom.min_timeout_value", 1);
user_pref("privacy.reduceTimerPrecision", false);
