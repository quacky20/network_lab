import socket
import threading
from datetime import datetime

HOST = "127.0.0.1"
PORT = 5555
LOG_FILE = "server_logs.txt"

clients = {} 
message_count = {}   
lock = threading.Lock()


def log_to_file(message: str):
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    with open(LOG_FILE, "a") as f:
        f.write(f"[{timestamp}] {message}\n")


def broadcast(message: str, exclude_socket=None):
    with lock:
        for client_socket in clients:
            if client_socket != exclude_socket:
                try:
                    client_socket.sendall(message.encode())
                except:
                    pass


def private_message(sender, target_name, message):
    with lock:
        for sock, name in clients.items():
            if name == target_name:
                sock.sendall(message.encode())
                return True
    return False


def handle_client(client_socket, address):
    try:
        client_socket.sendall(b"NAME")
        name = client_socket.recv(1024).decode().strip()

        with lock:
            clients[client_socket] = name
            message_count[name] = 0

        join_msg = f"{name} joined the chat."
        log_to_file(join_msg)
        print(f"[{current_time()}] <Server>: {join_msg}")
        broadcast(f"[{current_time()}] <Server>: {join_msg}\n")

        while True:
            data = client_socket.recv(1024)
            if not data:
                break

            message = data.decode().strip()

            if message.upper() == "EXIT":
                break

            elif message.upper() == "LIST":
                with lock:
                    names = ", ".join(clients.values())
                client_socket.sendall(
                    f"[{current_time()}] <Server>: Connected clients: {names}\n".encode()
                )

            elif message.startswith("@"):
                try:
                    target, msg = message[1:].split(" ", 1)
                    pm = f"[{current_time()}] <Private from {name}>: {msg}\n"
                    if not private_message(name, target, pm):
                        client_socket.sendall(
                            f"[{current_time()}] <Server>: User not found.\n".encode()
                        )
                except ValueError:
                    client_socket.sendall(
                        f"[{current_time()}] <Server>: Invalid private message format.\n".encode()
                    )

            else:
                message_count[name] += 1
                formatted = f"[{current_time()}] <{name}>: {message}\n"
                log_to_file(f"{name}: {message}")
                print(formatted.strip())
                broadcast(formatted, client_socket)

    except ConnectionResetError:
        pass
    finally:
        disconnect_client(client_socket)


def disconnect_client(client_socket):
    with lock:
        name = clients.pop(client_socket, None)

    if name:
        exit_msg = f"{name} left the chat."
        log_to_file(exit_msg)
        print(f"[{current_time()}] <Server>: {exit_msg}")
        broadcast(f"[{current_time()}] <Server>: {exit_msg}\n")
        print(f"[STATS] {name} sent {message_count[name]} messages.")

    client_socket.close()


def current_time():
    return datetime.now().strftime("%H:%M:%S")


def start_server():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen()
    print(f"Server running on {HOST}:{PORT}")

    while True:
        client_socket, address = server.accept()
        threading.Thread(
            target=handle_client,
            args=(client_socket, address),
            daemon=True
        ).start()


if __name__ == "__main__":
    start_server()

