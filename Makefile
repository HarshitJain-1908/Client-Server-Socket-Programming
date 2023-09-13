tag = client_ server_ concClient_ concServerMP_ concServerMT_ non_blockingServerSelect_ non_blockingServerPoll_ non_blockingServerEPoll_
all: q
q: $(tag)
client_: 
		gcc sequentialClient.c -o seqclient -fsanitize=address
server_:
		gcc sequentialServer.c -o seqserver
concClient_:
		gcc concurrentClient.c -o client -fsanitize=address -lpthread
concServerMP_:
		gcc concurrentServerMP.c -o fork_server
concServerMT_:
		gcc concurrentServerMT.c -o multithreaded_server -lpthread
non_blockingServerSelect_:
		gcc non_blockingServerSelect.c -o server_select
non_blockingServerPoll_:
		gcc non_blockingServerPoll.c -o server_poll
non_blockingServerEPoll_:
		gcc non_blockingServerEPoll.c -o server_epoll