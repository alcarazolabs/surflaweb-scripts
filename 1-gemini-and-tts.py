from google import genai
from google.genai import types
import wave


# === Configuración ===
# 'TU_API_KEY' API real de Google Gemini
API_KEY = "XXXXXXXXXX"

client = genai.Client(api_key=API_KEY)

# Variable para
SAVED_AUDIO_PATH = "audios/output.wav"

# Define el comportamiento o rol de Gemini (opcional)

ROL_DE_GEMINI = """
Actúa como un experto en ciencias de la computación.
Tu respuesta debe ser como el texto de una página de un libro.
**Es crucial que la respuesta sea solo texto plano.**
**No uses negritas, cursivas, listas con asteriscos, guiones, ni ningún otro símbolo de formato Markdown.**
No uses encabezados como '###' o '**'.
No incluyas código ni ejemplos de código.
Solo proporciona información en oraciones completas y párrafos.
Sé lo más simple y directo posible.
La respuesta final será convertida a audio, por lo que la ausencia de símbolos y formatos es fundamental para una reproducción de voz natural.
Evita cualquier tipo de formato especial, solo texto corrido.
"""


# Set up the wave file to save the output:
def wave_file(filename, pcm, channels=1, rate=24000, sample_width=2):
   with wave.open(filename, "wb") as wf:
      wf.setnchannels(channels)
      wf.setsampwidth(sample_width)
      wf.setframerate(rate)
      wf.writeframes(pcm)

def generar_audio_desde_texto(texto):
    """
    Convierte texto a audio usando el modelo TTS de Gemini
    """
    try:
        response = client.models.generate_content(
            model="gemini-2.5-flash-preview-tts",
            contents=texto,
            config=types.GenerateContentConfig(
                response_modalities=["AUDIO"],
                speech_config=types.SpeechConfig(
                    voice_config=types.VoiceConfig(
                        prebuilt_voice_config=types.PrebuiltVoiceConfig(
                            voice_name='Kore',
                        )
                    )
                ),
            )
        )
        # Guardar el audio en un archivo .wav
        data = response.candidates[0].content.parts[0].inline_data.data

        wave_file(SAVED_AUDIO_PATH, data)  # Saves the file to current directory

        print(f"✓ Audio WAV generado correctamente!")

    except Exception as e:
        print(f"Error al generar audio: {e}")
        return None


def main():
    # Banco de preguntas
    pregunta = "¿Qué diferencia hay entre concurrencia y paralelismo?"

    print("--- Pregunta ---")
    print(pregunta)
    print("\n" + "=" * 50 + "\n")

    response = client.models.generate_content(
            model="gemini-2.5-flash",
            contents=pregunta,
            config=types.GenerateContentConfig(
            system_instruction=ROL_DE_GEMINI)
        )

    # === Mostrar la respuesta ===
    print("--- Respuesta de Gemini ---")
    print(response.text)
    print("\n" + "=" * 50 + "\n")

    # === Generar audio a partir de la respuesta ===
    print("Generando audio...")

    # Generar audio
    # Convertir texto a audio
    texto_final = pregunta+ " "+response.text
    generar_audio_desde_texto(texto_final)

    print(f"✓ Proceso completado!")

if __name__ == "__main__":
    main()
