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
  TagTable tag_table(mysql);
  UserInfo user_info(mysql);
  //3.创建服务器,并设置"路由"(HTTP服务器中的路由,和
  //IP协议的路由不一样),此处的路由指的是把 方法+path=》
  //哪个处理函数 关联关系声明清楚
  Server server;
  server.Post("/blog",[&blog_table](const Request& req,
        Response& resp){
      //此处为了方便直接使用拉姆达表达式而不适用处理函数的方式
       printf("新增博客!\n");

       //1.获取到请求中的 body 并解析成 json
       Json::Reader reader;   //将字符串转换成json对象
       Json::FastWriter writer;
       Json::Value req_json;
       Json::Value resp_json;
      bool ret= reader.parse(req.body,req_json);
      //参数一:解析的目标
      //参数二:解析结果的存储位置 
      //将接收到的字符串转换成Json格式
      
      if(!ret){
        //解析出错,提示用户
        printf("解析请求失败!%s\n",req.body.c_str());
        //构造一个响应对象,告诉客户端出错了
        resp_json["ok"]=false;
        resp_json["reason"]="input data parse failed!";
        resp.status=400;
        resp.set_content(writer.write(resp_json),"application/json");
        //参数一表示设置resp的返回内容
        //参数二表示resp的返回类型
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
      

      //2.掉用数据库操作来获取所有博客结果
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

//正则表达式(任何语言里面都有正则表达式的相关操作)
//用特殊符号来表述一个字符串应该具有那些特殊特征。
//特征包括:包含那些特定字符串,以什么开头,以什么结尾,特定
//字符重复出现几次。。。

//R"(/blog/(\d+))"表示原始字符串,在原始字符串中没有进行转义(转义字符不生效)
//正真的字符串内部是这些内容(/blog/(\d+))
//(\d+)是正则表达式,，\d表示0~9中的某个数字字符
//而后面的+表示该字符出现一次/多次

  server.Get(R"(/blog/(\d+))",[&blog_table](const Request& req,
        Response& resp){
 
      Json::FastWriter writer; 
      //1.解析获取到 blog_id
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
   //修改某个指定的博客内容
      //1.获取博客id
      int32_t blog_id=std::stoi(req.matches[1].str());
      printf("修改id=%d的博客!\n",blog_id);

      //2.获取请求并解析结果
      Json::Reader reader;
      Json::FastWriter writer;
      Json::Value resp_json; 
      Json::Value req_json; 
      bool ret=reader.parse(req.body,req_json);
      //参数req.body指定需要解析的对象
      //参数req_json 指定解析结果的放置地点
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
        resp_json["reason"]="update blog request format error";
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
      resp_json["ok"]=true;     
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      });//修改某个博客
/*使用原始字符串的原因:
 * 此处需要指定某个博客id或者标签id的时候
 * 需要用到\d，为了避免出现转义的问题此处使用原始
 * 字符串(也为了美观而不使用\\d)
 * */

  server.Delete(R"(/blog/(\d+))",[&blog_table](const Request& req,Response& resp){

      Json::FastWriter writer;
      Json::Value resp_json;
      //1获取到blog_id
      int32_t blog_id=std::stoi( req.matches[1].str() );
      printf("删除id为%d的博客!\n",blog_id);

      //2.调用数据库操作
      bool ret=blog_table.Delete(blog_id);
      if(!ret){
      printf("执行删除失败!\n");
      resp_json["ok"]=false;
      resp_json["reason"]="delete blog failed";
      resp.status=500;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      }
    
       //构造一个正确的响应
       resp_json["ok"]=true;
       resp.set_content(writer.write(resp_json),"application/json");
       return;
      }); //删除某个特定的博客
  
  server.Post("/tag",[&tag_table](const Request& req,Response& resp){

      Json::FastWriter writer;
      Json::Reader reader;
      Json::Value req_json;
      Json::Value resp_json;
      //1.解析请求
      
     bool ret= reader.parse(req.body,req_json);
     if(!ret){
      printf("插入标签失败!\n");

      resp_json["ok"]=false;
      resp_json["reason"]="insert tag req parse failed";
      resp.status=400;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      }
      
      //2.对请求进行校验
     if(req_json["tag_name"].empty()){
       printf("插入标签失败!\n");
       resp_json["ok"]=false;
       resp_json["reason"]="insert tag farmat error";
       resp.status=400;
       resp.set_content(writer.write(resp_json),"application/json");
       return;
     }
      

     //3.调用数据库操作完成插入
     ret=tag_table.Insert(req_json);
     if(!ret){
       printf("插入标签失败!\n");
       resp_json["ok"]=false;
       resp_json["reason"]="insert tag database failed";
       resp.status=500;
       resp.set_content(writer.write(resp_json),"application/json");

       return;
     }

     //4.返回正确的结果
     resp_json["ok"]=true;
     resp.set_content(writer.write(resp_json),"qpplication/json");
     return;
      }); //新增标签


  server.Get("/tag",[&tag_table](const Request& req,
        Response& resp){

      //1.不需要解析参数,直接执行数据库操作
      Json::FastWriter writer;
      Json::Value resp_json;
      bool ret=tag_table.SelectAll(&resp_json);
      if(!ret){
      printf("获取标签失败!\n");
      resp_json["ok"]=false;
      resp_json["reason"]="get tag database failed";
      resp.status=500;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      }

      //2.构造正确的结果
      
      resp.set_content(writer.write(resp_json),"application/json");
      }); //查看所有的标签


  server.Delete(R"(/tag/(\d+))",[&tag_table](const Request& req,
        Response& resp){

      Json::FastWriter writer;
      Json::Value resp_json;

      //1.解析出tag_id
      int32_t tag_id=std::stoi(req.matches[1].str());
      printf("删除id为%d的标签!\n",tag_id);
      

      //2.执行数据库操作
      bool ret=tag_table.Delete(tag_id);
      if(!ret){
      printf("标签删除失败!\n");
      resp_json["ok"]=false;
      resp_json["reason"]="delete tag database  failed";
      resp.status=500;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      }

      //3.构造正确的响应
      resp_json["ok"]=true;
      resp.set_content(writer.write(resp_json),"application/json");
      }); //删除某个特定的标签
  
  //用户登录
  server.Post("/login",[&user_info](const Request& req, Response& resp){
    printf("用户登录!\n");
    Json::FastWriter writer;
    Json::Reader reader;
    Json::Value req_json;
    Json::Value resp_json;

    bool ret=reader.parse(req.body,req_json);
    if(!ret){
      printf("解析出错！ %s\n",req.body.c_str());

      //构造一个响应对象给客户端
      resp_json["ok"]=false;
      resp_json["reason"]="login parse request failed!";
      resp.status=400;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
    }

    if(req_json["user_name"].empty()){
      //解析出错,给用户提示
      printf("查找该用户失败! %s\n",req.body.c_str());

      //构造一个响应对象,告诉客户端出错
      resp_json["ok"]=false;
      resp_json["reason"]="search user format failed!";
      resp.status=400;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
    }

    //指定数据库操作
    ret=user_info.Check(req_json);
    printf("here");
    if(!ret){
      printf("用户密码不匹配!\n");

      resp_json["ok"]=false;
      resp_json["reason"]="password is wrong!\n";
      resp.status=400;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
    }


    //构造一个正确的返回结果
    resp_json["ok"]=true;
    resp.set_content(writer.write(resp_json),"application/json");
    return;
  });

  server.Post("/sign_in",[&user_info](const Request& req,Response& resp){
    printf("注册用户!\n");

    Json::FastWriter writer;
    Json::Reader reader;
    Json::Value req_json;
    Json::Value resp_json;

   bool ret=reader.parse(req.body,req_json);
    
    if(!ret){
      printf("解析出错! %s\n",req.body.c_str());
      
      //构造一个响应对象给客户端
      resp_json["ok"]=false;
      resp_json["reason"]="login parse request failed!";
      resp.status=400;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
    }

    if(req_json["user_name"].empty()){
      //解析出错,给用户提示
      printf("插入用户失败! %s\n",req.body.c_str());
      //构造一个响应对象,告诉客户端出错

      resp_json["ok"]=false;
      resp_json["reason"]="Insert user format failed!";
      resp.status=400;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
    }
   


   //TODO
   printf("user_name:%s user_password:%s\n",req_json["user_name"],req_json["user_password"]);
    ret=user_info.Insert(req_json);
    printf("haha\n");
    if(!ret){
      //插入用户失败
      printf("用户注册失败!");
      resp_json["ok"]=false;
      resp_json["reason"]="insert user failed!";
      resp.status=400;
      resp.set_content(writer.write(resp_json),"application/json");

      return ;
    }
  

    //构造一个正确的响应
    printf("插入工程!\n");
    resp_json["ok"]=true;
    resp.set_content(writer.write(resp_json),"application/json");
  });
  server.set_base_dir("./wwwroot");//设置静态目

  server.listen("0.0.0.0",9093); //启动listen 服务器
 return 0;
}
