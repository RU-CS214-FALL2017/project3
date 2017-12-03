flags = -std=gnu11 -Wall -pthread
dir = code

serverSource = $(dir)/server.c $(dir)/mainTools.c
serverAll = $(serverSource) $(dir)/mainTools.h

clientSource = $(dir)/client.c $(dir)/sorter.c $(dir)/tools.c $(dir)/mainTools.c $(dir)/queue.c
clientAll = $(clientSource) $(dir)/sorter.h $(dir)/tools.h $(dir)/mainTools.h $(dir)/queue.h

server: $(serverAll)
	gcc $(flags) $(serverSource) -o server

client: $(clientAll)
	gcc $(flags) $(clientSource) -o client