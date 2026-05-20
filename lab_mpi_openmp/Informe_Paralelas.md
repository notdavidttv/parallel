# Informe de Computación Paralela: MPI y OpenMP

**Estudiantes:**
* Kevin Parada
* Orlando Hernandez

---

## Ejercicio 1: Hola Mundo MPI

**Descripción:** En este ejercicio cada proceso imprimirá su id y el total de procesos, esto es posible al usar rank y size.

**Ejecución usando 4 procesos:**

![Ejecución con 4 procesos](lab_mpi_openmp/images/01.jpeg)

**Ejecución con 2 procesos:**

![Ejecución con 2 procesos](lab_mpi_openmp/images/02.jpeg)


### Preguntas de análisis:

* **¿Por qué el orden de salida varía entre ejecuciones?**
  A MPI no le importa el número de rank para imprimir, él simplemente le da el paso al proceso que va llegando de primero, entonces eso determina el orden de impresión.
* **¿Qué pasaría si ejecutas con -n 1? ¿Tiene sentido paralelizar así?**
  Pues solo un proceso de rank 0 ejecutaría el programa, así que esto no tendría sentido paralelizarlo así, ya que no va a ver división de trabajo, es decir, sería como ejecutar un programa secuencial, pero con un gasto mayor en recursos.
* **¿Para qué sirve MPI_COMM_WORLD? ¿Podría haber otros comunicadores?**
  MPI_COMM_WORLD es como un canal que permite la comunicación entre procesos. Sí se puede haber otros comunicadores, ya que MPI_COMM_WORLD es el comunicador global que agrupa a todos los procesos, pero a veces se necesita un comunicador que solo agrupe un pequeño grupo de procesos de los demás, para realizar tareas aisladas.

---

## Ejercicio 2: OpenMP dentro de MPI

**Descripción:** En este ejercicio hicimos una combinación de MPI con OpenMP. Con MPI gestionamos la comunicación entre los dos procesos creados y dentro de cada proceso usamos OpenMP para asignarles 4 hilos lógicos que comparten la memoria del proceso.

![Ejecución híbrida MPI + OpenMP](lab_mpi_openmp/images/03.jpeg)


### Preguntas de análisis:

* **Con 2 procesos MPI y 4 hilos OMP, ¿cuántas unidades de cómputo hay en total?**
  Habrían 8 unidades de cómputo ya que cada proceso tiene a su disposición 4 hilos.
* **¿En qué se diferencia ejecutar con -n 4 (4 MPI, 4 hilos) vs -n 1 (1 MPI, 16 hilos)?**
  Se diferencia en el rendimiento, ya que con -n 4 (4 MPI, 4 hilos) también hay 16 unidades de cómputo pero por cada proceso tiene su propia memoria lo cual es más rentable que -n 1 (1 MPI, 16 hilos) en donde al solo ser un proceso, los 16 hilos van a compartir la misma memoria y esto se traduce en más tiempo de ejecución.
* **¿Por qué es importante MPI_Init_thread en lugar de MPI_Init cuando usamos OpenMP?**
  Es importante usar MPI_Init_thread en lugar de MPI_Init cuando se trabaja con OpenMP, porque permite que MPI soporte la ejecución con múltiples hilos dentro de un mismo proceso.

---

## Ejercicio 3: Suma híbrida de vector

**Descripción:** En este ejercicio se hizo la sumatoria de un arreglo usando el modelo híbrido de MPI con OpenMP, dividiendo los valores del vector entre los procesos, para que así cada proceso realice su suma local de forma paralela con hilos usando OpenMP.

![Resultado de la suma híbrida](lab_mpi_openmp/images/04.jpeg)


### Preguntas de análisis:

* **¿Qué hace exactamente MPI_Scatter? ¿Quién envía y quién recibe?**
  MPI_Scatter es el encargado de distribuir el trabajo a los procesos, es decir, se encarga de decirle a los procesos “esta es tu parte del trabajo”. Quien envía es el proceso root (Siempre usamos el proceso 0) quien es el que inicializa el arreglo, y todos los procesos incluido el 0 reciben su parte.
