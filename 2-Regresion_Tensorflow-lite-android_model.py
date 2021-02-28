# -*- coding: utf-8 -*-
"""
@author: Freddy Alc (@surflaweb)
"""
import pandas as pd
import tensorflow as tf
from tensorflow import keras
from tensorflow import lite

#Ecuación ordinaria de la recta:
# Y = m * X + b (m la pendiente de la linea y b es el intercepto/intersección)
df = pd.read_csv('dataset/cells.csv')

x_df = df.drop('cells', axis='columns')
y_df = df.cells

model = keras.Sequential([keras.layers.Dense(units=1,input_shape=[1])])
metric = ['accuracy']
model.compile(optimizer='sgd',loss='mean_squared_error', metrics=metric)


model.fit(x_df,y_df,epochs= 1000)

test_loss, test_accuracy = model.evaluate(x_df, y_df)
print(test_loss)
print(test_accuracy)

print(model.predict([1.6]))


kearas_file = "linear.h5"
tf.keras.models.save_model(model,kearas_file)
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tfmodel = converter.convert()
open("linear.tflite","wb").write(tfmodel)


