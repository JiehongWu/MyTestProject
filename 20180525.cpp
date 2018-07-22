// 20180525.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

/*
greater_equal
less_equal

*/

#include <iostream>  // C++��I/O ��������
#include <vector>    //����C++ STL���������Ĵ���  vector
#include <time.h>
//����C++ STL��������Ĵ���greater less��Ԫ��������
#include <functional>
#include <algorithm> //����C++ STL�ķ����㷨ʵ��  sort  find
#include <iterator>
using namespace std;

/*
C++19 
�����㷨��  =  ģ��  +  ������  +  ��������
vector ��������
�ײ����ݽṹ
�ڴ�������ʽ
��ɾ�Ĳ鷽ʽ

vec.reserve���� Ԥ�������vector��ʼ�ڴ����Ч�ʲ��ߵ�����
vec.resize resize

vec.swapЧ�ʸ߲��ߣ�
1.�Լ����Լ�������do nothing
2.�ж�����������allocator�Ƿ�һ�������ڴ����ʽ�Ƿ�һ������ֻ�����˳�Ա����
3.����������allcator��һ������Ч�ʷǳ��ĵ�

vec.size():Ԫ�صĸ���

vec[index]���ṩ��operator[]����

vec.empty():�п�

��Ѷԭ�⣺gcc�ܲ��ܱ���C++�ļ����룬g++�ܲ��ܱ���C�ļ����룬������ʲô����
gcc ���ӵ���C��
g++ ���ӵ���C++��   pakages  *.rpm    g++ xxxxxx




C++ 19�ڿ�
��������
C++ STL ��Ƚ�Ԫ�ش�С��صĺ�������ֻ�ж�Ԫ�ĺ��� ����
���Ƿ����㷨���е�ֻ��ҪһԪ��������

��Ԫ�������� + ���� ======�� һԪ��������
C++ STL������ �������������һ��Ӧ��
����  ���ǰ󶨸�Ԫ��������     ��Ԫ�ĺ�������+���� =����Ԫ�ĺ�������
bind1st =�� �Ѷ�Ԫ���������operator()�����ĵ�һ���������а�
bind2nd =�� �Ѷ�Ԫ���������operator()�����ĵڶ����������а�
ȡ����
not1 =�� ��һԪ��������ȡ��  ture -> false
not2 =�� ����Ԫ��������ȡ��  false -> true


push_back push_front insert
��������
�������������ǰ���������    iterator vec.begin() vec.end()
������������������Ӻ���ǰ������ reverse_iterator vec.rbegin() vec.rend()
�����͵�����  =>  ���ڷ����㷨���еģ�����ͨ�����������������Ԫ��
ǰ���͵�����  =��  push_front
����͵�����  =��  push_back
��λ�ò����͵����� =�� insert
��������  =��  ���ڷ����㷨���е�
������������ istream_iterator
����������� ostream_iterator


����vector��list��ʲô�������Ǹ��Ե�Ӧ�ó�����ʲô��

C++ STL  
����

˳������
vector:��������   ������� vec[i] O(1)

reserve
resize


deque:˫�˶�������     deque�ײ���ڴ��ǲ��������ģ� ����������
�ײ����ݽṹ:��̬���ٵĶ�ά���飬һάĬ�ϵ�MapSize��8����ά�����Ļ���4
��Զ�Ѷ�ά����һά�м���±꿪ʼ��ţ���ҪΪ��֧��deque��ͷβ��ɾ�����㣬Ч�ʸ�
push_back(val)   O(1)
push_front(val)  O(1)
insert(it, val)  O(n) 

pop_back() O(1) ��Ҫ�Ȳ���O(n)
pop_front() O(1)
erase(val) O(1) ��Ҫ�Ȳ���O(n)
erase(it)  O(1) ��Ҫ�Ȳ���O(n)


list��˫����������   head
�ײ����ݽṹ��˫������ṹ
����Ԫ��:
push_back(val)   O(1) ��Ҫ�Ȳ���O(n)    
push_front(val)  O(1)
insert(it, val)  O(1) ��Ҫ�Ȳ���O(n)

pop_back() O(1) ��Ҫ�Ȳ���O(n)
pop_front() O(1)
erase(val) O(1) ��Ҫ�Ȳ���O(n)
erase(it)  O(1) ��Ҫ�Ȳ���O(n)

�����ڴ����ϲ���list������splice���������漰�κ��ڴ濪���ͷţ�������������

swap

����������
stack:ջ


queue������


priorit_queue�����ȼ�����


�������������򼯺ϣ�=��  �ײ����ݽṹ�������  ��ɾ��O(log2n)
set������
multiset�����ؼ���

map��ӳ���
multimap������ӳ���

��C++ STL  =�� Boost�⣩
��������         =��  �ײ����ݽṹ����ϣ��  ��ɾ��������O(1) ��ϣ��ͻ/��ϣ��ײ
unordered_set�����򼯺�
unordered_multiset��������ؼ���

unordered_map��ӳ���
unordered_multimap������ӳ���

*/

