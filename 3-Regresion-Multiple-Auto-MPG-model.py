# -*- coding: utf-8 -*-
"""
@author: (surflaweb)
video: https://youtu.be/mQHZYLRN8z8
"""
import pandas as pd
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers

# 2.1.0
print(tf.__version__)
# https://archive.ics.uci.edu/ml/datasets/auto+mpg
# Descargar dataset Auto MPG.
dataset_path = keras.utils.get_file("auto-mpg.data", "http://archive.ics.uci.edu/ml/machine-learning-databases/auto-mpg/auto-mpg.data")
#Imprimir ruta del dataset.
dataset_path

# Crear lista de nombres de las columnas del dataset.
column_names = ['MPG','Cylinders','Displacement','Horsepower','Weight',
                'Acceleration', 'Model Year', 'Origin']

# La primera columna (MPG) corresponde al ground truth (Millas por galón).
# Poner nombres de columnas al dataset
raw_dataset = pd.read_csv(dataset_path, names=column_names,
                      na_values = "?", comment='\t',
                      sep=" ", skipinitialspace=True)
# copiar el dataset original a la variable dataset.
dataset = raw_dataset.copy()
# mostrar las 5 ultimas filas
dataset.tail()

# contar cuantos valores nan existen en las columnas.
dataset.isna().sum()
# borrar valores nan y fila completa.
dataset = dataset.dropna()

# obtener y quitar columna 'Origin' de la variable 'dataset' la cual esta en categorias (columna categorica).
# 0 = USA, 1=Europa y 2=Japon
origin = dataset.pop('Origin')
# One-Hot encode. Agregar columnas USA, Europe y Japan, Se agrega 1 si corresponde a su categoria ó 0 si no.
dataset['USA'] = (origin == 1)*1.0
dataset['Europe'] = (origin == 2)*1.0
dataset['Japan'] = (origin == 3)*1.0

# mostrar las ultimas 5 filas del dataset.
dataset.tail()

# dividir dataset en 80% entrenamiento y para pruebas 20%.
train_dataset = dataset.sample(frac=0.8,random_state=0)
test_dataset = dataset.drop(train_dataset.index)

import seaborn as sns
sns.pairplot(train_dataset[["MPG", "Cylinders", "Displacement", "Weight"]], diag_kind="kde")

train_stats = train_dataset.describe()
# La función describe () calcula un resumen de estadísticas pertenecientes a las columnas del DataFrame.
# Mostrar estadisticas generales del dataframe train_stats.
train_stats

#obtener las ground truth del dataset train_dataset y test_dataset quitando la columna de MPG en esas variables.
train_labels = train_dataset.pop('MPG')
test_labels = test_dataset.pop('MPG')

# Reflejar el DataFrame 'train_stats' escribiendo filas como columnas y viceversa.
# (Aqui debemos de obtener la mean y std para llevarlas a android)
train_stats.pop("MPG")
train_stats = train_stats.transpose()
train_stats

# Normalizar el dataset para entrenar y pruebas:
# se crean dos variables: normed_train_data y normed_test_data 
# estas variables se convertiran en el dataset final para entrenar y pruebas.

"""
 Es una buena práctica normalizar funciones que utilizan diferentes escalas y rangos.
 Aunque el modelo * podría * converger sin normalización de características,
 dificulta el entrenamiento y hace que el modelo resultante dependa de la elección
 de las unidades utilizadas en la entrada.

 estas estadísticas también se utilizarán para normalizar el conjunto de datos 
 de prueba. Necesitamos hacer eso para proyectar el conjunto de datos de prueba 
 en la misma distribución en la que el modelo ha sido entrenado.
"""

def norm(x):
  return (x - train_stats['mean']) / train_stats['std']

normed_train_data = norm(train_dataset)
normed_test_data = norm(test_dataset)

"""
 Precaución: las estadísticas utilizadas para normalizar las entradas aquí
 (media y desviación estándar) deben aplicarse a cualquier otro dato que
 se alimente al modelo, junto con la codificación de un punto que hicimos
 anteriormente. Eso incluye el conjunto de pruebas, así como los datos en
 vivo cuando el modelo se usa en producción.
"""


# Crear el modelo:
# se utiliza un modelo secuencial con dos capas ocultas densamente conectadas
# y una capa de salida que devuelve un único valor continuo.

def build_model():
  model = keras.Sequential([
    layers.Dense(64, activation='relu', input_shape=[len(train_dataset.keys())]),
    layers.Dense(64, activation='relu'),
    layers.Dense(1)
  ])

#(*) Nota: el input_shape tiene el tamaño columnas del train_dataset 8.
  optimizer = tf.keras.optimizers.RMSprop(0.001) #root mean square ( valor cuadrático medio)
  """
  RMSprop es un optimizador bueno, rápido y muy popular. So popularidad solo es superada por Adam (Andrej Karpathy).
  """
  model.compile(loss='mse',
                optimizer=optimizer,
                metrics=['mae', 'mse'])
  return model

model = build_model()

model.summary()

# Epocas para entrenar el modelo.
EPOCHS = 1000

# entrenar modelo.
history = model.fit(normed_train_data, train_labels, epochs=EPOCHS)


# evaluar modelo
loss, mae, mse = model.evaluate(normed_test_data, test_labels, verbose=0)
print("Test_loss: "+str(loss))
print("Test mae:"+str(mae))
print("Test mse:"+str(mse))

# realizar predicciones en el 20% de datos para pruebas.
# (Comparar con test_labels la cual tiene la verdad fundamental)

test_predictions = model.predict(normed_test_data).flatten()

#Exportar el modelo
kearas_file = "automobile.h5"
tf.keras.models.save_model(model,kearas_file)
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tfmodel = converter.convert()
open("automobile.tflite","wb").write(tfmodel)