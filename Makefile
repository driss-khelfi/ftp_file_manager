# Variables
SERVER = lpf_server
CLIENT = lpf
USER ?= Bob
IP ?= 127.0.0.1
PORT ?= 8080
FILE ?= goupix.png
ACTION ?= -upload

# Compilation
all: $(SERVER) $(CLIENT)

$(SERVER): lpf_server.cpp
	g++ -Wall -std=c++11 -o $(SERVER) lpf_server.cpp -lws2_32

$(CLIENT): lpf.cpp
	g++ -Wall -std=c++11 -o $(CLIENT) lpf.cpp -lws2_32

# Commandes
run_server: $(SERVER)
	./$(SERVER)

run_client: $(CLIENT)
	./$(CLIENT) $(USER)@$(IP):$(PORT) $(ACTION) $(FILE)

# Nettoyage
clean:
	rm -f $(SERVER) $(CLIENT)
