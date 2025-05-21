#!/bin/bash

#URL="https://phpmyadmin.control3v.com"
URL="http://control3v.com/reportes"
REQUESTS=50  # Total de peticiones a realizar
CONCURRENCY=10  # Número de peticiones concurrentes

echo "Probando limitación de peticiones contra $URL ..."
echo "Realizando $REQUESTS peticiones con $CONCURRENCY en paralelo..."
echo

# Usar Apache Benchmark para pruebas más realistas
ab -n $REQUESTS -c $CONCURRENCY -H "Host: control3v.com" $URL

# Alternativa con curl para ver códigos de respuesta individuales
echo
echo "Prueba detallada con curl:"
total=0
limitados=0

for i in {1..30}; do
  total=$((total + 1))
  status=$(curl -s -o /dev/null -w "%{http_code}" "$URL")
  echo "[$i] Código de respuesta: $status"

  if [ "$status" -eq 429 ]; then
    limitados=$((limitados + 1))
  fi
  
  # Pequeña pausa entre algunas peticiones
  if (( i % 5 == 0 )); then
    sleep 0.5
  fi
done

echo
echo "✅ Total de peticiones: $total"
echo "❌ Rechazadas por límite (429): $limitados"
