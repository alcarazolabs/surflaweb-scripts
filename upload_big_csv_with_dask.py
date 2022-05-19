# Dataframes implement the Pandas API
import dask.dataframe as dd
import time


"""
dataframe = dd.read_csv("my_csv_file.csv", 
                        dtype={'DNI': 'str', 'AP_PAT': 'str', 'AP_MAT': 'str','NOMBRES': 'str'},
                        low_memory=False)
                        
"""
start_time = time.time()
dataframe = dd.read_csv("my_csv_file.csv",
                        low_memory=False,
                        dtype=str)
# dtype=str lee todo el dataframe diciendo que todo los tipos de datos de todas las columnas son strings.
# Si se desea especificar el tipo de dato de cada columna se debe de hacer para cada una como arriba linea 7.
# essto es util por que dask o pandas creara la columna en la bd en base a este tipo de dato.

print("dask.dataframe took %s seconds" %(time.time() - start_time))


# Enviar csv a la base de datos
start_time = time.time()
dataframe.to_sql('persons', uri='mysql://root:123456@localhost/myapidb', if_exists='replace', index=False)
print("Realizado en  %s segundos" %(time.time() - start_time))

# Dask creara la tabla primero y luego de unos minutos empezara a escribir los datos.


