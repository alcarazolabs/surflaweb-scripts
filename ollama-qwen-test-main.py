import ollama

# Definir el rol
ROL_QWEN = """
Actúa como un experto en ciencias de la computación.
Tu respuesta debe ser como el texto de una página de un libro.
Es crucial que la respuesta sea solo texto plano.
No uses negritas, cursivas, listas con asteriscos, guiones, ni ningún otro símbolo de formato Markdown.
No uses encabezados como '###' o '**'.
No incluyas código ni ejemplos de código.
Solo proporciona información en oraciones completas y párrafos.
Sé lo más simple y directo posible.
La respuesta final será convertida a audio, por lo que la ausencia de símbolos y formatos es fundamental para una reproducción de voz natural.
Evita cualquier tipo de formato especial, solo texto corrido.
"""

# Definir la pregunta
pregunta = "Cual es la diferencia entre concurrencia y paralelismo?"

# Hacer la consulta al modelo
respuesta = ollama.chat(
    model="qwen3:1.7b",
    messages=[
        {"role": "system", "content": ROL_QWEN},
        {"role": "user", "content": pregunta}
    ],
    think=False # Desactivar pensamiento interno para respuestas más rápidas.
)

# Mostrar respuesta en consola
print(respuesta["message"]["content"])