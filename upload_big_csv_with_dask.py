# Dataframes implement the Pandas API
import dask.dataframe as dd
import time

start_time = time.time()
dataframe = dd.read_csv("csv.csv", dtype={'myColumn1': 'str',
                                    'myColumn2': 'str',
                                    'myColumn_n': 'str'
                                    }) # Important to specify the datatype of your columns of your csv

print("dask.dataframe took %s seconds" %(time.time() - start_time))


# Enviar csv a la base de datos
start_time = time.time()
dataframe.to_sql('persons', uri='mysql://root:123456@localhost/apidb', if_exists='replace', index=False)
print("Realizado en  %s segundos" %(time.time() - start_time))

# Dask creara la tabla primero y luego de unos minutos empezara a escribir los datos.

# https://docs.dask.org/en/latest/generated/dask.dataframe.to_sql.html