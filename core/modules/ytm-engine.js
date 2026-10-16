// Waterfox Lite - Motor YTM Integrado
// Bloquea telemetría de estadísticas de YouTube Music, salta anuncios y reduce el uso de CPU

(function() {
  'use strict';
  if (!location.hostname.includes('music.youtube.com')) return;

  console.log('[Waterfox Lite] Módulo YTM activado.');

  // 1. Filtrado de telemetría invasiva (QoE, Clearcut, Stats)
  const BLOCKED = ['play.google.com/log', '/api/stats/qoe', '/api/stats/playback', '/api/stats/atr', 'google-analytics.com'];
  const origFetch = window.fetch;
  window.fetch = async function(input, init) {
    const url = typeof input === 'string' ? input : (input && input.url ? input.url : '');
    if (BLOCKED.some(b => url.includes(b))) {
      return new Response('{"status":"ok"}', { status: 200, headers: { 'Content-Type': 'application/json' } });
    }
    return origFetch.apply(this, arguments);
  };

  // 2. Saltar cortes de anuncios automáticamente
  setInterval(() => {
    const adBadge = document.querySelector('.ad-showing, .ad-interrupting');
    const video = document.querySelector('video');
    if (adBadge && video) {
      video.muted = true;
      video.currentTime = video.duration || 9999;
      const skipBtn = document.querySelector('.ytp-ad-skip-button, .ytp-ad-skip-button-modern');
      if (skipBtn) skipBtn.click();
    }
  }, 500);
})();
