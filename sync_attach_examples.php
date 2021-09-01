<?php 

######### REGISTER A CHECKBOX ARRAY TO A PIVOT TABLE WITH EXTRA FIELDS:
# Table columns: id	
#quality_id	
#product_id	
#buyer_id	
#color	 <- extra field
#humidity	<- extra field

// Code:     

$arrayQualitiesIds = $request->get('qualitiesProductCheckbox');
         
//apilar en un array_map los valores de las llaves foraneas de la tabla pivot
//incluido el buyer_id, a pesar de que este tiene la relación 'qualities' no registro
//automaticamente el id del buyer si no que se tuvo que pasar en el array_map:
$extra = array_map(function($qualityId) use($request, $buyer){
    return ['quality_id' => $qualityId,
             'product_id' => $request->product,
             'buyer_id' => $buyer->id,
             'color' => $request->color,
             'humidity' => $request->humidity];
}, $arrayQualitiesIds);
//combibar arreglos
$data = array_combine($arrayQualitiesIds, $extra);

$buyer->qualities()->sync($data); //sync -> update, write or delete
//or you can use attach:
$buyer->qualities()->attach($data); //attach you can append a lot of data without delete/update


########### REGISTER ARRAY USING ATTACH TO A PIVOT TABLE:
// Table pivot columns: 

#id	
#logistic_cost_id	
#expense_type_id	
#quantity <- extra field
#price <- extra field
$logisticCost = LogisticCost::findOrFail($logisticCostId);
// build array        
$extra = array(
    'expense_type_id' => $request->expenseType,
    'quantity' => $request->quantity,
    'price' => $request->price
);  
//dd($extra);   
//Registrar detalle en la tabla pivot 'expense_logistic' usando la relación:
$logisticCost->expenses()->attach($logisticCost->id, $extra);



?>