from flask import Flask
from flask import request
from flask import jsonify
import uuid
import os
app = Flask(__name__)

UPLOAD_FOLDER = "static/images"
if not os.path.exists(UPLOAD_FOLDER):
    os.makedirs(UPLOAD_FOLDER)


@app.route('/upload', methods=['POST'])
def get_news():
    if 'image' in request.files:
        imagen = request.files['image']
        if imagen.filename != '':
            # Generar un ID único para la imagen
            imagen_id = str(uuid.uuid4())
            # Construir la ruta del archivo con el ID único
            imagen_filename = os.path.join(UPLOAD_FOLDER, imagen_id + '.jpg')
            imagen.save(imagen_filename)


    return jsonify({'status': 'ok'})


if __name__ == "__main__":

	app.run(host='192.168.18.42', port=5000)