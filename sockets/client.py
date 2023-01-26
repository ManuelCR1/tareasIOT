import socket

def client():
    #host = socket.gethostname()
    port = 5000

    client_socket = socket.socket()
    client_socket.connect(('172.20.10.2', port))

    message = input(" -> ")

    while message.lower().strip() != 'adios':
        client_socket.send(message.encode())
        data = client_socket.recv(1024).decode()
        print('Server: ' + data)
        message = input(" -> ")

    client_socket.close()


if __name__ == '__main__':
    client()