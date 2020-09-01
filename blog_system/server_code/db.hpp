//////////////////////////////////////
//创建一些相关的类来封装数据库操作
////////////////////////////////////////
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<memory>
#include<mysql/mysql.h>
#include<jsoncpp/json/json.h>
#include"MD5.hpp"
//该头文件需要安装 jsoncpp-devel.x86_64 安装包
//yum list | grep jsoncpp 
//yum install jsoncpp-devel.x86_64
//devel 后缀表示开发者工具包
namespace blog_system{
    static MYSQL* MySQLInit(){
        //初始化一个MySQL 句柄并建立连接
        //1.创建一个句柄
        MYSQL* connect_fd=mysql_init(NULL);
        //2.和数据库建立链接
        //连接过程中需要指定 一些必要的信息
        //   1)连接句柄
        //   2)服务器的ip地址
        //   3)用户名
        //   4)密码
        //   5)数据库名
        //   6)服务器的端口号,数据库默认的是3306
        //   7)unix_sock NULL 
        //   8)client_flag  0
        if(mysql_real_connect(connect_fd,"127.0.0.1",
        "root","0","blog_system",3306,NULL,0)==NULL)
        {
            printf("连接失败!%s\n",mysql_error(connect_fd));
            return NULL;
        }

        //3.设定字符编码格式
        mysql_set_character_set(connect_fd,"utf8");
        return connect_fd;
    }

    static void MySQLRelease(MYSQL* connect_fd){
        //释放句柄并断开连接
        mysql_close(connect_fd);

    }

    //创建一个类,用于操作博客表的类
    class BlogTable{
        public:
        BlogTable(MYSQL* connect_fd)
        :mysql_(connect_fd)
        {
            //通过该构造函数获取到一个数据库的操作句柄
           
        }


        //以下操作相关参数都统一使用JSON 的方式
        //Json::Value 是 jsoncpp 中最核心的类
        //Json::Value 就表示一个具体的json对象
        //形如
        //{
        //  title:"博客标题"
        //  content:"博客正文"    
        //  create_time："创建时间"
        //  tag_id:"标签id"
       // }
        //使用json最大的好处就是便于扩展

        bool Insert(const Json::Value& blog){
             //核心就是拼装 sql 语句

            const std::string& content=blog["content"].asString();
            //获取博客正文

            //to缓冲区的申请,需要动态分配
            // char* to=new char[content.size()*2+1]; 
            //这种方式申请的资源存在改动后不能及时释放的问题
            //这里to 的长度是文档要求的2*size+1
            
            
            std::unique_ptr<char> to(new char[content.size()*2+1]);
            //使用智能指针来实现动态申请和释放资源
            //*2+1是文档的要求

            mysql_real_escape_string(mysql_,to.get(),
            content.c_str(),content.size());

            //mysql_real_escape_string参数要求的to 是char*类型的
            //但此处的to 已经不在世char*类型的了,所以需要通过to.get()
            //的方式去获取里面的char*
            
            //核心就是拼装sql语句
            std::unique_ptr<char> sql(new char[content.size()*2+4096]);
            //4095个空间用来存放标题那些内容
            
            //拼接SQL语句
            sprintf(sql.get(),"insert into blog_table value(null,'%s','%s',%d,'%s')",
            blog["title"].asCString(),
            //blog["content"].asCString(),
            to.get(),
            blog["tag_id"].asInt(),
            blog["create_time"].asCString());


            //执行
            int ret=mysql_query(mysql_,sql.get());
            if(ret!=0){
                printf("执行插入博客失败!%s\n",mysql_error(mysql_));
                return false;
            }
            //释放动态申请的资源
            //通过blog["title"]的方式获取标题

            printf("执行插入操作成功!\n");
            return true;
        }
        //获取所有的博客信息
        //blogs做为一个输出型参数
        bool SelectAll(Json::Value* blogs,const std::string& tag_id=""){
            //查找不需要太长的sql，固定长度就可以了
            char sql[1024*4]={0};
             if( tag_id ==""){
                 //tag_id为空此时不需要按照tag来进行筛选结果
                 sprintf(sql,"select blog_id,title,tag_id,create_time from blog_table");
             }else{
                 //此时就需要对结果按找tag进行筛选
                 sprintf(sql,"select blog_id,title,tag_id,create_tiem from blog_table where tag_id=%d",
                 std::stoi(tag_id));
                 //stoi 将c风格的字符串转换成整型数字，atoi 将整数转成string c++ 11提供的功能
             }

             int ret=mysql_query(mysql_,sql);//数据库进行操作
             if(ret!=0){
                 printf("执行查找所有博客失败!%s\n",mysql_error(mysql_));
                 return false;
             }
             
             MYSQL_RES* result=mysql_store_result(mysql_);//将结果存储在MYSQL_RES类型的结构体中 
             int rows=mysql_num_rows(result);//记录结果集合中的行数
             //遍历结果集合然后将结果过写到blogs参数中,返回给调用者
             for(int i=0;i<rows;i++){
                 MYSQL_ROW  row=mysql_fetch_row(result);
                 //遍历结果集合中的每行
                 Json::Value blog;   //单个博客

                 //row[]中的下标和上面的select 语句中写的列的顺序相关联
                 blog["blog_id"]=atoi(row[0]); //因为blog设置的是整数,所以需要将row转成c风格的字符串(atoi)
                 blog["title"]=row[1];
                 blog["tag_id"]=atoi(row[2]);
                 blog["create_time"]=row[3];
                 blogs->append(blog);
                 }
            //mysql 查询的结果集合需要记得及时释放
            mysql_free_result(result);  
            printf("执行查找所有博客成功!,共查找到 %d 条博客!\n",rows);   
            return true;
        }
        

