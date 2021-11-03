<?php 
//https://www.youtube.com/watch?v=dRroOoQRVIs&t=3163s
//Creditos: Sergio Sianuco Leon
require 'vendor/autoload.php';
//crear event-loop
$loop = \React\EventLoop\Factory::create();


//ejecutar algo cada 1 sg
$loop->addPeriodicTimer(5, function() use ($loop){
 //conectar a la base de datos
 //Get a new connection to query jobs
 //utilizar factory MySQL (para eso se instalo react/mysql)
 $factory = new \React\MySQL\Factory($loop);
 $uri= 'root:123456@localhost/test_db?timeout=0.5';
 $connectionJob = $factory->createLazyConnection($uri);

 /*
 //consulta de prueba
 $connectionJob->query('SELECT * FROM employees')->then(
    function (\React\MySQL\QueryResult $command) {
        echo count($command->resultRows) . ' row(s) in set' . PHP_EOL;
    },
    function (Exception $error) {
        echo 'Error: ' . $error->getMessage() . PHP_EOL;
    }
);
*/
  //Get unreserved jobs
  $connectionJob->query('SELECT * FROM reports WHERE created_at IS NULL LIMIT 1')->then( //query-1
    
        function(\React\MySQL\QueryResult $command) use($connectionJob, $loop, $factory){
            //Do nothing if there is no jobs
            if(count($command->resultRows) === 0){
                //'no hay jobs';
                return;
            }
            //Get job
            $resultRow = $command->resultRows[0];

            //Reserve job
            $connectionJob
                ->query('UPDATE reports SET created_at = '.\Carbon\Carbon::now()->getTimestamp().' WHERE id ='.$resultRow['id'])
                ->then(function() use($connectionJob, $resultRow, $loop){ //then 1
                   
		     $tableName = $resultRow['table_name'];
		    
                    //Get a new connection to query report
                    $factory = new \React\MySQL\Factory($loop);
                    $uri = 'root:123456@localhost/test_db?timeout=0.5';
                    $connectionReport = $factory->createLazyConnection($uri);
 		    
 		    
                    //Perform an async query and stream the rows of the result set
        
                    $stream = $connectionReport->queryStream('SELECT * FROM '. $tableName);

		     echo "Inicio informe ".$tableName. PHP_EOL;
		     
                    //Create a stream for file
                    $fileName = tempnam(__DIR__, "report_".$tableName."_");
                    //agregar extension
                    rename($fileName, $fileName .= '.csv');
                    
                    echo 'filename: '.$fileName. PHP_EOL;;
                    $fileStream = new \React\Stream\WritableResourceStream(fopen($fileName, 'w'), $loop, 1);

                    //Create a Stream to transform ResultSet to CSV line
                    $transformStream = new \React\Stream\ThroughStream(function($data){
                    
                        return implode(',', array_values($data)).PHP_EOL;
                    });
                     
                    //write to file
                    $stream->pipe($transformStream)->pipe($fileStream); //conectar streams el de bd y el del archivo
		     $command = $tableName;
		     
                    //When stream ends, close file, close connections
                    $stream->on('end', function() use ($connectionJob, $connectionReport, $fileStream, $resultRow, $tableName) {
                        
                        echo "Fin informe ".$tableName. PHP_EOL;
                        //close file
                        $fileStream->end();
                        //close connection to query report
                        $connectionReport->quit();
                        //delete job
                        $connectionJob
                            ->query("DELETE FROM `test_db`.`reports` WHERE (`id` ='{$resultRow['id']}')")
                            ->then(
                                function() use($connectionJob){
                                //close connection to query jobs
                                $connectionJob->quit();
                            },
                                function (Exception $error){
                                echo 'Error '.$error->getMessage(). PHP_EOL;
                            });
                    });

                
                },   function (Exception $error){ //}); //end then 1
                    echo 'Error '.$error->getMessage(). PHP_EOL;
                });
        },
        function (Exception $error) {
        echo 'Error: ' . $error->getMessage() . PHP_EOL;
    	}
       
  ); //end-query-1
 
});

$loop->run();



?>
