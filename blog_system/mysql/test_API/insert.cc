/////////////////////////////
//通过这个程序使用MySQL API实现数据的插入功能
/////////////////////////////


#include<cstdio>
#include<cstdlib>
//编译器默认在/user/include 目录中查找头文件 
//mysql.h 在一个mysql的目录中
#include<mysql/mysql.h>

int main(){
    //1.创建一个连接数据库的句柄(遥控器)
    MYSQL *connect_fd = mysql_init(NULL);
    //2.和数据库建立连接(和TCP的区分开,这是应用层层次的连接)
    //连接过程需要指定一些必要信息
    //a.连接句柄，通过该句柄对数据库进行操作
    //b.服务器的ip地址
    //c.用户名
    //d.密码
    //e.数据库名(blog_system)
    //f.服务器的端口号
    //g.unix_sock NULL
    //h.client_flag 0
    if(mysql_real_connect(connect_fd,"0.0.0.0",
    "root","0","blog_system",3306,NULL,0)==NULL)
    //接口返回结果为一个判断结果
    //用户root后的参数是密码
    {
        printf("连接失败!%s\n",mysql_error(connect_fd));
        return 1;
    }
    printf("连接成功!\n");
    //3.设置编码方式
    // 我的电脑  mysql server 部分最初安装的时候已经设置成了utf8
    //  客户端这边也需要设置为utf8
    mysql_set_character_set(connect_fd,"utf8");

    //4.拼接SQL语句
    char sql[1024*4]={0};
    char title[]="立一个flag";
    char content[]="我要去腾讯";
    int tag_id=1;
    char date[]="2019/12/10 17:39";
    //将上述信息进行拼接
    sprintf(sql,"insert into blog_table values(null,'%s','%s','%d','%s')",
    title,content,tag_id,date);
  //在blog_system表中的主键是自增的所以设置成了null 
    printf("sql:%s\n",sql);//打印出sql语句中的内容


    //5.让数据库 服务器 执行 SQL
    int ret=mysql_query(connect_fd,sql);

    if(ret<0){
        printf("执行 sql 失败!%s\n",mysql_error(connect_fd));
        mysql_close(connect_fd);
        //在异常退出前也需要关闭数据库
        return 1;
    }
    
    printf("插入成功博客!\n");
   //6.断开连接
   
    char _sql[1024*4]={0};
    char user_name[]="jack";
    char user_password[]="12345";
    sprintf(_sql,"insert into user_table values(null,'%s','%s')",user_name,user_password);
    printf("_sql:%s\n",_sql);

    ret=mysql_query(connect_fd,_sql);
    mysql_close(connect_fd);
    printf("执行成功!\n");
    return 0;
}
