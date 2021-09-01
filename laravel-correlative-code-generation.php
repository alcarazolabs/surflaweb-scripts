<?php 
 ############ GENERAR CODIGO CORRELATIVO
 ### se requiere una tabla con una columna llamada codigo - Tipo Varchar o String Laravel.
 // Generar codigo máximo
 $max_code = LogisticCost::select(
    DB::raw(' (IFNULL(MAX(RIGHT(code,7)),0)) AS number_max')
)->withTrashed()->first(); //se agrega withTrashed para obligar contar registros eliminados con softdeletes. Si no siempre retornara un codigo menor contando solo softdeletes null
$year = date('Y');
$month = date('m');
$day = date('d');
//Generar código correlativo
$code = 'GL'.$year.'-'.$month.'-'.$day.'-'.str_pad($max_code->number_max +1, 7, "0", STR_PAD_LEFT);

//ejemplo de salida:

# GL2021-09-01-0000001
# GL2021-09-01-0000002
?>