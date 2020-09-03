---
title: 博客系统
date: 2019-08-10 09:47:14
tags:
- 博客系统
categories:
- Linux
---

#### 需求：

- [x] 通过自己所学的知识和一些前端的知识,实现一个简单的博客管理系统,用户可以通过网页端进行博客的添加、删除、修改和查询博客.还可以通过标签的方式对博客进行分类和管理,我们也可以对标签进行添加、删除、修改和查询.

> 对于普通用户来说,进入网页端之后只能看到博客的标题和执行内容,不能对其进行修改等操作,当用户进行登录之后,就可以点击博客标题进修改了,可以新增,删除和修改。

#### 开发平台和环境

- 开发语言:Cpp+Mysql+css+vue.js+Javascript+html

- 开发平台:Linux CentOS 7.0

- 开发工具:vim代码编辑器,gcc编辑器,makefile工程管理工具,gdb调试器,VSCode

#### 需求分析

- 1.只支持单个用户

- 2.实现针对文章的增删改查

- 3.实现针对标签的增删改查

- 4.用户登录

#### 模块划分

##### 客户端:网页的形式呈现

![](https://i.loli.net/2019/08/10/a8PmhEczWfGkeKw.png)

用服务器端操纵数据库,客户在网页端执行某个操作之后,由客户端给服务器发送HTTP请求,请求中包含着用户行为,HTTP服务器再将JSON格式的数据进行解析,服务器再根据这个行为来完成对数据库的操作

##### 网页端的具体展现形式

- 1.展现博客列表页面

- 2.展现博客详情页面

- 3.管理博客页面

>  例如：当用户在客户端(网页上)执行某个操作的时候,比如新增博客,此时就会由客户端给服务器发送HTTP请求,请求中就包含了用户的行为,HTTP服务器再根据这个行为来完成对数据的操作(数据库的操作 )

##### 设计思路

- 1.先设计数据库(表结构)

- 2.设计前后端交互接口

- 3.实现服务器端和客户端的逻辑

##### 数据库设计(MySQL关系型数据库)

- 1.创建数据库(blog_system)

- 2.创建博客表(blog_table)

> a.一个主键(blog_id int)
> 
> b.一个标题（title_id  varchat(50))
> 
> c.正文(content text)
> 
> d.creat_time  varchar(50)
> 
> e.标签(tag_id  int )(此处属于一个便签)

- 3.创建一个标签表(tag_table)

> a.tag_id(外键)  int
> 
> b.tag_name   varchar(50)

- 4.创建用户表(user_table)

> a.user_id;
> 
> b.user_name;
> 
> c.user_password;

- [ ] 分布式系统下生成唯一主键的方式

- 为了提高效率,牺牲了数据的强一致性

- [ ] 第三方工具尽量使用yum安装.

- 1.使用时间戳

- 2.使用机房id

- 3.使用主机ip

- 4.使用随机数

- 数据库对Mysql 的默认端口号

` netstat -anp| grep 3306//查看数据库默认的端口`

##### 对数据库的操作

**使用C语言中提供的MySQL API来完成对数据库的操作**

