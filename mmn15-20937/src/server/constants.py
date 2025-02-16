"""
constants - represents the constants, enums across the project
@author <Ilay Zvi>
"""
from enum import Enum


# enums
class RequestCode(Enum):
    REGISTRATION_REQUEST = 600
    CLIENT_LIST_REQUEST = 601
    PUBLIC_KEY_REQUEST = 602
    MESSAGE_SEND_REQUEST = 603
    AWAITING_MESSAGES_REQUEST = 604


class ResponseCode(Enum):
    SUCCESSFUL_REGISTRATION = 2100
    CLIENT_LIST = 2101
    PUBLIC_KEY = 2102
    SUCCESSFUL_CLIENT_MESSAGE = 2103
    AWAITING_MESSAGES = 2104
    GENERAL_ERROR = 9000

    def __str__(self):
        return self.name.replace('_', ' ').title()


class MessageType(Enum):
    SYMMETRICAL_KEY_REQUEST = 1
    SYMMETRICAL_KEY_RESPONSE = 2
    SEND_MESSAGE = 3


# Constants

SERVER_INFO_PATH = "myport.info"
LOCAL_HOST = '127.0.0.1'
UUID_LENGTH = 16
SERVER_VERSION = 1
VERSION_LENGTH = 1
CODE_LENGTH = 2
SIZE_LENGTH = 4
PUBLIC_KEY_LENGTH = 160
USERNAME_LENGTH = 255
MESSAGE_TYPE_LENGTH = 1
CONTENT_SIZE_LENGTH = 4
MESSAGE_ID_LENGTH = 4
DEFAULT_PORT = 1357

payload_sizes = {
    # represents the expected values for the request payloads - with MSG_SEND_REQUEST requiring an additional
    # content_size bytes
    RequestCode.REGISTRATION_REQUEST: USERNAME_LENGTH + PUBLIC_KEY_LENGTH,
    RequestCode.CLIENT_LIST_REQUEST: 0,
    RequestCode.PUBLIC_KEY_REQUEST: UUID_LENGTH,
    RequestCode.MESSAGE_SEND_REQUEST: UUID_LENGTH + MESSAGE_TYPE_LENGTH + CONTENT_SIZE_LENGTH,
    RequestCode.AWAITING_MESSAGES_REQUEST: 0
}
