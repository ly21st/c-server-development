#!/bin/bash

# stop
kill -9 $(ps aux | grep "./bin_cgi/upload.cgi" | grep -v grep | awk '{print $2}')

# start
spawn-fcgi -a 127.0.0.1 -p 8002 -f ./bin_cgi/upload.cgi
