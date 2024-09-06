# Variables
CC = g++
CFLAGS = -Wall
LIBS = -lws2_32
SERVER_SRC = lpf_server.cpp
CLIENT_SRC = lpf_client.cpp
SERVER_OUT = lpf_server.exe
CLIENT_OUT = lpf_client.exe

# Règle par défaut : tout compiler
all: $(SERVER_OUT) $(CLIENT_OUT)

# Règle pour compiler le serveur
$(SERVER_OUT): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_OUT) $(SERVER_SRC) $(LIBS)

# Règle pour compiler le client
$(CLIENT_OUT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_OUT) $(CLIENT_SRC) $(LIBS)

# Règle pour nettoyer les fichiers générés
clean:
	del $(SERVER_OUT) $(CLIENT_OUT)

# Règle pour exécuter le serveur
run_server: $(SERVER_OUT)
	./$(SERVER_OUT)

# Règle pour exécuter le client (tu peux adapter les arguments)
run_client: $(CLIENT_OUT)
	./$(CLIENT_OUT) user@127.0.0.1:8080 -upload example.txt

