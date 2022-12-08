fun calcularTendenciaVentas(ventas: List<Double>): String {
    // primero, obtenemos el número de elementos en la lista
    val n = ventas.size

    // luego, calculamos la media de las ventas
    val media = ventas.sum() / n

        // luego, calculamos el promedio de las diferencias entre cada venta y
    // la venta anterior
    var promedioDiferencias = 0.0
    for (i in 1 until n) {
        promedioDiferencias += ventas[i] - ventas[i - 1]
    }
    promedioDiferencias /= n

    println(promedioDiferencias)
    
    // finalmente, si el promedio de las diferencias es mayor que 0,
    // devolvemos un mensaje indicando que la tendencia es positiva
    if (promedioDiferencias > 0) {
        return "La tendencia es positiva"
    } else {
        // en otro caso, devolvemos un mensaje indicando que la tendencia es
        // negativa
        return "La tendencia es negativa"
    }
}



fun main() {
 
	val ventas = listOf(100.0, 90.0, 70.0, 80.0, 40.0, 80.0, 50.0, 90.0, 80.0, 50.0, 100.0,100.0,110.0, 101.0)
	println(calcularTendenciaVentas(ventas))  // imprime "La tendencia es positiva"

}