* **¿Por qué usamos reduction(+:suma_local) y no una variable compartida directamente?**
  Usamos reduction(+:suma_local) para que no haya condición de carrera entre los hilos, es decir, para que los hilos no intenten actualizar la variable al tiempo ya que esto puede dar problemas.
* **¿Qué pasaría si olvidaras MPI_Reduce y solo imprimieras suma_local en rank==0?**
  En esta condición, solo se imprimirá la suma local calculada por el proceso 0.

---

## Ejercicio 4: Speedup Híbrido

### Tabla de resultados:

| Configuración | Procesos MPI | Hilos OMP | Speedup obtenido |
| :--- | :---: | :---: | :---: |
| Solo MPI, sin OMP | 4 | 1 | 0.14x |
| Solo OMP, sin MPI | 1 | 4 | 0.15x |
| MPI + OMP | 2 | 2 | 0.12x |
| MPI + OMP | 4 | 2 | 0.12x |

### Evidencia de ejecuciones:

**Solo MPI, sin OMP:**
![Ejecución solo MPI](lab_mpi_openmp/images/05.jpeg)


**Solo OMP, sin MPI:**
![Ejecución solo OMP](lab_mpi_openmp/images/06.jpeg)


**MPI + OMP (2 procesos, 2 hilos):**
![Ejecución MPI+OMP 2x2](lab_mpi_openmp/images/07.jpeg)


**MPI + OMP (4 procesos, 2 hilos):**
![Ejecución MPI+OMP 4x2](lab_mpi_openmp/images/08.jpeg)


### Discusión de eficiencia:
Para nuestro caso, las configuraciones dieron mejoras muy similares, debido a que los datos que se procesaron son pequeños, y según nuestros resultados el mejor tiempo fue el de solo OMP sin MPI ya que se redujo el overhead por MPI, y como los datos son pocos, entonces es más eficiente.

### Preguntas de análisis:

* **¿Coincide el speedup con lo que predice la Ley de Amdahl? (parte paralela ≈ 100%)**
  Sí coincide. La ley dice que el speedup siempre va a estar limitado por la parte secuencial del programa. En las pruebas el speedup fue bastante bajo y eso confirma lo que dice la ley. Aunque la operación de suma sí se puede paralelizar, hay partes que siguen siendo secuenciales, como la inicialización del vector en el proceso 0 y el tiempo que se pierde en la comunicación entre procesos. Como el problema era pequeño, esos tiempos terminaron afectando más el rendimiento total.
* **¿Por qué más procesos/hilos no siempre significan mayor speedup?**
  Porque esto significa más overhead, y para un trabajo pequeño, resulta ser más costoso. Por otra parte, está el límite regido a la parte secuencial inevitable del programa, que hace que, aunque aumentemos infinitos procesos o hilos, el beneficio sea bajo o nulo.
* **¿Qué overhead introduce MPI que no existe en OpenMP puro?**
  Sincronizar procesos e intercambio de datos entre procesos es el costo que tiene MPI que no hay en OMP.

---

## Conclusiones

* Comprendimos la principal diferencia entre MPI y OpenMP que es la memoria. Mientras que con MPI cada proceso tiene su propia memoria, en OpenMP los hilos comparten la misma memoria.
* Logramos aprender y aplicar la arquitectura híbrida usando MPI + OpenMP. Esta combinación permite resolver problemas de forma eficiente, ya que MPI se encarga de dividir el problema entre varios procesos, mientras que OpenMP hace que cada proceso pueda trabajar con varios hilos al mismo tiempo.
* Trabajar con procesos y, dentro de cada uno, con grupos de hilos es mucho mejor que usar solamente MPI o solamente OpenMP, porque se distribuye mejor el trabajo y se logra un mayor aprovechamiento del paralelismo.
* Finalmente, fue vital entender que no siempre resulta ser provechoso este paralelismo, ya que si el problema no es lo suficientemente grande como para que sea necesario el gasto adicional de overhead por paralelismo, entonces es mejor resolverlo secuencial.
