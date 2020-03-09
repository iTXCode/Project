#include"httplib.h"

void Handler(const httplib::Request& req,
    httplib::Response& resp){
  (void)req;
  resp.set_content("<html><h1>hello</h1></html>", "text/html");
    //该函数用来设置博客正文
}

int main(){
  using namespace httplib;
  Server server;
  server.Get("/",Handler);//将Handler关联到了根目录下   
   //http协议用来处理Get请求
 server.listen("0.0.0.0",9093); //启动listen 服务器
}
