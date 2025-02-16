"""
Message class - represents a message its recipient and sender, the type of message, and the contents
@author <Ilay Zvi>
"""


class Message:
    """
        class Client represents a client and its respective information

        Attributes:
            to_client (UUID) : the receiving client's UUID.
            from_client (UUID) : the sending client's UUID.
            message_type (unsigned int) : the message type.
            content_size (unsigned int) : the size of the message's content.
            content (str) : the message's content.
    """
    ID = 1

    def __init__(self, to_client, from_client, message_type, content_size, content):
        self.message_id = Message.ID
        Message.ID += 1
        self.to_client = to_client
        self.from_client = from_client
        self.message_type = message_type
        self.content_size = content_size
        self.content = content

    def get_to_client(self):
        return self.to_client

    def get_from_client(self):
        return self.from_client

    def get_message_type(self):
        return self.message_type

    def get_content_size(self):
        return self.content_size

    def get_content(self):
        return self.content

    def get_message_id(self):
        return self.message_id