FLAG=-std=c++14  -L/usr/lib64/mysql  -lmysqlclient -ljsoncpp  -lpthread


.PHONY:all 

all:db_test  blog_server

db_test:db_test.cc db.hpp
	g++ db_test.cc  -o db_test -g  $(FLAG)

blog_server:blog_server.cc 
	g++ blog_server.cc -o blog_server $(FLAG)

.PHONY:clean
clear:
	rm db_test
