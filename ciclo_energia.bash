#!/bin/bash

# =============================================================================
#   CONFIGURACIÓN DE HORARIOS
# =============================================================================
# Modifica estas dos variables para establecer la hora de encendido y apagado.
# Utiliza el formato de 24 horas "HH:MM".

HORA_ENCENDIDO="18:00"
HORA_APAGADO="18:15"

# =============================================================================
#   INICIO DEL SCRIPT (No es necesario modificar a partir de aquí)
# =============================================================================

echo "----------------------------------------------------"
echo "SCRIPT DE ENCENDIDO Y APAGADO PROGRAMADO PARA RASPBERRY PI 5"
echo "----------------------------------------------------"
echo "Hora de encendido configurada: $HORA_ENCENDIDO"
echo "Hora de apagado configurada:   $HORA_APAGADO"
echo ""

# --- Parte 1: Programar el próximo encendido ---

# Convertir la hora de encendido a un timestamp de Unix para mañana.
# El comando 'date -d "tomorrow HH:MM"' calcula la fecha y hora exactas.
TIMESTAMP_ENCENDIDO=$(date -d "tomorrow $HORA_ENCENDIDO" +%s)

# Formatear la fecha para mostrarla al usuario de forma clara.
FECHA_ENCENDIDO_LEGIBLE=$(date -d "@$TIMESTAMP_ENCENDIDO" "+%Y-%m-%d %H:%M:%S")

echo "[PASO 1] Programando la alarma de encendido para mañana."
echo "La Raspberry Pi se encenderá en la fecha: $FECHA_ENCENDIDO_LEGIBLE"

# Usar rtcwake para programar la alarma de encendido.
# -m no: Indica que no se apagará inmediatamente con este comando.
# -t <timestamp>: Establece la hora de la alarma usando un timestamp de Unix.
# El comando necesita privilegios de superusuario (sudo).
sudo rtcwake -m no -t $TIMESTAMP_ENCENDIDO

if [ $? -eq 0 ]; then
    echo "¡Alarma de encendido programada con éxito!"
else
    echo "Error: No se pudo programar la alarma de encendido. Asegúrate de ejecutar el script con sudo."
    exit 1
fi

echo ""

# --- Parte 2: Esperar hasta la hora de apagado y apagar el sistema ---

# Convertir la hora de apagado a un timestamp de Unix para hoy.
TIMESTAMP_APAGADO_HOY=$(date -d "$HORA_APAGADO" +%s)
TIMESTAMP_ACTUAL=$(date +%s)

# Comprobar si la hora de apagado ya pasó hoy.
if [ "$TIMESTAMP_ACTUAL" -gt "$TIMESTAMP_APAGADO_HOY" ]; then
    echo "[PASO 2] La hora de apagado ($HORA_APAGADO) para hoy ya ha pasado."
    echo "El sistema no se apagará automáticamente. Deberás apagarlo manualmente."
    echo "El próximo encendido ya está programado."
    exit 0
fi

echo "[PASO 2] El sistema esperará hasta las $HORA_APAGADO de hoy para apagarse."
echo "Puedes cerrar esta terminal, el proceso seguirá en segundo plano."
echo "Para cancelar el apagado, ejecuta: sudo pkill -f 'shutdown -h'"

# Usar el comando 'at' o un bucle 'sleep' para apagar a la hora exacta.
# 'at' es más robusto. Si no está instalado, puedes instalarlo con: sudo apt install at
if command -v at &> /dev/null
then
    echo "shutdown -h now" | at $HORA_APAGADO
    echo "Apagado programado para las $HORA_APAGADO usando 'at'."
else
    # Alternativa si 'at' no está instalado (menos precisa)
    SEGUNDOS_HASTA_APAGADO=$((TIMESTAMP_APAGADO_HOY - TIMESTAMP_ACTUAL))
    echo "El comando 'at' no está instalado. Usando 'sleep' para esperar $SEGUNDOS_HASTA_APAGADO segundos."
    sleep $SEGUNDOS_HASTA_APAGADO && sudo shutdown -h now &
fi

echo "----------------------------------------------------"