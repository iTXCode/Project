#include<cstdio>
#include<cstdlib>
#include<mysql/mysql.h>

int main(){
    //1.创建一个句柄
    MYSQL* connect_fd= mysql_init(NULL);
    //2.建立连接
    if(mysql_real_connect(connect_fd,"127.0.0.1",
    "root","0","blog_system",3306,NULL,
    0)==NULL)//接口返回结果为一个判断结果
    //用户root后的参数是密码
    {
        printf("连接失败!%s\n",mysql_error(connect_fd));
        return 1;
    }
    printf("连接成功!\n");

    //3.设置编码集
    //3.设置编码方式
    //  mysql server 部分最初安装的时候已经设置成了utf8
    //  客户端这边也需要设置为utf8
    mysql_set_character_set(connect_fd,"utf8");

    //4.拼接SQL语句

    char sql[1024*4]="select * from blog_table";
    //5.执行SQL语句
    int ret=mysql_query(connect_fd,sql);
    if(ret<0){
      printf("mysql_query failed! %s\n",mysql_error(connect_fd)); 
      mysql_close(connect_fd);
      return 1;
    }

    //6.遍历结果集合,MYSQL_RES select 得到的结果集合
    MYSQL_RES* result=mysql_store_result(connect_fd);//获取结果集合
    //mysql_store_result所产生的结果集合
    //内部使用malloc申请内存,或许需要手动释放 
    //a.获取到结果中的行数和列数
    int rows=mysql_num_rows(result);//获取行数
    int fields=mysql_num_fields(result);//获取字段数
    //b.根据行数和列数来遍历结果

    for(int i=0;i<rows;++i){
        //一次获取到一行数据
        MYSQL_ROW row=mysql_fetch_row(result);

        for(int j=0;j<fields;++j){
            printf("%s\t",row[j]);
            //访问数据库表中一行中的一列
        }
        printf("\n");
    }

    //释放结果集合,容易遗忘的点
    mysql_close(connect_fd);
    return 0;
}