        //blog同样是输出型参数,他表示根据当前的 blog_id 在数据库中找到具体的
        //博客内容通过blog参数返回给调用者
        bool SelectOne(int32_t blog_id,Json::Value* blog){
            char sql[1024]={0};//用于存储查询结果
            sprintf(sql,"select blog_id,title,content,tag_id,create_time from blog_table where blog_id=%d",blog_id);

            //数据库执行
            int ret=mysql_query(mysql_,sql);
            if(ret!=0){
                printf("执行查找博客失败! %s\n",mysql_error(mysql_));
                return false;
            }

            MYSQL_RES* result=mysql_store_result(mysql_);
            int rows=mysql_num_rows(result);
            if(rows!=1){
                printf("查找的博客不是一条!实际查询到的结果有%d条\n",rows);
                return false;
            }

            MYSQL_ROW row=mysql_fetch_row(result);
            (*blog)["blog_id"]=atoi(row[0]);
            (*blog)["title"]=row[1];
            (*blog)["content"]=row[2];
            (*blog)["tag_id"]=atoi(row[3]);
            (*blog)["create_time"]=row[4];
            return true;  
        }

        bool Update(const Json::Value& blog){
            //因为
            const std::string& content=blog["content"].asString();
            //获取博客正文

            //to缓冲区的申请,需要动态分配
            // char* to=new char[content.size()*2+1]; 
            //这种方式申请的资源存在改动后不能及时释放的问题
            //这里to 的长度是文档要求的2*size+1
            
            std::unique_ptr<char> to(new char[content.size()*2+1]);
            //使用智能指针来实现动态申请和释放资源
            mysql_real_escape_string(mysql_,to.get(),
            content.c_str(),content.size());
            
            //核心就是拼装sql语句
            std::unique_ptr<char> sql(new char[content.size()*2+4096]);
            sprintf(sql.get(),"update blog_table set title='%s',content='%s',tag_id=%d where blog_id=%d",
            blog["title"].asCString(),
            to.get(),
            blog["tag_id"].asInt(),
            blog["blog_id"].asInt());
            int ret=mysql_query(mysql_,sql.get());

            if(ret!=0){
                printf("更新博客失败!%s\n",mysql_error(mysql_));
                return false;
            }

            printf("更新博客成功!\n");
            return true;
        }

