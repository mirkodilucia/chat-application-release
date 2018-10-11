#!/bin/sh

tmux new-session "./msg_server/server 4242" \; \
	splitw -h "./msg_client/client 127.0.0.1 8081 127.0.0.1  4242" \; \
	select-pane -t 1 \; \
	splitw -v "./msg_client/client \"127.0.0.1\" 8082 \"127.0.0.1\"  4242" \; \
	select-pane -t 1 \; \
	splitw -v "./msg_client/client \"127.0.0.1\" 8083 \"127.0.0.1\"  4242" \; \
	attach
