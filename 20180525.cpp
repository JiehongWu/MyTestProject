// 20180525.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

/*
greater_equal
less_equal

*/

#include <iostream>  // C++的I/O 输入和输出
#include <vector>    //包含C++ STL向量容器的代码  vector
#include <time.h>
//包含C++ STL函数对象的代码greater less二元函数对象
#include <functional>
#include <algorithm> //包含C++ STL的泛型算法实现  sort  find
#include <iterator>
using namespace std;

/*
C++19 
泛型算法：  =  模板  +  迭代器  +  函数对象
vector 向量容器
底层数据结构
内存增长方式
增删改查方式

vec.reserve保留 预留，解决vector初始内存操作效率不高的问题
vec.resize resize

vec.swap效率高不高？
1.自己和自己交换，do nothing
2.判断两个容器的allocator是否一样？（内存管理方式是否一样），只交换了成员变量
3.两个容器的allcator不一样，那效率非常的低

vec.size():元素的个数

vec[index]：提供了operator[]函数

vec.empty():判空

腾讯原题：gcc能不能编译C++文件代码，g++能不能编译C文件代码，它俩有什么区别？
gcc 链接的是C库
g++ 链接的是C++库   pakages  *.rpm    g++ xxxxxx




C++ 19节课
函数对象
C++ STL 库比较元素大小相关的函数对象，只有二元的函数 对象
但是泛型算法，有的只需要一元函数对象

二元函数对象 + （） ======》 一元函数对象
C++ STL库里面 函数对象的另外一个应用
绑定器  都是绑定高元函数对象     高元的函数对象+绑定器 =》低元的函数对象
bind1st =》 把二元函数对象的operator()函数的第一个参数进行绑定
bind2nd =》 把二元函数对象的operator()函数的第二个参数进行绑定
取反器
not1 =》 给一元函数对象取反  ture -> false
not2 =》 给二元函数对象取反  false -> true


push_back push_front insert
迭代器：
正向迭代器（从前往后遍历）    iterator vec.begin() vec.end()
反向迭代器（保留，从后往前遍历） reverse_iterator vec.rbegin() vec.rend()
插入型迭代器  =>  用在泛型算法当中的，可以通过迭代器给容器添加元素
前插型迭代器  =》  push_front
后插型迭代器  =》  push_back
按位置插入型迭代器 =》 insert
流迭代器  =》  用在泛型算法当中的
输入流迭代器 istream_iterator
输出流迭代器 ostream_iterator


请问vector和list有什么区别？他们各自的应用场景是什么？

C++ STL  
容器

顺序容器
vector:向量容器   随机访问 vec[i] O(1)

reserve
resize


deque:双端队列容器     deque底层的内存是不是连续的？ 不是连续的
底层数据结构:动态开辟的二维数组，一维默认的MapSize是8，二维整数的话是4
永远把二维放在一维中间的下标开始存放，主要为了支持deque的头尾增删都方便，效率高
push_back(val)   O(1)
push_front(val)  O(1)
insert(it, val)  O(n) 

pop_back() O(1) 需要先查找O(n)
pop_front() O(1)
erase(val) O(1) 需要先查找O(n)
erase(it)  O(1) 需要先查找O(n)


list：双向链表容器   head
底层数据结构：双向链表结构
插入元素:
push_back(val)   O(1) 需要先查找O(n)    
push_front(val)  O(1)
insert(it, val)  O(1) 需要先查找O(n)

pop_back() O(1) 需要先查找O(n)
pop_front() O(1)
erase(val) O(1) 需要先查找O(n)
erase(it)  O(1) 需要先查找O(n)

请大家在代码上测试list容器的splice方法（不涉及任何内存开辟释放，对象构造析构）

swap

容器适配器
stack:栈


queue：队列


priorit_queue：优先级队列


关联容器（有序集合）=》  底层数据结构：红黑树  增删查O(log2n)
set：集合
multiset：多重集合

map：映射表
multimap：多重映射表

（C++ STL  =》 Boost库）
无序容器         =》  底层数据结构：哈希表  增删查趋近于O(1) 哈希冲突/哈希碰撞
unordered_set：无序集合
unordered_multiset：无序多重集合

unordered_map：映射表
unordered_multimap：多重映射表

*/

