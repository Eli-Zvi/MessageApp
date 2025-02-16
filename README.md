# **MessageApp - Encrypted Messaging Application**

## **Overview**

MessageApp is an instant messaging application similar to WhatsApp or Facebook Messenger. It follows a client-server architecture with end-to-end encryption (E2EE), ensuring that only the intended recipient can decrypt messages.

## **Architecture**

- Client: Written in C++
  
- Server: Written in Python, stateless, and supports multiple users using threads or selectors.
  
- Communication: Clients retrieve messages from the server using a pull request model.
  
- Security: Messages are encrypted end-to-end, so even the server cannot decrypt them.

## **Server Implementation**

### **Responsibilities**

- Manage registered users.
  
- Handle message transmission between users.
  
- Maintain a stateless protocol, meaning each request is independent.

### **Features**

- Port Configuration: Reads the port number from myport.info; defaults to 1357 if missing.
  
- Multi-user support: Utilizes threads or selectors for handling multiple users.
  
- Data Storage:
  
- Default: Stores user and message data in memory (RAM).
  
- #### **Clients Table (clients)**
  
  |Column|Type|Description|
  |------|----|-----------|
  |ID|16 bytes (UUID)|Unique identifier for each user|
  |UserName|String (255)|ASCII username (null-terminated)|
  |PublicKey|String (160 bytes)|User's public key|
  
- #### **Messages Table (messages)**
  
  |Column|Type|Description|
  |------|----|-----------|
  |ID(MSG_ID)|unsigned int (4 bytes)|Unique message ID.|
  |ToClient|16 bytes (UUID)|Recipient ID.|
  |FromClient|16 bytes (UUID)|Sender ID|
  |Message Type| 1 byte |Type of Message|
  |Content|String (Varies)|Encrypted message content|

### **Server Workflow**

- Read Port Configuration: Load myport.info for the port number.
  
- Listen for Client Requests: Run an infinite loop to handle client connections.

#### **Process Requests:**

- User Registration: Generates a UUID for the user and registers them.
  
- Retrieve Client List: Returns a list of all registered users.
  
- Send Message: Extracts message type and content, then stores it.
  
- Retrieve Messages: Fetches pending messages for a user and deletes them after successful delivery.

## **Client Implementation**

- Implements the pull request model to fetch messages from the server.
  
- Encrypts messages before sending them to ensure end-to-end encryption.

### **Security**

- The server does not decrypt messages.
  
- Only the recipient can decrypt the messages using their private key.
  
- Ensures privacy even when messages are stored on the server.
