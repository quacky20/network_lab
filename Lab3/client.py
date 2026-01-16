import socket
import threading
import sys

HOST = "127.0.0.1"
PORT = 5555


def receive_messages(sock):
    while True:
        try:
            message = sock.recv(1024).decode()
            if not message:
                break
            print(message, end="")
        except:
            break


def start_client(name):
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((HOST, PORT))

    if client.recv(1024).decode() == "NAME":
        client.sendall(name.encode())

    thread = threading.Thread(
        target=receive_messages,
        args=(client,),
        daemon=True
    )
    thread.start()

    while True:
        msg = input()
        if msg.upper() == "EXIT":
            client.sendall(b"EXIT")
            break
        client.sendall(msg.encode())

    client.close()


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python client.py <Name>")
        sys.exit(1)

    start_client(sys.argv[1])

