flags = -std=gnu11 -Wall -pthread
dir = code

serverSource = $(dir)/server.c $(dir)/mainTools.c
serverAll = $(serverSource) $(dir)/mainTools.h

clientSource = $(dir)/client.c $(dir)/mainTools.c
clientAll = $(clientSource) $(dir)/mainTools.h

server: $(serverAll)
	gcc $(flags) $(serverSource) -o server

client: $(clientAll)
	gcc $(flags) $(clientSource) -o client