//�����ļӼ�������
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
	���Ǹ������   ��ʹ��vector�������������
	����ʾ�û�����������������
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
1.�����showContainer����ģ�壬ͨ������������ӡ������Ԫ��ֵ
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
	//2.��intʵ����һ��vec����
	vector<int> vec;
	vector<int> vec2;

	vec.swap(vec2);

	//vec.reserve(100);  //Ԥ��100��λ�õ��ڴ�ռ�
	//cout << vec.size() << endl;
	
	//vec.resize(100);
	//cout << vec.size() << endl;
	
	//vector<int> vec1(100, 0);
	//cout << vec1.size() << endl;

	//3.��ϵͳʱ�������������
	srand(time(0));

	//4.��vec�������20��0-100֮��������
	for (int i = 0; i < 20; ++i)
	{
		vec.push_back(rand() % 100 + 1);
	}

	//5.��showContainer��ӡ����
	showContainer(vec);

	//6.Ĭ��С����������showContainer��ӡ��֤
	sort(vec.begin(), vec.end());
	showContainer(vec);

	//7.�Ӵ�С����vec����������������showContainer��ӡ��֤
	sort(vec.begin(), vec.end(), greater<int>());
	showContainer(vec);

	//8.��vector�У���50���Ԫ���Ƿ���ڣ�������ڣ�ɾ����Ԫ��
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

	//9.showContainer��ӡ��֤
	showContainer(vec);

	//10.ѭ������vec��ɾ���������е�ż�� showContainer��ӡ��֤
	//  iterator it = insert(it, val)  �����²���λ��Ԫ�صĵ�����
	//  iterator it =  erase(it) ++it  ����ɾ��λ�õĵ�����
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

	//11.ѭ������vec������������ǰ������һ��С������1��ֵ  showContainer��ӡ��֤  16 17    18 19
	vector<int>::iterator it2 = vec.begin();
	for (; it2 != vec.end(); ++it2)
	{
		if (*it2 % 2 != 0)
		{
			//insert�꣬ͨ������ֵ����it2 ָ���²���λ�õĵ�����
			it2 = vec.insert(it2, *it2 - 1);
			++it2;
		}
	}
	showContainer(vec);

	//���Ե����⣺ͬѧ����֪��ʲô�е�����ʧЧ������

	//12���ڴ�С���е�vector�����У��ҵ�һ��С��70������
	//�ҵ�����ӡ���������֣�Ȼ��ɾ�����Ҳ�������ӡnot found!

	//13.�����������
	vector<int>::reverse_iterator rit = vec.rbegin();
	for (; rit != vec.rend(); ++rit)
	{
		cout << *rit << " ";
	}
	cout << endl;

	//14.Ĭ�Ϲ���һ���µ�vector��ͨ��copy�����㷨����vec������
	//����Ԫ�ض��������µ�vector
	vector<int> vec1;
	copy(vec.begin(), vec.end(), back_inserter(vec1));
	showContainer(vec1);


	//15.�����̨��ӡvec1������Ԫ�ص�ֵ  ostream_iterator
	copy(vec1.begin(), vec1.end(), ostream_iterator<int>(cout, ","));
	cout << endl;
	//  vec.push_back()    cout<<val

	//16.��ʾ�û�����һ���������������
	vector<int> vec3;
	copy(istream_iterator<int>(cin),
		istream_iterator<int>(), inserter(vec3, vec3.end()));
	sort(vec3.begin(), vec3.end());
	copy(vec3.begin(), vec3.end(), ostream_iterator<int>(cout, " "));

	return 0;
}

