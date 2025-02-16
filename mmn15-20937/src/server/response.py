"""
the file contains the definition for the response class and its methods
@author Ilay Zvi
"""
import uuid
from constants import *
import struct


class Response:
    """
    Class Response initializes Response objects using its static methods and packs the responses using struct

    Attributes:
        code (ResponseCode) : the response's code.
        version (unsigned int) : the client's application version.
        payload_size (unsigned int) : the size of the payload.
        payload (str or none) : the payload of the response.
    """

    def __init__(self, code: ResponseCode, payload=None):
        self.version = SERVER_VERSION
        self.code = code
        if payload is None:
            self.payload_size = 0
        else:
            self.payload_size = len(payload)
        self.payload = payload

    def pack_response(self):
        """
        packs the data stored in the response object, so it can be sent to the client
        """
        try:
            if self.payload is None:
                return struct.pack(f"<BHI", self.version, self.code.value, self.payload_size)
            else:
                return struct.pack(f"<BHI", self.version, self.code.value, self.payload_size) + self.payload
        except struct.error:
            print("Error during data packing")
            return Response.general_error_response()  # not recursive, will exit after returning an error response

    """
    all response methods create the response objects according to their type and call pack_response
    so it can be sent to the client
    note: can be condensed down to one function
    """

    @staticmethod
    def registration_response(client_uuid: uuid.UUID):
        return Response(ResponseCode.SUCCESSFUL_REGISTRATION, client_uuid).pack_response()

    @staticmethod
    def client_list_response(client_list):
        return Response(ResponseCode.CLIENT_LIST, client_list).pack_response()

    @staticmethod
    def public_key_response(payload):  # payload consists of uuid and public_key related to the uuid
        return Response(ResponseCode.PUBLIC_KEY, payload).pack_response()

    @staticmethod
    def successful_message_response(payload):
        return Response(ResponseCode.SUCCESSFUL_CLIENT_MESSAGE, payload).pack_response()

    @staticmethod
    def awaiting_message_response(payload):
        return Response(ResponseCode.AWAITING_MESSAGES, payload).pack_response()

    @staticmethod
    def general_error_response():
        print("A general error has occurred")
        return Response(ResponseCode.GENERAL_ERROR).pack_response()
