# MANUAL DE DOCUMENTACION - PRACTICA 3

## Descripcion General del Proyecto
Este proyecto corresponde a la Practica 3 de la asignatura Sistemas Operativos. El objetivo principal es la implementacion y el analisis de un entorno de comunicanion de red basado en una arquitectura cliente-servidor mediante el protocolo de transporte TCP/IP. El sistema simula la captura de datos procedentes de una camara termica, donde un servidor central distribuye lecturas de temperatura en forma de una matriz bidimensional y un cliente programado en C++ se encarga de realizar la conexion de red, recibir el flujo de bytes, validar la integridad del contenido y procesar los resultados finales.

## Arquitectura del Sistema
La arquitectura del entorno consta de dos componentes independientes distribuidos que se comunican de forma asincrona:

* Servidor de la Camara: Un script desarrollado en Python que actua como emisor fisico simulado. Escucha las conexiones entrantes en el puerto local de red y, al detectar al cliente, transmite en flujo directo de red un conjunto cerrado de valores correspondientes a los pixeles termicos.
* Cliente de Recepcion: Un programa desarrollado en C++ adaptado para sistemas operativos Windows mediante la interfaz de programacion de aplicaciones Winsock. Este componente solicita la conexion al servidor, gestiona la recepcion de los fragmentos de red a nivel de buffer de memoria, almacena la secuencia numerica recibida y escribe la informacion final en un archivo local.

## Especificaciones de la Transmision de Datos
La camara termica simulada posee una resolucion geometrica de 32 columnas por 24 filas. El volumen total de lecturas requeridas equivale exactamente a la multiplicacion de estas dimensiones, resultando en un limite estricto de 768 datos numericos de tipo flotante por cada captura completa. 

Total = 32 columnas x 24 filas = 768 lecturas totales

A nivel de transmision, cada dato numerico individual de precision flotante ocupa exactamente 4 bytes en memoria fisica. Por consiguiente, la sesion de red transmite un total neto de 3072 bytes. El servidor envia esta secuencia simulando una progresion numerica modular que oscila secuencialmente de cero a 255 y se repite tres veces sucesivas de manera lineal hasta completar la matriz fisica de la camara.

## Archivos Componentes del Repositorio
El espacio de trabajo consta unicamente de los siguientes archivos esenciales para su despliegue y evaluacion:

* ServidorCamara.py: Contiene el codigo fuente del servidor encargado de gestionar el puerto, abrir el canal de escucha TCP e inyectar el flujo numerico por el socket abierto.
* cliente.cpp: Codigo fuente del cliente de red desarrollado en lenguaje C++ con las llamadas al subsistema Winsock2 de Windows.
* bash.sh: Script de comandos para interpretes de comandos POSIX que automatiza de forma integrada la construccion, ejecucion secuencial y finalizacion segura de los procesos involucrados.
* datos_temperatura.txt: Archivo de texto plano autogenerado por el ejecutable tras una transmision exitosa, el cual contiene la persistencia fisica de los 768 valores organizados por columnas.
* .gitignore: Archivo de configuracion del control de versiones diseñado para excluir los archivos objeto temporales y los binarios compilados de la plataforma de desarrollo.

## Estrategia de Sincronizacion y Correccion en Windows
Durante la migracion del software hacia el entorno Windows, se identificaron desafios en el manejo del flujo continuo de datos a traves de sockets debido a las diferencias en la alineacion de los limites de buffer de red entre sistemas operativos de tipo Unix y Windows. La llamada estandar de lectura en sockets puede retornar fragmentos parciales de bytes en canales de tipo stream.

Para solucionar de raiz la lectura de datos corruptos o desalineaciones en la memoria, se implemento una estrategia de validacion por tamaño en el bloque cliente de recepcion. El bucle de captura inspecciona los bytes retornados por cada llamada de red y unicamente añade el elemento a la estructura logica cuando los bytes leidos en el canal igualan de manera exacta al tamaño fisico del tipo de dato float en el compilador. Esta comprobacion matematica garantiza que el flujo de datos finalice limpiamente en la medicion numero 768, rompiendo el bucle de transmision y cerrando los sockets de red sin perdida de informacion.

## Instrucciones de Despliegue y Ejecucion
Para inicializar y comprobar el funcionamiento del entorno completo de pruebas en una sesion activa de Git Bash en Windows, se debe ejecutar el archivo automatizado proporcionado. El comando de terminal es el siguiente:

./bash.sh

Este script interno realiza de forma automatica las siguientes acciones del sistema:

1. Compila el archivo fuente de C++ invocando el enlazador de librerias del sistema de sockets mediante la instruccion de vinculacion de la biblioteca ws2_32.
2. Inicializa de forma asincrona en segundo plano el interprete de Python asignado al archivo del servidor.
3. Ejecuta el archivo binario cliente generado, forzando la apertura de la conexion a la direccion local interna 127.0.0.1 a traves del puerto de red parametrizado.
4. Muestra por el canal de salida de texto el listado numerico de control comprendido entre cero y 255.
5. Registra la finalizacion de la transferencia mediante el volcado hacia el archivo de texto plano local y procede a la destruccion y liberacion del puerto de comunicaciones del sistema operativo.
