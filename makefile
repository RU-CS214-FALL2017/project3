flags = -std=gnu11 -Wall -pthread
dir = code

serverSource = $(dir)/server.c $(dir)/mainTools.c $(dir)/socketTools.c
serverAll = $(serverSource) $(dir)/mainTools.h $(dir)/socketTools.h

clientSource = $(dir)/client.c $(dir)/mainTools.c $(dir)/socketTools.c
clientAll = $(clientSource) $(dir)/mainTools.h $(dir)/socketTools.h

server: $(serverAll)
	gcc $(flags) $(serverSource) -o server

client: $(clientAll)
	gcc $(flags) $(clientSource) -o client