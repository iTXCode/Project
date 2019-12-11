////////////////////////////////////////
//创建一些相关的类来封装数据库操作
////////////////////////////////////////
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<memory>
#include<mysql/mysql.h>
#include<jsoncpp/json/json.h>


namespace blog_system{
    static MYSQL* MySQLInit(){
        //初始化一个MySQL 句柄并建立连接
        //1.创建一个句柄
        MYSQL* connect_fd=mysql_init(NULL);
        //2.和数据库建立链接
        if(mysql_real_connect(connect_fd,"127.0.0.1",
        "root","0","bolg_system",3306,NULL,0)==NULL)
        {
            printf("连接失败!%s\n",mysql_error(connect_fd));
            return NULL;
        }

        //3.设定字符编码格式
        mysql_set_character_set(connect_fd,'utf8');
        return connect_fd;
    }

    static void MySQLRelease(MYSQL* connect_fd){
        //释放句柄并断开连接
        mysql_close(connect_fd);

    }

    //创建一个类,用于操作博客表的类
    class BolgTable{
        public:
        BolgTable(MYSQL* connect_fd)
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
        //  connent:"博客正文"    
        //  create_time："创建时间"
        //  tag_id:"标签id"
       // }
        //使用json最大的好处就是便于扩展

        bool Insert(const Json::Value& bolg){
             //核心就是拼装 sql 语句

            const std::string& connent=blog["content"].asString();
            //获取博客正文

            //to缓冲区的申请,需要动态分配
            // char* to=new char[content.size()*2+1]; 
            //这种方式申请的资源存在改动后不能及时释放的问题
            //这里to 的长度是文档要求的2*size+1
            
            
            std::unique_ptr<char> to(new char[content.size()*2+1]);
            //使用智能指针来实现动态申请和释放资源


            mysql_real_escape_string(mysql_,to.get(),
            content.c_str(),content.size());

            //mysql_real_escape_string参数要求的to 是char*类型的
            //但此处的to 已经不在世char*类型的了,所以需要通过to.get()
            //的方式去获取里面的char*
            
            //核心就是拼装sql语句
            std::unique_ptr<char> sql(new char[content.size()*2+4096]);
            //4095个空间用来存放标题那些内容
            
            //拼接SQL语句
            sprintf(sql.get(),"insert into blog_table value(null,'%s','%s',
            %d,'%s')",
            blog["title"].asCString(),
            //blog["content"].asCString(),
            to.get();
            blog["tag_id"].asCString(),
            blog["create_time"].asCString());


            //执行
            int ret=mysql_query(mysql_,sql.get());
            if(ret!=0){
                printf("执行插入博客失败!%s\n",mysql_error(mysql_));
                delete[] to;
                return false;
            }
            delete[] to;//释放动态申请的资源
            //通过blog["title"]的方式获取标题
            return true;
        }
        //获取所有的博客信息
        //blogs做为一个输出型参数
        bool SelectAll(Json::Value* blogs,const std::string& tag_id){
            return true;
        }
        

        //blog同样是输出型参数,他表示根据当前的 blog_id 在数据库中扎到具体的
        //博客内容通过blog参数返回给调用者
        bool SelectOne(int32_t blog_id,Json::Value* blog){
            return true;  
        }

        bool Update(const Json::Value& blog){
            
        }

        bool Delete(int32_t blog_id){
            return true;
        }
        private:
        MYSQL* mysql_;
    };


    class TagTable{
        public:
        TagTable(){

        }
        bool Insert(const Json::Value& tag){
            return ture;
        }
        bool Delete(int32_t tag_id){
            return ture;
            
        }

        bool SelectAll(Json::Value* tags){
            return ture;
        }
        
        private:
    };
}//end of blog_system