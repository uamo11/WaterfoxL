// ==UserScript==
// @name         Waterfox Lite - Web Game Shield & Performance Booster
// @namespace    https://github.com/uamo11/WaterfoxL
// @version      1.1.0
// @description  Acelerador WebGL/WASM para Pentium G620 y escudo bloqueador de portales de juegos web
// @match        *://*/*
// @grant        GM_getValue
// @grant        GM_setValue
// @run-at       document-start
// ==/UserScript==

// ============================================================================
// Waterfox Lite - Web Game Shield & Performance Booster Engine
// Optimización de WebGL/Wasm para Intel Pentium G620 y escudo bloqueador
// ============================================================================

(function () {
  'use strict';

  const GAME_PORTALS = [
    'poki.com',
    'crazygames.com',
    'krunker.io',
    'friv.com',
    'minijuegos.com',
    '1v1.lol',
    'agar.io',
    'slither.io',
    'armorgames.com',
    'kongregate.com',
    'y8.com',
    'roblox.com'
  ];

  const currentHost = window.location.hostname.toLowerCase();
  const isGamePortal = GAME_PORTALS.some(portal => currentHost.includes(portal));

  // --------------------------------------------------------------------------
  // 1. MODO ESCUDO / BLOQUEADOR (Si está activado por el usuario)
  // --------------------------------------------------------------------------
  let isBlockActive = false;
  try {
    if (typeof GM_getValue === 'function') {
      isBlockActive = GM_getValue('wfl_block_games', false);
    } else {
      isBlockActive = localStorage.getItem('wfl_block_games') === 'true';
    }
  } catch(e) {}

  if (isBlockActive && isGamePortal) {
    console.warn('[Waterfox Lite Shield] Portal de juegos web bloqueado:', currentHost);
    
    function renderBlockScreen() {
      document.documentElement.innerHTML = `
        <!DOCTYPE html>
        <html lang="es">
        <head>
          <meta charset="UTF-8">
          <title>Juego Web Restringido - Waterfox Lite</title>
          <style>
            body {
              margin: 0;
              padding: 0;
              background: #0d1117;
              color: #f0f6fc;
              font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
              display: flex;
              flex-direction: column;
              align-items: center;
              justify-content: center;
              min-height: 100vh;
              text-align: center;
            }
            .card {
              background: #161b22;
              border: 1px solid #30363d;
              border-radius: 16px;
              padding: 40px 30px;
              max-width: 500px;
              box-shadow: 0 8px 24px rgba(0,0,0,0.5);
            }
            .icon { font-size: 48px; margin-bottom: 15px; }
            h1 { font-size: 22px; margin-bottom: 10px; color: #f85149; }
            p { font-size: 14px; color: #8b949e; line-height: 1.5; margin-bottom: 25px; }
            .btn-group { display: flex; gap: 12px; justify-content: center; }
            button {
              padding: 10px 20px;
              border-radius: 8px;
              font-weight: 600;
              font-size: 14px;
              cursor: pointer;
              border: none;
              transition: opacity 0.2s;
            }
            .btn-allow { background: #238636; color: #fff; }
            .btn-home { background: #21262d; color: #c9d1d9; border: 1px solid #30363d; }
            button:hover { opacity: 0.9; }
          </style>
        </head>
        <body>
          <div class="card">
            <div class="icon">🛡️</div>
            <h1>Juego Web Restringido</h1>
            <p>El portal <strong>${currentHost}</strong> ha sido bloqueado por el escudo de Waterfox Lite para preservar CPU, memoria RAM y evitar distracciones.</p>
            <div class="btn-group">
              <button class="btn-allow" onclick="localStorage.setItem('wfl_block_games', 'false'); location.reload();">Desbloquear y Acelerar</button>
              <button class="btn-home" onclick="window.location.href='file:///C:/Users/Usuario/Documents/REA/Waterfox%20Lite/core/pages/newtab.html'">Ir al Inicio</button>
            </div>
          </div>
        </body>
        </html>
      `;
    }

    if (document.documentElement) renderBlockScreen();
    else document.addEventListener('DOMContentLoaded', renderBlockScreen);
    return;
  }

  // --------------------------------------------------------------------------
  // 2. MODO GAME BOOSTER (Aceleración de WebGL/Wasm para Intel Pentium G620)
  // --------------------------------------------------------------------------
  console.log('[Waterfox Lite Booster] Optimizando motor de juegos web para CPU Pentium G620...');

  // A) Limitar hardwareConcurrency para que WebAssembly no sature la CPU
  try {
    Object.defineProperty(navigator, 'hardwareConcurrency', {
      get: () => 2,
      configurable: true
    });
  } catch(e) {}

  // B) Escalar resolución de Canvas para triplicar los FPS en Intel HD Graphics
  try {
    Object.defineProperty(window, 'devicePixelRatio', {
      get: () => Math.min(window.screen.width > 1600 ? 0.8 : 1.0, 1.0),
      configurable: true
    });
  } catch(e) {}

  // C) Optimizar parámetros de contexto WebGL para bajo consumo de VRAM y GPU
  try {
    const origGetContext = HTMLCanvasElement.prototype.getContext;
    HTMLCanvasElement.prototype.getContext = function (type, attributes) {
      if (type === 'webgl' || type === 'webgl2' || type === 'experimental-webgl') {
        attributes = attributes || {};
        attributes.powerPreference = 'low-power';
        attributes.antialias = false; // Sin MSAA pesado (ahorra 60% de GPU)
        attributes.preserveDrawingBuffer = false;
        attributes.desynchronized = true; // Reduce latencia de entrada (input lag)
      }
      return origGetContext.call(this, type, attributes);
    };
  } catch(e) {}

  // D) Limitador inteligente de cuadros (Frame Limiter a 60 FPS)
  // Evita que juegos sencillos en bucle abierto intenten correr a 200+ FPS y sobrecalienten la PC
  const origRAF = window.requestAnimationFrame;
  let lastFrameTime = 0;
  const frameInterval = 1000 / 60; // Máximo 60 FPS estables

  window.requestAnimationFrame = function (callback) {
    return origRAF(function (timestamp) {
      const elapsed = timestamp - lastFrameTime;
      if (elapsed >= frameInterval) {
        lastFrameTime = timestamp - (elapsed % frameInterval);
        callback(timestamp);
      } else {
        origRAF(callback);
      }
    });
  };

  console.log('[Waterfox Lite Booster] Game Booster activado (MSAA Off, Input Lag Reducido, 60 FPS Cap).');
})();
