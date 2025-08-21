#!/bin/bash

# =============================================================================
#   CONFIGURACIÓN DE HORARIOS
# =============================================================================
# Modifica estas dos variables para establecer la hora de encendido y apagado.
# Para probar hoy, pon una HORA_ENCENDIDO unos minutos en el futuro.
# Ejemplo para probar: HORA_ENCENDIDO="14:45", HORA_APAGADO="15:00"

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

# Obtener el timestamp (segundos) de la hora de encendido para HOY.
TIMESTAMP_ENCENDIDO_HOY=$(date -d "$HORA_ENCENDIDO" +%s)
TIMESTAMP_ACTUAL=$(date +%s)

# Comprobar si la hora de encendido para hoy ya ha pasado.
if [ "$TIMESTAMP_ENCENDIDO_HOY" -le "$TIMESTAMP_ACTUAL" ]; then
    # Si la hora ya pasó, programamos para mañana.
    echo "[PASO 1] La hora de encendido ($HORA_ENCENDIDO) para hoy ya ha pasado."
    echo "Programando la alarma de encendido para MAÑANA."
    TIMESTAMP_A_PROGRAMAR=$(date -d "tomorrow $HORA_ENCENDIDO" +%s)
else
    # Si la hora aún no ha llegado, programamos para hoy.
    echo "[PASO 1] La hora de encendido para hoy aún no ha llegado."
    echo "Programando la alarma de encendido para HOY."
    TIMESTAMP_A_PROGRAMAR=$TIMESTAMP_ENCENDIDO_HOY
fi

# Formatear la fecha para mostrarla al usuario de forma clara.
FECHA_ENCENDIDO_LEGIBLE=$(date -d "@$TIMESTAMP_A_PROGRAMAR" "+%Y-%m-%d %H:%M:%S")

echo "La Raspberry Pi se encenderá en la fecha: $FECHA_ENCENDIDO_LEGIBLE"

# Usar rtcwake para programar la alarma de encendido.
sudo rtcwake -m no -t $TIMESTAMP_A_PROGRAMAR

if [ $? -eq 0 ]; then
    echo "¡Alarma de encendido programada con éxito!"
else
    echo "Error: No se pudo programar la alarma de encendido. Asegúrate de ejecutar el script con sudo."
    exit 1
fi

echo ""

# --- Parte 2: Esperar hasta la hora de apagado y apagar el sistema ---
# (Esta parte funciona igual que antes)

TIMESTAMP_APAGADO_HOY=$(date -d "$HORA_APAGADO" +%s)

if [ "$TIMESTAMP_ACTUAL" -gt "$TIMESTAMP_APAGADO_HOY" ]; then
    echo "[PASO 2] La hora de apagado ($HORA_APAGADO) para hoy ya ha pasado."
    echo "El sistema no se apagará automáticamente. El próximo encendido ya está programado."
    exit 0
fi

echo "[PASO 2] El sistema esperará hasta las $HORA_APAGADO de hoy para apagarse."
if command -v at &> /dev/null
then
    echo "shutdown -h now" | at $HORA_APAGADO
    echo "Apagado programado para las $HORA_APAGADO usando 'at'."
else
    SEGUNDOS_HASTA_APAGADO=$((TIMESTAMP_APAGADO_HOY - TIMESTAMP_ACTUAL))
    echo "El comando 'at' no está instalado. Usando 'sleep' para esperar $SEGUNDOS_HASTA_APAGADO segundos."
    sleep $SEGUNDOS_HASTA_APAGADO && sudo shutdown -h now &
fi

echo "----------------------------------------------------"