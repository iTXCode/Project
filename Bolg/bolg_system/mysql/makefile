all:insert select

insert:insert.cc
	g++ $^ -o $@ -L/usr/lib64/mysql  -lmysqlclient
# 需要指定哭的路径和库的名字

select:select.cc 
	g++ $^ -o $@ -L/usr/lib64/mysql  -lmysqlclient 

.PHONY:clean
clean:
		rm insert  select
