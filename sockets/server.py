import socket

def server():
    host = socket.gethostname()
    port = 5000

    server_socket = socket.socket()
    server_socket.bind((host, port))
    server_socket.listen(2)
    
    conn, address = server_socket.accept()
    print("Conexion: " + str(address))
    while True:
        data = conn.recv(1024).decode()
        if not data:
            break
        print("User: " + str(data))
        data = input(' -> ')
        conn.send(data.encode())

    conn.close()

if __name__ == '__main__':
    server()