        bool Delete(int32_t blog_id){
            char sql[1024*4]={0};
            sprintf(sql,"delete from blog_table where blog_id=%d\n",blog_id);
            int ret=mysql_query(mysql_,sql);
            if(ret!=0){
                printf("删除博客失败!%s\n",mysql_error(mysql_));
                return false;
            }
            printf("删除成功!\n");
            return true;
        }
    private:
        MYSQL* mysql_;
    };


    class TagTable{
        public:
        TagTable(MYSQL* mysql):mysql_(mysql){}

        bool Insert(const Json::Value& tag){
            char sql[1024*4];
            sprintf(sql,"insert into tag_table values(null,'%s')",
            tag["tag_name"].asCString());
            //此时插入的数据只有两个参数,但是还是使用Json的原因
            //为了进一步的提高扩展性

            int ret=mysql_query(mysql_,sql);
            if( ret!=0){
                printf("插入标签失败!%s\n",mysql_error(mysql_));
                return false;
            }
            printf("插入标签成功!\n");
            return true;
        }
        bool Delete(int32_t tag_id){
            char sql[1024*4]={0};
            sprintf(sql,"delete from tag_table where tag_id=%d",tag_id);
            
            int ret=mysql_query(mysql_,sql);
            if(ret!=0){
                printf("删除标签失败! %s\n",mysql_error(mysql_));
                return false;
            }

            printf("删除标签成功!\n");
            return true;
        }

        bool SelectAll(Json::Value* tags){
            
            char sql[1024*4]={0};
            sprintf(sql,"select tag_id,tag_name from tag_table");
            int ret=mysql_query(mysql_,sql);
            if(ret!=0){
                printf("查找标签失败!%s\n",mysql_error(mysql_));
                return false;
            }
            MYSQL_RES* result=mysql_store_result(mysql_);
            int rows=mysql_num_rows(result);
            for(int i=0;i<rows;i++){
                MYSQL_ROW row=mysql_fetch_row(result);
                Json::Value tag;
                tag["tag_id"]=atoi(row[0]);
                tag["tag_name"]=row[1];
                tags->append(tag);
            }
            printf("查找标签成功! 一共找到%d 个\n",rows);
            return true;
        }
        
        private:
        MYSQL* mysql_;
    };

    class UserInfo{
      public:
        UserInfo(MYSQL* mysql)
        :mysql_(mysql)
        {

        }

        bool Insert(const Json::Value &user_info){
          //使用MD5程序对用户密码进行签名
        MD5 md5;
        
        if(user_info["user_password"].empty() || user_info["user_name"].empty()){
            printf("用户名或者用户密码为空!\n");
            return false;
        }
        std::string password = md5.StrMD5(user_info["user_passwed"].asCString());
    
          //申请空间用于存储MySQL指令
          char sql[1024*4]={0};
          sprintf(sql,"insert into user_table value(null,'%s','%s')",user_info["user_name"].asCString(),password.c_str());

          int ret=mysql_query(mysql_,sql);

          if(ret!=0){
            printf("用户注册失败:%s\n",mysql_error(mysql_));
            return false;
          }

          printf("用户注册成功!\n");
          return true; 
        }

        bool Check(const Json::Value& user_info){
          if(user_info["user_name"].asString().empty()){
            printf("用户名为空!\n");
            return false;
          }

          //检查用户密码
          char sql[1024*4]={0};
          sprintf(sql,"select user_password form user_table where user_name=%s",user_info["username"].asCString());

          int ret=mysql_query(mysql_,sql);

          if(!ret){
            printf("执行SQL语句出错:%s",mysql_error(mysql_));
            return false;
          }
          

          //核对传入的用户密码是否正确
          MD5 md5;
          std::string password=md5.StrMD5(user_info["user_password"].asCString());
          MYSQL_RES* result=mysql_store_result(mysql_);
          MYSQL_ROW row=mysql_fetch_row(result);
          if(row==nullptr){
            printf("没有该用户的线管信息!%s\n",user_info["user_name"].asCString());
            return false;
          }

          std::string pw=row[1];

          if(pw.compare(password)!=0){
            printf("用户密码输入错误!请重新输入\n");
            return false;
          }

          printf("欢迎登录本平台!\n");
          return true;
        }
      private:
        MYSQL* mysql_;
    };
}//end of blog_system


