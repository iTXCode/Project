在复杂的网络环境中,你所收到的文件可能在传输的过程中已经被别人篡改过.而收到消息之后,没有验证其正确性就直接拿过来使用导致的后果可能是不堪设想的.
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200825204835744.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)


那么我们怎么才能知道文件在传输的过程中是否被人修改过呢？
- 我们可以通过 ==数字签名== 的方式来验证用户的身份.当然也可以通过数字签名的方式来验证某个二进制文件在传输过程中是否被别人修改过.

 - [ ] 下述故事只是为了说明该种情况.

小时候特别喜欢玩手机游戏,但是总会出现游戏中的装备太差不能过关的现情况.于是我总会想着下载==破解版== 的游戏.😏。这样我就可以买到任何自己想要的装备了,但是当你下载完破解版的游戏,往手机上装载的时候.在安装界面就会出现一串红色的文字,提示你说该应用签名不一致,请谨慎安装.

那么问题来了,在这个过程中手机端是怎么检查到这个app被修改过的呢？
这个检测过程其实就是对比原始文件的数字签名和收到的文件的签名是否一致,若一致就说明没有被修改过,如果不一致的话就说明该文件已经被篡改过,使用过程中存在风险.


#### 数字签名
- [ ] 数字签名:

就是只有信息的发送者才能产生的别人无法伪造的一段数字串.
- [ ] 数字签名的功效
- 1.确定消息确实是由发送方签名并发出来的,因为别人不能假冒发送方才持有的数字签名
- 2.数字签名能确定消息的完整性,因为数字签名的特点是它代表了文件的特征,文件如果发生改变,数字摘要的值也将发生变化.不同的文件将得到不同的数字摘要.

- [ ] 常用的数字签名方法

1.HASH算法

- 此算法主要包括MD(信息摘要)SHA算法(安全散列算法)

2.Digital Signature Algorithm (DSA)， ECDSA (Elliptic Curve Digital Signature Algorithm)，椭圆曲线数字签名算法，微软产品的序列号验证算法使用的就是ECDSA与传统的数字签名算法相比，速度快，强度高，签名短

#### 计算机签名MD5
MD5是由Ron Rivest在1991设计的一种信息摘要(message-digest )算法，当给定任意长度的信息，MD5会产生一个固定的128位“指纹”或者叫信息摘要。从理论的角度，所有的信息产生的MD5值都不同，也无法通过给定的MD5值产生任何信息，即不可逆。

#### MD5功能特点
- 1.输入任意长度的信息，经过处理，输出为128位的信息（数字指纹）   
- 2.不同的输入得到的不同的结果（唯一性）。要使两个不同的信息产生相同的摘要，操作数量级在2^64次方。
- 3.根据128位的输出结果不可能反推出输入的信息。根据给定的摘要反推原始信息，它的操作数量级在2^128次。
#### 算法步骤
##### 添加填充位
- 信息的最尾部(不是每一块的尾部)要进行填充,使其最终的长度length(以bit为单位)满足lenght%512=448,这一步在计算MD5值的时候必须要执行,即使信息的原始长度恰好符合上述要求.

==填充的规则:第一个bit填充位填'1'，后续的bit填充位都填'0'，最终使消息的总体长度恰好符合上述要求.总之,至少要填充1bit,至多填充512bit==

```cpp
void MD5::calculateMD5Final() {
	//函数用于对整体文件数据的最后位数进行填充
	// lastByte： 小于64个字节，表示最后一块数据大小
	unsigned char* p = _chunk + _lastByte;
	//p记录需要填充的起始位置
	//填充位的前八位 : 1000 0000 0x80 
	*p++ = 0x80;
	size_t remainFillByte = _chunkByte - _lastByte - 1;//判断需要填充的位数
	// 剩余填充字节数 小于8 个字节
	if (remainFillByte < 8) {
		// 从下一个自己二开始 全部填 0
		memset(p, 0, remainFillByte);
		calculateMD5((size_t*)_chunk);
		memset(_chunk, 0, _chunkByte);
		//将新出现在空间的中的512位全部置为0
		//最后将数组中的后8位置为文章长度
	}
	else {
		// 填充 0 后 剩余字节数 大于等于 8 个字节  8个字节前 中间全部填 0 
		memset(p, 0, remainFillByte);
	}
	// 最后的64 bit 存放原始文档的bit长度

	// char 类型 最大范围 256 最终类型 还是要以long long 类型存放
	//_chunk = _totalByte * 8;
	((unsigned long long*) _chunk)[7] = _totalByte * 8;
	//最后的字节存放文档的长度
	calculateMD5((size_t*)_chunk);

}

```

##### 添加bit长度
- 在增加填充位之后，添加一个64bit大小的length，length表示原始消息(未填充之前)的bit长度极端情况，如果消息长度超过2^64， 那么只使用前2 ^ 64bit的消息。

- 这一步完成之后，消息的最终长度变为(length + 64) % 512 = 0，即length为512的整数倍
- 从这里再去看第一步，至少需要填充 8 bit，我们假设几种情况分析一下：
首先要明确一个字符占1byte(8bit, 中文字符的话占16bit)，==所以原始信息bit长度一定是8的倍数==。

