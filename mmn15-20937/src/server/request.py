"""
the file contains the definition for the response class and its methods
@author Ilay Zvi
"""
import struct
from exceptions import ProtocolError
from constants import *


class Request:
    """
    Class Request initializes Request objects using its static method parse_request

    Attributes:
        client_id (UUID) : the client's UUID.
        version (unsigned int) : the client's application version.
        code (unsigned int) : the Request's Code.
        payload (str or none) : the payload of the request.
    """

    def __init__(self, client_id, version, code, payload):
        self.client_id = client_id
        self.version = version
        self.code = code
        self.payload = payload

    def get_client_uuid(self):
        return self.client_id

    def get_code(self):
        return self.code

    def get_payload(self):
        return self.payload

    @staticmethod
    def parse_request(socket):
        """
        parses the header of the request and calls onto parse_payload to obtain the payload of the request
        """

        # parses header
        client_id = socket.recv(UUID_LENGTH)  # get client id
        buffer = socket.recv(VERSION_LENGTH + CODE_LENGTH)
        version, code = struct.unpack('<BH', buffer)  # unpack version and request code

        try:
            code = RequestCode(code)
        except ValueError:
            raise ValueError("Invalid Request Code")

        return Request(client_id, version, code, Request.parse_payload(socket, code))

    @staticmethod
    def parse_payload(socket, code):
        """
        parses the payload of the request
        """
        # read and unpack size of payload
        payload_size = struct.unpack("<I", socket.recv(SIZE_LENGTH))[0]

        # check that the size matches protocol for constant size fields(not msg_send_rqst)
        if code != RequestCode.MESSAGE_SEND_REQUEST and payload_size != payload_sizes[code]:
            raise ProtocolError(f"Payload size mismatch expected{payload_sizes[code]}, got {payload_size}")

        if payload_size == 0:  # our payload is the right size and is also zero, no need to parse it
            return

        payload = socket.recv(payload_size)  # read payload

        if len(payload) != payload_size:  # check that payload size matches the size that was sent in the message
            raise ProtocolError(f"Payload size mismatch expected{payload_size}, got {len(payload)}")

        # follow each code request's protocol
        if code == RequestCode.REGISTRATION_REQUEST:
            username, public_key = struct.unpack(f'<{USERNAME_LENGTH}s{PUBLIC_KEY_LENGTH}s', payload)
            return username, public_key
        elif code == RequestCode.PUBLIC_KEY_REQUEST:
            try:
                client_id = payload
                return client_id
            except ValueError:
                raise ValueError("Invalid client id - unable to convert to UUID")
        elif code == RequestCode.MESSAGE_SEND_REQUEST:
            try:
                to_client_id = payload[0:UUID_LENGTH]
                message_type, content_size = struct.unpack(f"<BI", payload[
                                                                   UUID_LENGTH:UUID_LENGTH + MESSAGE_TYPE_LENGTH + CONTENT_SIZE_LENGTH])

                message_content = struct.unpack(f"<{content_size}s",
                                                payload[UUID_LENGTH + MESSAGE_TYPE_LENGTH + CONTENT_SIZE_LENGTH:])[0]

                try:
                    message_type = MessageType(message_type)
                except ValueError:
                    raise ValueError("Invalid message type")

                if content_size != len(message_content):
                    raise ProtocolError(f"Payload size mismatch expected{content_size}, got {len(message_content)}")

                return to_client_id, message_type, content_size, message_content
            except ValueError:
                raise ValueError("Invalid client id - unable to convert to UUID")
