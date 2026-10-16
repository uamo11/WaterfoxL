// ==UserScript==
// @name         Waterfox Lite - GTA V Web (FivePlay) Extreme Booster
// @namespace    https://github.com/uamo11/WaterfoxL
// @version      1.0.0
// @description  Inyector de parámetros de ultrabajo consumo (360p, No Shadows, 30 FPS cap, 2 cores) y bloqueador de adware en fiveplay.lol
// @match        https://fiveplay.lol/*
// @grant        none
// @run-at       document-start
// ==/UserScript==

(function () {
  'use strict';

  // 1. Inyección forzada de parámetros de ultrabajo consumo en la URL
  try {
    const url = new URL(window.location.href);
    let needsRedirect = false;

    const TARGET_PARAMS = {
      'low': '1',       // Perfil low-memory (sin sombras, peds 0, vehiculos 0)
      'fps': '30',      // Limitador nativo a 30 FPS (-frameLimit=2)
      'cores': '2',     // Reducir hilos a 2 núcleos para Pentium G620
      'res': '640x360', // Resolución 360p ultra ligera
      'shadows': '0',   // Desactivar el pase de sombras en cascada
      'verbose': '0'    // Desactivar logs pesados
    };

    for (const [key, val] of Object.entries(TARGET_PARAMS)) {
      if (!url.searchParams.has(key)) {
        url.searchParams.set(key, val);
        needsRedirect = true;
      }
    }

    if (needsRedirect && (window.location.pathname === '/' || window.location.pathname === '')) {
      console.warn('[Waterfox Lite FivePlay Booster] Aplicando parámetros de rendimiento extremo:', url.search);
      window.location.replace(url.toString());
      return;
    }
  } catch (e) {
    console.error('[Waterfox Lite FivePlay Booster] Error en URL injection:', e);
  }

  // 2. Interceptor y supresor de scripts de publicidad y telemetría parásita
  const observer = new MutationObserver((mutations) => {
    for (const mutation of mutations) {
      for (const node of mutation.addedNodes) {
        if (node.nodeType === Node.ELEMENT_NODE) {
          const src = node.getAttribute('src') || '';
          if (src.includes('quge5') || src.includes('alwingulla') || src.includes('monetag') || src.includes('cloudflareinsights')) {
            node.remove();
            console.log('[Waterfox Lite FivePlay Booster] Script parásito neutralizado:', src);
          }
        }
      }
    }
  });

  observer.observe(document.documentElement || document, { childList: true, subtree: true });

  console.log('[Waterfox Lite FivePlay Booster] Motor de optimización extremo activado para Pentium G620.');
})();
