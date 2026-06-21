import socket
import struct

# Dirección IP y puerto del servidor
HOST = '127.0.0.1'
PORT = 12345

# Array de 768 elementos limitados al rango de bytes para simular la cámara [cite: 93, 94]
data_array = [i % 256 for i in range(768)]

# Crear un socket TCP/IP
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
    # Enlazar el socket a la dirección y puerto especificados
    server_socket.bind((HOST, PORT))
    
    # Escuchar conexiones entrantes
    server_socket.listen()

    print(f'Servidor escuchando en {HOST}:{PORT}')
    
    # Aceptar la conexión entrante del cliente C++
    conn, addr = server_socket.accept()
    with conn:
        print('Conexión aceptada desde', addr)
        
        # Convertir los elementos de la lista a bytes (768 floats empaquetados)
        data_bytes = struct.pack('768f', *data_array)
        
        # Enviar los datos estructurados al cliente
        conn.sendall(data_bytes)
        print('Array de datos enviado al cliente.')