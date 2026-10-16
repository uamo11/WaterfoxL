// Waterfox Lite - Motor XApp Integrado
// Neutraliza telemetría Scribe/Sentry, recicla memoria DOM en scroll largo y bloquea anuncios en x.com

(function() {
  'use strict';
  if (!location.hostname.includes('x.com') && !location.hostname.includes('twitter.com')) return;

  console.log('[Waterfox Lite] Módulo XApp activado.');

  // 1. Neutralizar Sentry
  try {
    Object.defineProperty(window, 'SENTRY_RELEASE', {
      get: () => ({ id: 'neutralized' }),
      set: () => {},
      configurable: true
    });
    Object.defineProperty(window, '_sentryModuleMetadata', {
      get: () => ({}),
      set: () => {},
      configurable: true
    });
  } catch(e) {}

  // 2. Filtrado de red (Scribe & Sentry & Ads)
  const BLOCKED = ['/i/api/1.1/jot', '/i/api/jot', 'sentry.io', 'ads.x.com', 'analytics.twitter.com', 'scribe'];
  const origFetch = window.fetch;
  window.fetch = async function(input, init) {
    const url = typeof input === 'string' ? input : (input && input.url ? input.url : '');
    if (BLOCKED.some(b => url.includes(b))) {
      return new Response('{"status":"ok"}', { status: 200, headers: { 'Content-Type': 'application/json' } });
    }
    return origFetch.apply(this, arguments);
  };

  // 3. Reciclaje de memoria de tweets viejos en el DOM
  let lastGC = Date.now();
  window.addEventListener('scroll', () => {
    const now = Date.now();
    if (now - lastGC < 4000) return;
    lastGC = now;

    const cells = document.querySelectorAll('[data-testid="cellInnerDiv"]');
    if (cells.length > 40) {
      cells.forEach(cell => {
        const r = cell.getBoundingClientRect();
        if (r.bottom < -3000 || r.top > window.innerHeight + 3000) {
          cell.style.display = 'none';
        }
      });
    }
  }, { passive: true });
})();
