#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <winsock2.h> // Cabecera principal de sockets en Windows
#include <ws2tcpip.h>  // Necesaria para la función inet_pton en Windows

// Indicamos al compilador que vincule la librería de sockets de Windows
#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Función para crear el socket TCP/IP
int crearSocket() {
    // si falla devuelve INVALID_SOCKET en lugar de -1
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        cerr << "Error al crear el socket" << endl;
    }
    return sockfd;
}

// Función para configurar la dirección del servidor
bool configurarDireccion(struct sockaddr_in &serv_addr, const char* ip, int puerto) {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(puerto);
    
    // Convierte la dirección IP de texto a formato binario
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        cerr << "Direccion invalida o no soportada" << endl;
        return false;
    }
    return true;
}

// Función para conectar con el servidor
bool conectarServidor(int sockfd, const struct sockaddr_in &serv_addr) {
    // En Windows, los errores de socket se evalúan contra SOCKET_ERROR
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        cerr << "Error al conectar" << endl;
        return false;
    }
    return true;
}

vector<float> recibirDatos(int sockfd) {
    vector<float> floats;
    float buffer;
    
    // Forzamos a que lea exactamente el tamaño de un float
    int bytesRecibidos;
    while ((bytesRecibidos = recv(sockfd, reinterpret_cast<char*>(&buffer), sizeof(buffer), 0)) > 0) {
        // CORRECCIÓN CRÍTICA: Solo guardamos si llegaron los 4 bytes completos
        if (bytesRecibidos == sizeof(float)) {
            floats.push_back(buffer);
        }
    }
    return floats;
}

// Función para guardar los datos en archivo
void guardarDatos(const vector<float>& datos, const string& nombreArchivo) {
    ofstream archivoSalida(nombreArchivo); 
    if (archivoSalida.is_open()) {
        for (float f : datos) {
            archivoSalida << f << "\n";
        }
        archivoSalida.close();
        cout << "Los datos se han guardado correctamente en: " << nombreArchivo << endl;
    } else {
        cerr << "Error al abrir el archivo para escritura." << endl;
    }
}

// Función para imprimir los resultados
void imprimirResultados(const vector<float>& datos) {
    cout << "Numeros de punto flotante recibidos:" << endl;
    for (float f : datos) {
        cout << f << endl; 
    }
}

int main() {
    // PASO OBLIGATORIO EN WINDOWS: Inicializar la DLL de Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error al inicializar Winsock" << endl;
        return 1;
    }

    // 1. Crear un socket TCP/IP
    int sockfd = crearSocket();
    if (sockfd == INVALID_SOCKET) {
        WSACleanup(); // Limpia Winsock antes de salir
        return 1;
    }

    // 2. Configurar la dirección y el puerto del servidor (Localhost: 12345)
    struct sockaddr_in serv_addr;
    if (!configurarDireccion(serv_addr, "127.0.0.1", 12345)) {
        closesocket(sockfd); // En Windows se usa closesocket() en vez de close()
        WSACleanup();
        return 1;
    }

    // 3. Conectar al servidor
    if (!conectarServidor(sockfd, serv_addr)) {
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    cout << "Conectado al servidor con exito." << endl;

    // 4. Recibir los datos binarios enviados por el servidor Python
    vector<float> datos_recibidos = recibirDatos(sockfd);

    // 5. Guardar los datos en el archivo de texto
    guardarDatos(datos_recibidos, "datos_temperatura.txt");

    // 6. Imprimir los números recibidos por consola
    imprimirResultados(datos_recibidos);

    // 7. Cerrar la conexión y descargar la librería de red de la memoria
    closesocket(sockfd);
    WSACleanup(); // Termina el uso de la biblioteca de sockets
    cout << "Conexion cerrada. Fin del programa." << endl;

    return 0;
}