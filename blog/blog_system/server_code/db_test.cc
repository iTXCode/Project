#include"db.hpp"

//使用该程序来测试封装的MYSQL操作是否正确
//采用单元测试的方式来进行测试,最大的意义在于一旦某个问题
//出现问题就可以立即发现


void TestBlogTable(){
    Json::StyledWriter  writer;
    MYSQL* mysql=blog_system::MySQLInit();
    blog_system::BlogTable blog_table(mysql);
 
    Json::Value blog;

    //测试插入
//
//    blog["title"]="我的第一篇博客";
//    blog["content"]="我要去腾讯!";
//    blog["tag_id"]=1;
//    blog["create_time"]="2019/12/13";
//    bool ret=blog_table.Insert(blog);
//    printf("insert : %d \n",ret);
//

    //测试查找
    
  //  Json::Value blogs;
  //  bool ret=blog_table.SelectAll(&blogs);
  //  printf("select all %d\n",ret);
  //  printf("blogs:%s\n",writer.write(blogs).c_str());
  //  
  // 
    

    //测试单个查找

   
  //  bool ret=blog_table.SelectOne(1,&blog);
  //  printf("select one %d\n",ret);
  //  printf("blog:%s\n",writer.write(blog).c_str());
  //  blog_system::MySQLRelease(mysql);
  // 


    //测试修改博客
    
   // blog["blog_id"]=1;
   // blog["title"]="我的第一篇C语言博客";
   // blog["content"]="1.我的第一个项目\n,我期待的工作'哈哈'";
   // bool ret=blog_table.Update(blog);
   // printf("update %d\n",ret);
   // 
    
   //测试删除
  // 
  //  bool ret=blog_table.Delete(1);
  //  printf("delete %d\n",ret);
  // 

    blog_system::MySQLRelease(mysql);

}


void TestTagTable(){
    Json::StyledWriter writer;
    MYSQL* mysql=blog_system::MySQLInit();
    blog_system::TagTable tag_table(mysql);
    //插入测试
  
    
//    Json::Value tag;
//    tag["tag_name"]="C语言";
//    bool ret=tag_table.Insert(tag);
//    printf("insert %d\n",ret);
//
    
    //测试查找
   
 //   Json::Value tags;
 //   bool ret=tag_table.SelectAll(&tags);
 //   printf("select %d\n",ret);
 //   printf("tags:%s\n",writer.write(tags).c_str());
 // 
    
    //测试删除
    /*
    bool ret=tag_table.Delete(1);
    printf("delete %d\n",ret);
    */

    
    blog_system::MySQLRelease(mysql);
}


int main(){
    //TestBlogTable();

   TestTagTable();
    return 0;
}



 
 
 
 
 
 
  