//大树的加减法操作
class BigInt
{
public:
	BigInt(String str) :value(str){}
	BigInt operator+(const BigInt &src);
	BigInt operator-(const BigInt &src);
private:
	string value;
};
ostream& operator<<(ostream &out, const BigInt &src);
int main()
{
	/*
	考虑负数情况   请使用vector容器解决该问题
	先提示用户输入两个长串数字
	87596278346578263497856278936457623784582345
	      56234785627836528634126836326478569823
	*/

	BigInt int1(str1);
	BigInt int2(str2);
	BigInt result = int1 + int2;
	cout << result << endl;

	BigInt int3 = int1 - int2;
	cout << int3 << endl;

	return 0;
}


/*
1.请输出showContainer函数模板，通过迭代器，打印容器的元素值
*/
template<typename Container>
void showContainer(Container &con)
{
	Container::iterator it = con.begin();
	for (; it != con.end(); ++it)
	{
		cout << *it << " ";
	}
	cout << endl;
}
int main(int argc, char* argv[])
{
	//2.用int实例化一个vec容器
	vector<int> vec;
	vector<int> vec2;

	vec.swap(vec2);

	//vec.reserve(100);  //预留100个位置的内存空间
	//cout << vec.size() << endl;
	
	//vec.resize(100);
	//cout << vec.size() << endl;
	
	//vector<int> vec1(100, 0);
	//cout << vec1.size() << endl;

	//3.以系统时间种随机数种子
	srand(time(0));

	//4.给vec容器添加20个0-100之间的随机数
	for (int i = 0; i < 20; ++i)
	{
		vec.push_back(rand() % 100 + 1);
	}

	//5.用showContainer打印容器
	showContainer(vec);

	//6.默认小到大排序并且showContainer打印验证
	sort(vec.begin(), vec.end());
	showContainer(vec);

	//7.从大到小排序vec的所有整数，并且showContainer打印验证
	sort(vec.begin(), vec.end(), greater<int>());
	showContainer(vec);

	//8.在vector中，找50这个元素是否存在；如果存在，删除该元素
	vector<int>::iterator it = find(vec.begin(), 
		vec.end(), 50);
	if (vec.end() == it)
	{
		cout << "not found!" << endl;
	}
	else
	{
		cout << "found 50!" << endl;
		vec.erase(it);
	}

	//9.showContainer打印验证
	showContainer(vec);

	//10.循环遍历vec，删除里面所有的偶数 showContainer打印验证
	//  iterator it = insert(it, val)  返回新插入位置元素的迭代器
	//  iterator it =  erase(it) ++it  返回删除位置的迭代器
	vector<int>::iterator it1 = vec.begin();
	while (it1 != vec.end())
	{
		if (*it1 % 2 == 0)
		{
			it1 = vec.erase(it1);
		}
		else
		{
			++it1;
		}
	}
	showContainer(vec);

	//11.循环遍历vec，给所有奇数前面增加一个小于奇数1的值  showContainer打印验证  16 17    18 19
	vector<int>::iterator it2 = vec.begin();
	for (; it2 != vec.end(); ++it2)
	{
		if (*it2 % 2 != 0)
		{
			//insert完，通过返回值更新it2 指向新插入位置的迭代器
			it2 = vec.insert(it2, *it2 - 1);
			++it2;
		}
	}
	showContainer(vec);

	//面试的问题：同学，你知道什么叫迭代器失效问题吗？

	//12。在大到小序列的vector容器中，找第一个小于70的数字
	//找到，打印出来该数字，然后删除，找不到，打印not found!

	//13.反向遍历容器
	vector<int>::reverse_iterator rit = vec.rbegin();
	for (; rit != vec.rend(); ++rit)
	{
		cout << *rit << " ";
	}
	cout << endl;

	//14.默认构造一个新的vector，通过copy泛型算法，把vec容器的
	//所有元素都拷贝给新的vector
	vector<int> vec1;
	copy(vec.begin(), vec.end(), back_inserter(vec1));
	showContainer(vec1);


	//15.向控制台打印vec1容器的元素的值  ostream_iterator
	copy(vec1.begin(), vec1.end(), ostream_iterator<int>(cout, ","));
	cout << endl;
	//  vec.push_back()    cout<<val

	//16.提示用户输入一组整数，排序，输出
	vector<int> vec3;
	copy(istream_iterator<int>(cin),
		istream_iterator<int>(), inserter(vec3, vec3.end()));
	sort(vec3.begin(), vec3.end());
	copy(vec3.begin(), vec3.end(), ostream_iterator<int>(cout, " "));

	return 0;
}

