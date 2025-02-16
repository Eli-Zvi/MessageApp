"""
exception file for the project
@author <Ilay Zvi>
"""


class ProtocolError(Exception):
    def __init__(self, message):
        super().__init__(message)

    def __str__(self):
        return f"Protocol Error: {self.args[0]}"