> MySQL官网文档
> 
> [MySQL :: MySQL Documentation](https://dev.mysql.com/doc/)

- [ ] 头文件 #include < mysql/mysql.h >

```sql
MYSQL* mysql_init(MYSQL* mysql);
```

> 若这里的参数写为**NULL**,就返回一个句柄.空需要写成NULL不能写成nullptr

- [ ] 连接数据库的函数

```sql
MYSQL *mysql_real_connect(
MYSQL *connect_fd, const char *host, 
const char *user, const char *passwd,
 const char *db, unsigned int port,
 const char *unix_socket, 
unsigned long client_flag) --(8 个参数！！！！)
```

- 参数
  
  - connect_fd:刚才的mysql_init()创建的句柄
  
  - host:要连接的服务器ip              
  
  - user：数据库用户名
  
  - passwd：数据库对应用户名的密码
  
  - db：database 名
  
  - port：数据库服务器端口名，默认 3306
  
  - unix_socket：NULL
  
  - client_flag：0

- **返回值**

> 如果连接失败返回NULL,根据返回结果打印日志,提醒用户连接状况
> 
> 如果连接成功,则返回一个与刚才一样的句柄

- [ ] **设置字符编码规则**

```sql
int mysql_set_character_set(MYSQL *mysql, const char *csname)
```

- 例如： `例如： mysql_set_character_set(connect_fd, "utf8");`

> 设置默认字符集为utf8

- [ ] 执行一个拼装的SQL语句

```sql
int mysql_query(MYSQL* mysql,const char* sql);
```

> 这里主要使用sprintf() 函数将SQL语句拼接到sql中,然后把 sql传进句柄mysql中

- **遍历查询结果**

- [ ] 遍历查询结果

```sql
MYSQL_RES *mysql_store_result(MYSQL *mysql)
```

> 将刚才 mysql 句柄查询的结果放进 MYSQL_RES 这个结构体中。

```sql
my_ulonglong mysql_num_rows(MYSQL_RES *result)
```

> 获取结果集合的行数

```sql
unsigned int mysql_num_fields(MYSQL_RES *result)
```

> 获取结果集合的列数

```sql
MYSQL_ROW mysql_fetch_row(MYSQL_RES *result)
```

> 一次获取一行数据

```sql
mysql_free_result()
```

> 释放查询结果集合

```sql
void mysql_close(MYSQL *mysql)
```

> 将刚才连接关闭，因为在某种情况下，就算程序退出，连接也不一定会一定关闭，所以一定要手动关闭。

```sql
unsigned long mysql_real_escape_string(MYSQL *mysql, 
char *to, const char from, 
unsigned long length)
```

- 参数
  
  - 将正文进行转义，防止正文出现特殊符号从而导致 SQL 语句出错
  
  - mysql 句柄
  
  - to 是动态开辟的一个空间，是 from.size() * 2 + 1
  
  - from 是正文地址
  
  - length 是 from.size() 的大小

### 设计前后端的交互 API

##### 基于HTTP 协议进行扩展,创建自定制协议

- [ ] 需要考虑传输交互那些数据
- [ ] 这些信息如何进行序列化和反序列化

#### 一：博客管理

- 1.新增博客

- 2.获取博客列表

- 3.获取某个博客的详细内容

- 4.修改博客

- 5.删除博客

#### 二：标签管理

- 1.新增标签

- 2.删除标签

- 3.查看所有标签

#### 新增博客设计风格

1.传统的 API　设计方式　：使用query_string 来进行信息传递

- [ ] 例如: 

- GET请求:GET/blog?method=insert&title=....&contnet=....

###### restful 风格的API设计方式 ：使用不同的 HTTP方法来表达不同的语义

- [x] 1.使用POST方法表示新增博客

- [x] 2.使用GET 方法表示查看博客

- [x] 3.使用PUT 方法表示修改博客

- [x] 4.使用DELETE 方法表示删除博客 

- [x] 5.使用path表示要操作的资源

### 三:具体操作

`通过json的方式来完成自定制协议`

#### 新增博客

- [ ] 请求

```json
POST/blog{
    "title":"xxx",
    "content":"xxx",
    "create_time":"xxx",
    "tag_id:xxx",
}
```

- [ ] 响应

```json
HTTP/1.1 200 ok
{
    "ok":"true/false",
    "reason":"错误原因"
}
```

#### 查看所有博客(标题列表)

- 展示博客的列表

- [ ] 请求(使用 tag_id 参数筛选所指定标签的博客)

```json
 GET/blog  获取所有博客
 GET/blog?tag_id=1 按照标签来筛选指定的博客
```

- [ ] 响应

```json
HTTP/1.1 200 OK
[
    {
        "blog_id":1,
        "title":"我的第一篇博客",
        "content":"博客正文",
        "create_time":"2020/07/28",
        "tag_id":1
    },
    {
       "blog_id":2,
        "title":"我的第二篇博客",
        "content":"博客正文",
        "create_time":"2020/08/01",
        "tag_id":1
    }
]
```

#### 查看某个博客

- 展示某一篇博客的详细内容

- [ ] 请求

```json
GET /blog/:blog_id(具体的某个博客的id)   --> 类似于 /blog/1314
```

- [ ] 响应

```json
HTTP/1.1 200 OK
{
    "blog_id":1,
    "title":"我的第一篇博客",
    "content":"博客正文",
    "create_time":"2019/07/28",
    "tag_id":1
}
```

#### 删除博客

> 删除id为指定id的博客,

- [ ] 请求

```json
DELETE /blog/blog_id
```

- [ ] 响应

```json
HTTP/1.1 200 OK
{
    "ok":"true",
    "no":"false"
}
```

#### 修改博客

> 修改id为指定id的博客

- [ ] 请求

```json
PUT /blog/blog_id
{
    "title":"修改之后的标题",
    "content":"修改之后的正文",
    "modify_time":"修改时间",
    "tag_id":"修改之后的tag_id"
}
```

- [ ] 响应

```json
HTTP/1.1 200 OK
{
    "ok":"true"
}
```

### 标签

#### 新增标签

- [ ] 请求

```json
POST /tag
{
    "tag_name":"新增的标签名"
}
```

- [ ] 响应

```json
HTTP/1.1 200 OK
{
    "ok":"true"
}
```

#### 删除标签

> 删除id为指定id的博客

- [ ] 请求

```json
DELETE /tag/:tag_id
```

- [ ] 响应

```json
HTTP/1.1 200 OK
{
    ok:true
}
```

#### 查看所有标签

> 查看标签列表 

- [ ] 请求

```json
GET /tag
```

- [ ] 响应

```json
HTTP /1.1 200 OK
[
    {
        "tag_id":1,
        "tag_name":"cpp",  
    },
    {
        "tag_id":2,
        "tag_name":"mysql"
    }
]
```

### 代码编辑

#### 使用到的工具

- [ ] Postman:用于测试http相关服务
- [ ] 网页模板网站:模板之家、amaze UI
- [ ] Vue.js用于书写客户端代码
- [ ] Javascript,css,html

#### 第三方库

- 1.httplib.h:一个C ++ 11单文件纯标题跨平台HTTP / HTTPS库

[httplib.h](https://github.com/yhirose/cpp-httplib)

- 2.Editor.md:第三方库的JS的库,完成makedown和html相互转换,以及makedown在线编辑器的功能
- 3.jquery ajax:用于从网络上将数据库中的数据获取之后,显示在屏幕上

#### 回归测试.

- 遇到bug之后,需要进行 定位和修改.修改完成之后一定要再测试一边,保证修改时生效的，并且没有引入其他问题
- 回归测试到底测那些?只测试当前逻辑,还是之前的所有逻辑？一定都要测
- 回归测试往往和自动化测试时相关的

#### sprintf 来拼接SQL语句

- [ ] 存在的问题

> '%s',content ==>本来就包含了' 怎么办？

- [ ] 例如 content =>My name is 'lingfeng'

```sql
insert into blog_table values(&#39;Myname is &#39;lingfeng&#39;&#39;);
```

> 该语句中存在着字符串识别的问题

**解决办法**

```cpp
mysql_real_escape_string(); //可以进行正确的转义
```

- [ ] 到Mysql官网去查看官方文档了解其正确用法

### 正文长度和sql语句长度未知所以需要动态分配

**当接收到的参数为字符串时转成整数用stoi**

- [ ] stoi 将c风格的字符串转成整数型数字
- [ ] atoi将C风格的字符串转成整数C+++ 11 提供的功能

### 测试

**自己的代码先进行单元测试,避免所有代码都写完了,才去测试**

使用Postman测试结果截图

- [ ] 插入博客

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200829092114998.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

- [ ] 查看所有博客

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200829092204760.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

- [ ] 查看指定博客

- 失败

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200829092240272.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

- 成功

![在这里插入图片描述](https://img-blog.csdnimg.cn/2020082909233869.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

- [ ] 更新某篇博客

- 成功

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200829092421850.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

- 失败

![在这里插入图片描述](https://img-blog.csdnimg.cn/2020082909251380.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

- [ ] 删除某篇指定的博客

- 成功

- [ ] 插入标签

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200829092735298.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

- [ ] 删除标签

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200829092810877.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

- [ ] 查看标签

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200829092841423.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

- [ ] 用户注册

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200903212912922.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

#### 

#### HTTP服务器

#### HTTP协议

- [ ] 基于TCP服务器,在HTTP协议的格式基础上,来完成字符串的解析和拼装

#### 正则表达式

- [ ] 任何语言里面都有正则表达式的相关操作

> 用特殊符号来描述一个字符串应该具有那些特殊特征.
> 
> 特征包括不限于:包含那些特定字符串,以什么开头
> 
> 以什么结尾,特定字符重复出现几次...

##### HTTP的客户端,构造出POST请求

- [ ] cul 命令行版本的HTTP客户端
- [ ] Postman 大名鼎鼎的http测试工具

####

#### 项目过程中应用的httplib.h出现警告

```
httplib.h:466:85: warning: ‘deprecated’ attribute directive ignored [-Wattributes]
   [[deprecated]] bool set_base_dir(const char *dir,const char *mount_point = nullptr);
```

- [ ] 解决办法:

- 1.首先利用谷歌翻译认识到了警告的大概意思

- 2.百度该警告没有发现类似的问题

- 3.自己在拷贝了源文件的前提下,大胆的进行了更改将警告提示处的[[deprecated]]删除后,问题得到了解决

#### 项目编辑时没有问题运行时出错

```
terminate called after throwing an instance of &#39;std::regex_error&#39;
  what():  regex_error
Aborted
```

- [ ] 解决办法

- 1.在CSDN中查找相关问题,发现有人说时gcc/g++版本问题

- 2.升级后gcc/g++版本后发现问题依然存在

- 3.在Google中查找,最终在git上的一篇文章中发现说是没有MNN应该没有使用**regex**

- 4.在服务器启动代码中加上了头文件 `include&lt;regex&gt;`问题得到解决
  
  **页面刷新过程中先显示代码,在显示固定的内容,最后再显示更新后的内容.(该过程客户可见)**

> 为解决这个问题:采用**遮罩**的办法

- 1.在代码中加上<v-cloak>
- 2.将数据中的默认数据也删除掉

#### 项目设计总体工程

##### 设计

- [ ] 1.数据库设计
- [ ] 2.先后端接口设计

##### 编码

- [ ] 1.封装数据库操作(Model)
- [ ] 2.实现了服务器API(Ctrl lre 层)
- [ ] 3.实现客户端的开发

#### 三剑客(跑在浏览器上)

- [ ] HTML:描述了网页的骨架
- [ ] CSS:描述了网页的样子(大小/颜色/位置/框图)
- [ ] Javascript:描述网页和用户之间的交互动作

#### 浏览器:

> 网页前端技术由于诞生的很早,很多开发方式非常古老和守旧(DOM技术)

#### JQuery JavaScript的最知名的第三方库没有之一

#### 前端框架 :前端工程化的一个过程

- [ ] 1.Angular:谷歌

- [ ] 2.React:Facebook

- [ ] 3.Vue.js:开源社区搞的框架(尤雨溪)

- 中文的文档

- 上手容易

#### 常见的html标签

- [ ] 1.div理解成一个"容器"里面放什么都行,页面布局主要靠div完成

- [ ] 2.span和div功能类似,div默认是独占一行(块级元素),span默认不独占一行(内联元素)

- [ ] 3.a标签表示一个链接.点击a标签会跳转到另外一个页面,通过href属性来表述

- 所有的标签都有class属性,class把当前标签和一个css类关联在一起

- [ ] 4.img表示一个图片,src属性来描述图片对应的地址

- [ ] 5.p标签标识段落,其实和div没啥差别

- [ ] 6.h1-h6表示1级标题到6级标题(每种标题的字体颜色粗细都可以使用css来修改)

###### 各种前端框架最主要的功能是为了方便操作界面

代码往往设计到两个方面

- 1.html中需要配套代码
- 2.js中也需要配套代码.js中执行一些操作,就会影响到html中的一些相关显示内容

### 博客页面

##### 1.博客列表页(给用户看的)

##### 2.博客详情页(editor.md)(给用户看的)

- markdowm

> 提交，存储博客的时候,都可以用markdown格式的数据来表示
> 
> 展示的时候,就自动把markdown数据转成html格式的数据

##### 3.博客管理页(给自己看的页面)

- [ ] 删除博客
- [ ] 编辑博客:

> 点击标题进入编辑页面,编辑页面中应该有一个markdown编辑器,编辑器里的内容已经是博客当前内容,编辑完成之后,点击提交按钮,就把数据发送到服务器上

- [ ] 插入博客

### 当前实现的功能

- [ ] 1.博客的增删改查
- [ ] 2.标签的增删改查(部分)

### 进一步改进

- [ ] 1.完善前面未完成的功能
- [ ] 2.迁移博客,实现一个爬虫程序(HTTP客户端cpp-httplib)把曾经的博客抓取下来,插入到数据库中
- [ ] 3.实现一个图床服务器(HTTP服务器,专门用于存图片)
- [ ] 4.支持多用户,需要数据库表结构重新设计.cookie and session
- [ ] 5.分页展示,
- [ ] 6.用户注册登录界面,身份验证
- [ ] 7.搜索博客功能(做的简单的话,可以直接使用数据库 like,做的复杂的话,"倒排索引")

### 
