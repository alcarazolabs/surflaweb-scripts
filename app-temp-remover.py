import os
import shutil
import flet as ft


def main(page: ft.Page):
    page.title = "Limpiador de Archivos Temporales by @surflaweb | @alcarazolabs"
    page.vertical_alignment = ft.MainAxisAlignment.CENTER
    page.horizontal_alignment = ft.CrossAxisAlignment.CENTER

    page.window.width = 480
    page.window.height = 700
    page.window.min_width = 480  # Ancho mínimo permitido
    page.window.min_height = 700  # Alto mínimo permitido

    page.window.resizable = False
    page.padding = 20  # Espaciado interno


    # Obtener lista de carpetas de usuarios
    users_path = "C:/Users"
    try:
        user_folders = [f.name for f in os.scandir(users_path) if f.is_dir()]
    except Exception as e:
        user_folders = []
        print(f"Error al leer la carpeta Users: {e}")

    # ComboBox para seleccionar usuario
    user_dropdown = ft.Dropdown(
        width=300,
        options=[ft.dropdown.Option(user) for user in user_folders],
        label="Seleccione un usuario",
        autofocus=True,
    )

    # Texto de estado/resultado
    status_text = ft.Text("", color=ft.colors.RED)


    def delete_temp_files(e):
        selected_user = user_dropdown.value
        if not selected_user:
            status_text.value = "Por favor seleccione un usuario"
            status_text.color = ft.colors.RED
            page.update()
            return

        temp_path = f"C:/Users/{selected_user}/AppData/Local/Temp"
        #temp_path = f"D:/test"

        if not os.path.exists(temp_path):
            status_text.value = f"No se encontró la carpeta Temp para {selected_user}"
            status_text.color = ft.colors.RED
            page.update()
            return

        try:
            # Eliminar todo el contenido de la carpeta Temp
            for filename in os.listdir(temp_path):
                file_path = os.path.join(temp_path, filename)
                try:
                    if os.path.isfile(file_path) or os.path.islink(file_path):
                        os.unlink(file_path)
                    elif os.path.isdir(file_path):
                        shutil.rmtree(file_path)
                except Exception as e:
                    print(f"No se pudo eliminar {file_path}: {e}")

            status_text.value = f"Archivos temporales de {selected_user} eliminados con éxito"
            status_text.color = ft.colors.GREEN



        except Exception as e:
            status_text.value = f"Error al eliminar archivos: {str(e)}"
            status_text.color = ft.colors.RED

        page.update()

    # Botón para eliminar archivos
    delete_button = ft.ElevatedButton(
        "Eliminar archivos temporales",
        icon=ft.icons.DELETE_FOREVER,
        on_click=delete_temp_files,
        width=300,
        height=50,
    )

    # Diseño de la interfaz
    page.add(
        ft.Column(
            [
                ft.Icon(ft.icons.CLEANING_SERVICES, size=50),
                ft.Text("Limpiador de Archivos Temporales", size=20, weight=ft.FontWeight.BOLD),
                user_dropdown,
                delete_button,
                status_text,
            ],
            spacing=20,
            alignment=ft.MainAxisAlignment.CENTER,
            horizontal_alignment=ft.CrossAxisAlignment.CENTER,
        )
    )


if __name__ == "__main__":
    ft.app(target=main, view=ft.AppView.FLET_APP)