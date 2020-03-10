#include<regex>
#include<signal.h>
#include"httplib.h"
#include"db.hpp"

 MYSQL *mysql=NULL;
int main(){
  using namespace httplib;
  using namespace blog_system;
  //1.先和数据库建立好连接
  mysql=blog_system::MySQLInit(); //建立句柄
  signal(SIGINT,[](int ){
      blog_system::MySQLRelease(mysql);
      }); //当按到Ctrl+c的时候自动触发这个操作

  //2.创建相关数据库处理对象
  BlogTable blog_table(mysql);


  Server server;
  server.Post("/blog",[&blog_table](const Request& req,
        Response& resp){
      //此处为了方便直接使用拉姆达表达式而不适用处理函数的方式
       printf("新增博客!\n");

       //1.获取到请求中的 body 并解析成 json
       Json::Reader reader;
       Json::FastWriter writer;
       Json::Value req_json;
       Json::Value resp_json;
      bool ret= reader.parse(req.body,req_json); 
       //将接收到的字符串转换成Json格式
      
      if(!ret){
        //解析出错,提示用户
        printf("解析请求失败!%s\n",req.body.c_str());
        //构造一个响应对象,告诉客户端出错了
        resp_json["ok"]=false;
        resp_json["reason"]="input data parse failed!";
        resp.status=400;
        resp.set_content(writer.write(resp_json),"application/json");
        return ;
      } 
      //2.对参数进行校验
       if(req_json["title"].empty()
           ||req_json["content"].empty()
           ||req_json["tag_id"].empty()
           ||req_json["create_time"].empty()){
           printf("请求数据格式有错!%s\n",req.body.c_str());
        //构造一个响应对象,告诉客户端出错了
        resp_json["ok"]=false;
        resp_json["reason"]="input data format error!";
        resp.status=400;
        resp.set_content(writer.write(resp_json),"application/json");
        return ;
       }
       //3.真正的调用 MySQL 接口来操作
       
       ret=blog_table.Insert(req_json);

       if(!ret){
         printf("博客插入失败!\n");
         resp_json["ok"]=false;
         resp_json["reason"]="blog insert failed!";
         resp.status=500;
         resp.set_content(writer.write(resp_json),"application/json");
         return;
       }

       //4.构造一个正确的响应给客户端
       printf("博客插入成功!\n");
       resp_json["ok"]=true;
       resp.set_content(writer.write(resp_json),"application/json");
       return ;
      }); //新增博客
  // server.Get("/",Handler);//将Handler关联到了根目录下   
   //http协议用来处理Get请求
   
  server.Get("/blog",[&blog_table](const Request& req,
        Response& resp){
      printf("查看所有博客!\n");

      //1.尝试获取 tag_id,如果tag_id这个参数不存在
      //返回空字符串
      const std::string& tag_id=req.get_param_value("tag_id");
      //就不需要解析请求了,也就是不需要合法性判定了
      

      //2.条用数据库操作来获取所有博客结果
      Json::Value resp_json;
      Json::FastWriter writer;
      bool ret=blog_table.SelectAll(&resp_json,tag_id);
      if(!ret){
      resp_json["ok"]=false;
      resp_json["reason"]="select all failed!";
      resp.status=500;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      }

      //3.成功：就构造响应结果
      resp.set_content(writer.write(resp_json),"appplication/json");
      return;

      });//查看所有博客列表


  server.Get(R"(/blog/(\d+))",[&blog_table](const Request& req,
        Response& resp){
 
      Json::FastWriter writer; 
      //1.解析过去到 blog_id
      int32_t blog_id=std::stoi( req.matches[1].str() );
      printf("查看blog_id=%d的博客!\n",blog_id);
      

      //2.直接调用数据库操作
      Json::Value resp_json;

      bool ret=blog_table.SelectOne(blog_id,&resp_json);
      if(!ret){
      resp_json["ok"]=false;
      resp_json["reason"]="查看指定博客失败:"+std::to_string(blog_id);

      resp.status=404;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      }

      //3.包装一个执行正确的响应
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      });//参数中的R表示原始 字符串 【功能]:查看某个博客

  server.Put(R"(/blog/(\d+))",[&blog_table](const Request& req,Response& resp){

      //1.获取博客id
      int32_t blog_id=std::stoi(req.matches[1].str());
      printf("修改id=%d的博客!\n",blog_id);

      //2.获取请求并解析结果
      Json::Reader reader;
      Json::FastWriter writer;
      Json::Value resp_json; 
      Json::Value req_json; 
      bool ret=reader.parse(req.body,resp_json);
      if(!ret){
        resp_json["ok"]=false;
        resp_json["reason"]="update blog parse request failed";
        resp.status=400;
        resp.set_content(writer.write(resp_json),"application/json");
        return;
      }

      //3.校验参数是否符合预期
      if(req_json["title"].empty()
          ||req_json["content"].empty()
          ||req_json["tag_id"].empty()){
        resp_json["ok"]=false;
        resp_json["reason"]="update blog parse request failed";
        resp.status=400;
        resp.set_content(writer.write(resp_json),"application/json");
        return;
      }


      //4.调用数据库操作来完成更新博客操作
      req_json["blog_id"]=blog_id;//从path中得到的id设置到json对象中

      ret=blog_table.Update(req_json);

      if(!ret){
        resp_json["ok"]=false;
        resp_json["reason"]="update blog database failed";
        resp.status=500;
        resp.set_content(writer.write(resp_json),"application/json");
        return;
      }

      //5.构造一个正确的返回结果
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      });//修改某个博客
/*使用原始字符串的原因:
 * 此处需要指定某个博客id或者标签id的时候
 * 需要用到\d，为了避免出现转义的问题此处使用原始
 * 字符串(也为了美观而不使用\\d)
 * */

  server.Delete(R"(/blog/(\d+))",[](const Request& req,Response& resp){

      }); //删除某个特定的博客
  
  server.Post("/tag",[](const Request& req,
        Response& resp){

      }); //新增标签

  server.Get("/tag",[](const Request& req,
        Response& resp){

      }); //查看所有的标签


  server.Delete(R"(/tag/(\d+))",[](const Request& req,
        Response& resp){

      }); //删除某个特定的标签
  server.set_base_dir("./wwwroot");//设置静态目录
 server.listen("0.0.0.0",9093); //启动listen 服务器
 return 0;
}
