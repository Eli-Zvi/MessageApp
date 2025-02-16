"""
Server file - represents the server and its "database" of clients and messages that is saved in memory
sends messages to clients
@author <Ilay Zvi>
"""
import functools
import socket
from client import Client
from messages import Message
from request import *
from response import *
import threading
from exceptions import ProtocolError


def lock(func):  # thread safety
    """
    lock wrapper
    """

    @functools.wraps(func)
    def wrapper(self, *args, **kwargs):
        with self._lock:
            return func(self, *args, **kwargs)

    return wrapper


def get_port():
    """
    attempts to read SERVER_INFO_PATH and returns the port stored in the file
    otherwise returns default port 1357
    """
    try:
        f = open(SERVER_INFO_PATH, "r")
        return int(f.readline().strip())
    except:
        return DEFAULT_PORT


class Server:
    """
    The server has 3 responsibilities
    1. it accepts users and initializes a new thread to communicate with them
    2. it creates a response by reading through the request object that has been created
    3. it sends messages to the client
    """
    clients = {}
    messages = {}

    def __init__(self, version):
        self._lock = threading.Lock()  # create a lock for safe multi threading
        self.version = version

        self.PORT = get_port()

        # Create Socket
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Bind socket to address and port
        self.server_socket.bind((LOCAL_HOST, self.PORT))

        self.server_socket.listen()
        print(f"Server listening on {LOCAL_HOST}:{self.PORT}")
        self.run()

    def run(self):
        """
        accepts new clients and creates a new thread to communicate with them
        """
        while True:
            client_socket, client_address = self.server_socket.accept()
            # New thread to handle client
            threading.Thread(target=self.handle_client, args=(client_socket,)).start()

    def handle_client(self, client_socket):
        """
        client loop, calls parsing request function and response creation function and finally sends the response to
        the client if an exception is triggered at any point the connection will be closed with the client to prevent
        undefined behavior
        """
        print("Client connected")
        try:
            while True:
                try:
                    request = Request.parse_request(client_socket)
                    response = self.create_response(request)
                    client_socket.sendall(response)
                except struct.error as e:
                    print("Error packing or unpacking data:")
                    print(e)
                    client_socket.sendall(Response.general_error_response())
                    break
                except ProtocolError as e:
                    print(e)
                    client_socket.sendall(Response.general_error_response())
                    break  # will terminate connection after protocol error has occurred
                except ConnectionResetError:
                    break
                except ValueError as e:
                    print(e)
                    client_socket.sendall(Response.general_error_response())
                    break
                except Exception as e:
                    print(e)
                    client_socket.sendall(Response.general_error_response())
                    break
        finally:
            client_socket.close()
            print("Client has disconnected")

    @lock
    def add_client(self, client: Client):
        """
        obtains lock and adds a new client to client dict
        """
        self.clients[client.get_user_uuid()] = client  # check if its already in the client list

    @lock
    def remove_client(self, client_uuid: uuid.UUID):
        """
        obtains lock and removes a client from client dict
        """
        self.clients.pop(client_uuid)
        self.messages.pop(client_uuid)

    @lock
    def add_message(self, to_client: uuid.UUID, from_client: uuid.UUID, message_type: MessageType, content_size,
                    content):
        """
            obtains lock and adds a new message to message dict
            """
        message = Message(to_client, from_client, message_type, content_size, content)
        if to_client not in self.messages:
            self.messages[to_client] = {}

        self.messages[to_client][message.get_message_id()] = message
        return message

    @lock
    def remove_messages(self, client_uuid: uuid.UUID, message_id_dict: dict):
        """
        obtains lock and removes the messages stored in message_id_dict if they exist
        """
        for message_id in message_id_dict:
            try:
                self.messages[client_uuid].pop(message_id)
            except KeyError:
                pass

    @lock
    def get_awaiting_messages(self, client_uuid: uuid.UUID):
        """
        obtains lock and sends the list of awaiting messages for client_uuid if it exists otherwise returns empty dict
        """
        try:
            return self.messages[client_uuid]
        except KeyError:
            return {}

    @lock
    def is_username_in_clients(self, username):
        """
        Returns true if the username is in the list of clients false otherwise
        """
        for client in self.clients.values():
            if username == client.get_username():
                return True

        return False

    def create_response(self, request):
        """
        crafts a response that can be sent back to the client
        """
        if request.get_code() == RequestCode.REGISTRATION_REQUEST:
            username, public_key = request.get_payload()

            if self.is_username_in_clients(username):  # if the client already exists return error
                return Response.general_error_response()

            client = Client(uuid.uuid4().bytes, username, public_key)  # create new client with a new uuid
            self.add_client(client)  # add to list of clients

            return Response.registration_response(client.get_user_uuid())

        elif request.get_code() == RequestCode.CLIENT_LIST_REQUEST:
            client_list = b""

            for client_uuid, client in self.clients.items():
                if client.get_user_uuid() != request.get_client_uuid():  # skip the user that sent the request
                    client_list += struct.pack(f"<{UUID_LENGTH}s{USERNAME_LENGTH}s",
                                               client.get_user_uuid(), client.get_username())

            return Response.client_list_response(client_list)

        elif request.get_code() == RequestCode.PUBLIC_KEY_REQUEST:
            client_uuid = request.get_payload()
            try:
                client = self.clients[client_uuid]
            except KeyError:
                return Response.general_error_response()

            return Response.public_key_response(struct.pack(f"<{UUID_LENGTH}s{PUBLIC_KEY_LENGTH}s",
                                                            client_uuid, client.get_public_key()))

        elif request.get_code() == RequestCode.MESSAGE_SEND_REQUEST:
            to_client_id, message_type, content_size, message_content = request.get_payload()  # get payload

            message = self.add_message(to_client_id, request.get_client_uuid(), message_type, content_size,
                                       message_content)  # add msg to dict

            return Response.successful_message_response(struct.pack(f"<{UUID_LENGTH}sI",
                                                                    to_client_id, message.get_message_id()))

        elif request.get_code() == RequestCode.AWAITING_MESSAGES_REQUEST:
            awaiting_messages = self.get_awaiting_messages(request.get_client_uuid())  # get list of awaiting messages
            remove_list = {}
            payload = b""

            for message in awaiting_messages.values():
                # add message header
                payload += message.get_from_client()

                payload += struct.pack(f"<IBI",
                                       message.get_message_id(), message.get_message_type().value,
                                       message.get_content_size())

                if message.get_message_type() != MessageType.SYMMETRICAL_KEY_REQUEST:  # add payload if it exists
                    payload += message.get_content()

                remove_list[message.get_message_id()] = message.get_message_id()

            response = Response.awaiting_message_response(payload)

            if remove_list:  # if the message was created successfully remove the messages that will be sent
                self.remove_messages(request.get_client_uuid(), remove_list)

            return response
        else:  # invalid request type return error
            return Response.general_error_response()
