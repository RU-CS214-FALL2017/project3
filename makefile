flags = -std=gnu11 -Wall -pthread
dir = code

serverSource = $(dir)/server.c $(dir)/mainTools.c $(dir)/serverTools.c $(dir)/tools.c
serverAll = $(serverSource) $(dir)/mainTools.h $(dir)/serverTools.h $(dir)/tools.h

clientSource = $(dir)/client.c $(dir)/mainTools.c $(dir)/clientTools.c $(dir)/tools.c
clientAll = $(clientSource) $(dir)/mainTools.h $(dir)/clientTools.h $(dir)/tools.h

server: $(serverAll)
	gcc $(flags) $(serverSource) -o server

client: $(clientAll)
	gcc $(flags) $(clientSource) -o client