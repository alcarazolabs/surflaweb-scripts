public function exportToPdf($id){
          
          //obtener comprador
          $buyer = Buyer::findOrFail($id);
          //Obtener productos que esta interesado
          $interestedProducts = $buyer->products()->orderBy('product_interested.id','desc')->get();
         //crear objeto pdf para mostrar número de página. Se habilito php para dompdf. "enable_php" => true en config/dompdf.php
          $pdf = app('dompdf.wrapper');

          //############ Permitir ver imagenes si falla ################################
          $contxt = stream_context_create([
            'ssl' => [
                'verify_peer' => FALSE,
                'verify_peer_name' => FALSE,
                'allow_self_signed' => TRUE,
            ]
        ]);

        $pdf = \PDF::setOptions(['isHTML5ParserEnabled' => true, 'isRemoteEnabled' => true]);
        $pdf->getDomPDF()->setHttpContext($contxt);
        //#################################################################################

          //Cargar vista/tabla html y enviar varibles con la data
          $pdf->loadView('buyer.pdfReportsViews.buyer', compact('buyer',
                                                                    'interestedProducts',
                                                                  'pdf')); 
           $fecha = date('Y-m-d');
            //descargar la vista en formato pdf 
            return $pdf->download("COMPRADOR-".strtoupper(utf8_decode($buyer->companyName))."-SIMBIOSIS-".$fecha.".pdf");
      }