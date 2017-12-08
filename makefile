flags = -std=gnu11 -Wall -pthread
dir = code

serverSource = $(dir)/server.c $(dir)/mainTools.c $(dir)/serverTools.c $(dir)/tools.c $(dir)/csvStore.c $(dir)/syncWraps.c $(dir)/sorter.c
serverAll = $(serverSource) $(dir)/mainTools.h $(dir)/serverTools.h $(dir)/tools.h $(dir)/csvStore.h $(dir)/syncWraps.h $(dir)/sorter.h

clientSource = $(dir)/client.c $(dir)/mainTools.c $(dir)/clientTools.c $(dir)/tools.c $(dir)/socketPool.c $(dir)/syncWraps.c
clientAll = $(clientSource) $(dir)/mainTools.h $(dir)/clientTools.h $(dir)/tools.h $(dir)/socketPool.h $(dir)/syncWraps.h

server: $(serverAll)
	gcc $(flags) $(serverSource) -o server

client: $(clientAll)
	gcc $(flags) $(clientSource) -o client