**需要注意必须:1.必须要进行填充,2.最后一块数据的最后64位必须是原始文件的长度值**

- [ ] 假设1:消息原始长度%512=448

这时候原始长度符合要求,但是根据填充规则,仍然要至少填1bit的 '1' ,后面还剩63bit,不够规定的64位用来添加长度,所以需要再添加一块数据(512bit),这样后面63bit填0,新加的数据前448bit需要填充位0.后面的64位用来填数据原始长度,**一定要记住长度值是放在最后一块数据的最后64bit** 

![在这里插入图片描述](https://img-blog.csdnimg.cn/2020090215081253.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

最后一块数据的大小刚好为448的时候,由于需要对最后一块信息进行填充,所以添加一个bit的‘1’,在此之后剩余的63位少于规定的最后64bit用来填充原始数据长度,所以选择再添加一块大小为512bit的数据块用来计算MD5值
- [ ] 假设2:消息原始长度%512<448

此时最后一块数据长度不大于440bit,最后64bit填数据长度可以用来存储原始数据的长度.由于需要填充的bit位不小于8,所以需要在真实的信息数据之后添加一个‘1’ 和若干个‘0’

![在这里插入图片描述](https://img-blog.csdnimg.cn/2020090215165291.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)
- [ ] 假设3:消息原始长度%512>448

此时最后一块 数据长度不小于456,最多504,剩余bit不够添加64位长度,和假设1相同,需要增加一块数据,最后64位添加长度,其余填充0


![在这里插入图片描述](https://img-blog.csdnimg.cn/20200902152000746.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

**结论：最少填充8bit，最少填充内容1000 0000，16进制即为0x80。**

##### 初始化MD buffer

用4个word buffer(A,B,C,D)存储计算的结果,这里A,B,C,D各为一个32bit的变量,这些变量初始化为下面的十六进制值,低字节在前

```cpp
/*
word A: 01 23 45 67
word B: 89 ab cd ef
word C: fe dc ba 98
word D: 76 54 32 10
*/
// 初始化A,B,C,D
_atemp = 0x67452301;
_btemp = 0xefcdab89;
_ctemp = 0x98badcfe;
_dtemp = 0x10325476;
```
> 处理过程中要用一个含有64个元素的表K[1......64]，表中的元素值由sin函数构建，K[i]等于2^(32) * abs(sin(i))的整数部分，即：

```cpp
/*
K[i] = floor(2^(32) * abs(sin(i + 1))) // 因为此处i从0开始，所以需要sin(i + 1)
*/
for (int i = 0; i < 64; i++)
{
	_k[i] = (size_t)(abs(sin(i + 1)) * pow(2, 32));
}
```
- [ ] 代码

```cpp
void MD5::init(){
	// 初始化 ABCD
	_a = 0x67452301;
	_b = 0xefcdab89;
	_c = 0x98badcfe;
	_d = 0x10325476;
	
	//用来记录每次循环左移所需要移动的位数
	size_t s[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7,
		12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
		4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10,
		15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };
	// TODO

	memcpy(_sft, s, sizeof(s));
	// k[i] 2^32 * abs(sin(i))
	for (int i = 0; i < 64; ++i) {
		_k[i] = (size_t)(pow(2, 32)*abs(sin(i + 1)));
	}
}
```

##### 按512位数据逐块处理输入信息
512bit数据为一个处理单位，暂且称为一个数据块chunk，每个chunk经过4个函数(F, G, H, I)处理,这四个函数输入为3个32位(4字节)的值，产生一个32位的输出。四个函数如下所示：

```cpp
/*
F(x,y,z) = (x & y) | ((~x) & z)
G(x,y,z) = (x & z) | ( y & (~z))
H(x,y,z) = x ^ y ^ z
I(x,y,z) = y ^ (x | (~z))
*/
```

- [ ]  代码

```cpp
	size_t F(size_t x, size_t y, size_t z) {
		return (x & y) | ((~x) & z);
	}
	size_t G(size_t x, size_t y, size_t z) {
		return (x & z) | (y & (~z));
	}
	size_t H(size_t x, size_t y, size_t z) {
		return x ^ y ^ z;
	}
	size_t I(size_t x, size_t y, size_t z) {
		return y ^ (x | (~z));
	}
```

##### 数据处理
在处理一个chunk(512bit)的数据时,会把这个chunk再细分成16组4字节数据，一个chunk经过4轮进行处理，每轮都会把chunk的所有数据处理一遍，每轮有16个相似的子操作，所以一个chunk的数据要进行64个子操作。
计算之前先保存MD buffer的当前值：
a = A, b = B, c = C, d = D
第一轮：F函数处理（0 <= i <= 15）

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200902154019329.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200902154126652.png#pic_center)

```cpp
F = F(b, c, d)
d = c
c =b
b = b + shift((a + F + k[i] + chunk[g]), s[i])
a = d
```
第二轮：G函数处理 (16 <= i <= 31)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200902154230935.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200902154241816.png#pic_center)

```cpp
G = G(b, c, d)
d = c
c =b
b = b + shift((a + G + k[i] + chunk[g]), s[i])
a = d
```
第三轮：H函数处理(32 <= i <= 47)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200902154321652.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200902154333115.png#pic_center)

```cpp
H = H(b, c, d)
d = c
c =b
b = b + shift((a + H + k[i] + chunk[g]), s[i])
a = d
```
第四轮：I函数处理(48 <= i <= 63)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200902154420644.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200902154430408.png#pic_center)

```cpp
I = I(b, c, d)
d = c
c =b
b = b + shift((a + I + k[i] + chunk[g]), s[i])
a = d
```

**图中的<<<s表示循环左移操作**，每次左移的位数，在算法中也是和i一一对应的，也就是我们这的shift表示的含义。

```cpp
/*
s[ 0..15] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22 }
s[16..31] = { 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20 }
s[32..47] = { 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23 }
s[48..63] = { 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 }
*/
size_t s[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7,
12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10,
15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };
```

从上图中可以看出g和i也存在一种确定的对应关系，关系如下：

```cpp
if (0 <= i < 16) g = i;
if (16 <= i < 32) g = (5 * i + 1) % 16;
if (32 <= i < 48) g = (3 * i + 5) % 16;
if(48 <= i < 63) g = (7 * i) % 16;
```
一个chunk数据处理完之后，更新MD buffer的值A, B, C, D

```cpp
A = a + A;
B = b + B;
C = c + C;
D = d + D;
```
- [ ] 代码实现

```cpp
void MD5::calculateMD5(size_t* chunk) {
	//建立大体框架
	//函数用于将所得的内容数据块进行分批次操作

	size_t a = _a;
	size_t b = _b;
	size_t c = _c;
	size_t d = _d;
	// chunk[g]
	// f 是 哈希函数返回值 
	size_t f, g;

	// 64 次变换， 4轮操作，每一轮操作：16个子操作

	for (size_t i = 0; i < 64; ++i) {
		if (0 <= i && i < 16) {
			f = F(b, c, d);
			g = i; 
		}
		else if (16 <= i && i < 32) {
			f = G(b, c, d);
			g = (5 * i + 1) % 16;
		}
		else if (32 <= i && i < 48) {
			f = H(b, c, d);
			g = (3 * i + 5) % 16;
		}
		else {
			f = I(b, c, d);
			g = (7 * i) % 16;
		}

		//更新数据块中的值
		size_t dtmp = d;
		d = c;
		c = b;
		b = b + shiftLeftRotate(a + f + _k[i] + chunk[g], _sft[i]);
		//f哈希函数的返回值,_chunk[g]当前用到的数据块，
		//_sft[i] 循环左移的位数
		a = dtmp; //a=没有更新前的d（dtmp暂存之前的d值)
	}

	_a += a;
	_b += b;
	_c += c;
	_d += d;
}
```

循环左移的操作

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200902190021579.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)
- [ ] 循环左移函数
```cpp
	size_t shiftLeftRotate(size_t num, size_t n) {
		return (num << n) | (num >> (32 - n));
		//循环左移n位
	} 
```

- [ ] 将四个buffer中的数据转换成16进制的字符串

```cpp
std::string MD5::changeHex(size_t num) {
	//用来将对应的整形数组转成16进制字符串形式
	static std::string strMap = "0123456789abcdef";
	std::string ret;
	std::string byteStr;
	for (int i = 0; i < 4; ++i) {
		byteStr = "";
		size_t b  = (num >> (i * 8)) & 0xff;
		for (int j = 0; j < 2; ++j) {
			byteStr.insert(0, 1, strMap[b % 16]);
			b /= 16;
		}
		ret += byteStr;
	}
	return ret;
}
```

#### 摘要输出
这一步拼接4个buffer(A，B，C，D)中的摘要信息，以A中的低位字节开始，D的高位字节结束。最终的输出是128bit摘要信息的16进制表示，故最后输出一个32长度的摘要信息。

```cpp
//比如一个数，它的16进制表示为： 0x23456789, 
//他所对应的8个摘要信息为从低位字节的89开始，高位字节的23结束，即： 89674523
std::string MD5::getFiledMDd5(const char* filename){
	//读取文件
	std::ifstream fin(filename, std::ifstream::binary);
	if (fin.is_open()) {
		while (!fin.eof()) {
			fin.read((char*)_chunk, _chunkByte);
			if (_chunkByte != fin.gcount())
				break;

			_totalByte += _chunkByte;
			calculateMD5((size_t*)_chunk);
			//计算当前资源的MD5值
		}
		_lastByte = fin.gcount();
		_totalByte += _lastByte;
		calculateMD5Final();
	}

	return (changeHex(_a) + changeHex(_b) + changeHex(_c) + changeHex(_d));
}
```

#### 白盒检测

```cpp
用windows自带工具验证读取文件时MD5代码的正确性：
CertUtil -hashfile "文件路径" MD5
```
- 检测结果

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200902195558327.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2phY2tfd2FuZzEyODgwMQ==,size_16,color_FFFFFF,t_70#pic_center)


