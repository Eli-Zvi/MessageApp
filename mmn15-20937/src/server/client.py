"""
Client class - represents a client and its respective information
@author <Ilay Zvi>
"""


class Client:
    """
    class Client represents a client and its respective information

    Attributes:
        uuid (UUID) : the client's UUID.
        username (str) : the client's username.
        public_key (str) : the client's public_key.
    """
    def __init__(self, uuid, username, public_key):
        self.uuid = uuid
        self.username = username  # will be stored in bytes with a null terminated at the end
        self.public_key = public_key

    def get_public_key(self):
        return self.public_key

    def get_user_uuid(self):
        return self.uuid

    def get_username(self):
        return self.username
