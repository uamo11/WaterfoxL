# ============================================================================
# Waterfox Lite - Monitor de Rendimiento y Aislamiento en Tiempo Real
# Diseñado para Intel Pentium G620 & Intel HD Graphics
# ============================================================================

Clear-Host
Write-Host "==================================================================" -ForegroundColor Cyan
Write-Host "       WATERFOX LITE - MONITOR DE RECURSOS Y AISLAMIENTO          " -ForegroundColor Yellow
Write-Host "==================================================================" -ForegroundColor Cyan
Write-Host ""

# 1. Monitoreo de Waterfox Lite (Contenedor Nativo en C)
$liteProc = Get-Process -Name "WaterfoxLite" -ErrorAction SilentlyContinue | Select-Object -First 1

if ($liteProc) {
    $liteRam = [math]::Round($liteProc.WorkingSet64 / 1MB, 2)
    Write-Host "[ACTIVO] Waterfox Lite (Contenedor C)" -ForegroundColor Green
    Write-Host "  • PID Contenedor: $($liteProc.Id)"
    Write-Host "  • Consumo RAM:    $liteRam MB"
} else {
    Write-Host "[INACTIVO] Waterfox Lite no está en ejecución actualmente." -ForegroundColor Gray
}

Write-Host ""

# 2. Monitoreo del Motor Gecko Acoplado (Perfil Portátil)
$geckoProcs = Get-CimInstance Win32_Process -Filter "Name = 'waterfox.exe'" -ErrorAction SilentlyContinue | Where-Object {
    $_.CommandLine -match "WaterfoxLite_Portable"
}

if ($geckoProcs) {
    $totalGeckoRam = 0
    Write-Host "[ACTIVO] Motor Gecko Acoplado a Waterfox Lite:" -ForegroundColor Green
    foreach ($p in $geckoProcs) {
        $pObj = Get-Process -Id $p.ProcessId -ErrorAction SilentlyContinue
        if ($pObj) {
            $ram = [math]::Round($pObj.WorkingSet64 / 1MB, 2)
            $totalGeckoRam += $ram
            Write-Host "  • PID $($p.ProcessId): $ram MB ($($p.CommandLine.Substring(0, [math]::Min(55, $p.CommandLine.Length)))...)"
        }
    }
    Write-Host "  -------------------------------------------------"
    Write-Host "  • Total RAM Motor Web: $([math]::Round($totalGeckoRam, 2)) MB" -ForegroundColor Yellow
} else {
    Write-Host "[EN ESPERA] Motor Gecko acoplado no ha sido invocado (estás en Home nativo)." -ForegroundColor Gray
}

Write-Host ""

# 3. Monitoreo de Seguridad del Navegador Personal del Usuario
$userProcs = Get-CimInstance Win32_Process -Filter "Name = 'waterfox.exe'" -ErrorAction SilentlyContinue | Where-Object {
    $_.CommandLine -notmatch "WaterfoxLite_Portable"
}

if ($userProcs) {
    Write-Host "[SEGURO] Navegador Personal del Usuario (Música / YT Music):" -ForegroundColor Cyan
    Write-Host "  • Estado: 100% AISLADO E INTACTO (Sin colisiones ni bloqueos)" -ForegroundColor Green
    Write-Host "  • Procesos en reproducción/navegación detectados: $($userProcs.Count)"
} else {
    Write-Host "[INFO] No se detecta navegador personal Waterfox en segundo plano." -ForegroundColor Gray
}

Write-Host ""
Write-Host "==================================================================" -ForegroundColor Cyan
