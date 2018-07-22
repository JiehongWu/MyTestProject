// 20180608.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


/*
C++23次课   智能指针

int *p = new int; 裸指针
裸指针的缺点：
1.必须手动释放资源
2.由于程序逻辑的错误，导致释放资源的代码没有被调用到
3.程序运行过程中，发生异常，导致释放资源的代码没有被调用到

智能指针   智能（不管程序运行发生什么意外的情况，一定会帮用户把资源释放掉）
数据段   堆    栈


所有的智能指针   都需要  考虑怎么解决浅拷贝问题!!!
auto_ptr 最新的auto_ptr智能指针对象引用资源，之前的auto_ptr都失效了


智能指针：
不带引用计数的智能指针(资源没有引用计数)
auto_ptr
scope_ptrauto_ptr 在拷贝（复制）时会从源 auto_ptr 自动交出拥有权，而 scoped_ptr 则不允许被复制（正是这一限制，使我们对scoped_ptr 的使用变得有信心且容易）。
unique_ptr



带引用计数的智能指针(所有的资源都有相应的引用计数)
1.强智能指针   =》   它可以改变资源的引用计数
带引用计数的强智能指针的缺陷：
循环/交叉引用的时候，造成资源无法释放  
怎么解决呢？
      弱智能指针（它不可以改变资源的引用计数）    

shared_ptr和weak_ptr
都是线程安全的  可以直接在多线程环境中使用




*/
#include <iostream>
#include <memory>    // 这个头文件中，包含了C++库所有的智能指针
#include <unordered_map>
using namespace std;

/*
专门用来统计资源的引用计数  void*  int

*/
class ReferenceCount
{
public:
	//给ptr指向的资源增加引用计数
	void incRef(void *ptr)
	{
		refMap[ptr]++;

#if 0
		cout << ptr << endl;
		unordered_map<void*, int>::iterator it = refMap.find(ptr);
		if (it == refMap.end())
		{
			cout << "xxx" << endl;
			refMap.insert(make_pair(ptr, 1));
		}
		else
		{
			++(it->second);
		}
#endif
	}
	//给ptr指向的资源减少引用计数
	int decRef(void *ptr)
	{
		unordered_map<void*, int>::iterator it = refMap.find(ptr);
		if (it == refMap.end())
		{
			return 0;
		}
		else
		{
			int ret = --(it->second);
			if (it->second == 0)
			{
				refMap.erase(it);
			}
			return ret;
		}
	}
private:
	//map的key表示资源地址， value表示资源的引用计数
	unordered_map<void*, int> refMap;
};

template<typename T>
class CSmartPtr
{
public:
	CSmartPtr(T *ptr = NULL)
		:mptr(ptr)
	{
		if (mptr != NULL)
		{
			incRef();
		}
	}
	CSmartPtr(const CSmartPtr<T> &src)
		:mptr(src.mptr)
	{
		if (mptr != NULL)
		{
			incRef();
		}
	}
	CSmartPtr<T>& operator=(const CSmartPtr<T> &src)
	{
		if (this == &src)
			return *this;

		if (decRef() == 0)
		{
			delete mptr;
			mptr = NULL;
		}

		mptr = src.mptr;
		if (mptr != NULL)
		{
			incRef();
		}
		return *this;
	}
	~CSmartPtr()
	{ 
		if (decRef() == 0)
		{
			delete mptr;
			mptr = NULL;
		}
	}

	T& operator*()
	{
		return *mptr;
	}
	const T& operator*()const
	{
		return *mptr;
	}
	T* operator->()
	{
		return mptr;
	}
	T* operator->()const
	{
		return mptr;
	}
private:
	T *mptr;
	//专门供智能指针，查看当前它所引用的资源的引用计数
	static ReferenceCount refCnt;

	void incRef(){ refCnt.incRef(mptr); }
	int decRef(){ return refCnt.decRef(mptr); }
};

template<typename T>
ReferenceCount CSmartPtr<T>::refCnt;

//类的前置声明
class B;
class A
{
public:
	A(){ cout << "A()" << endl; }
	~A(){ cout << "~A()" << endl; }
	void test();
	//资源引用的地方，都采用弱智能指针
	weak_ptr<B> _ptrb;
};
class B
{
public:
	B(){ cout << "B()" << endl; }
	~B(){ cout << "~B()" << endl; }
	void func(){ cout << "call B::func" << endl; }

	//资源引用的地方，都采用弱智能指针
	weak_ptr<A> _ptra;
};


void A::test()
{
	//_ptrb->func(); 
	//那么虽然弱智能指针不会引起资源的引用计数增加，
	//但是它也不能直接访问对象。
	//lock 从弱智能指针   提升  强智能指针
	shared_ptr<B> ptr = _ptrb.lock();
	if (ptr != NULL)
	{
		//就表示提升成功了
		ptr->func();
	}
}

int main()
{
	/*
	生成资源的时候，一定是使用强智能指针
	*/
	shared_ptr<A> ptra(new A());
	shared_ptr<B> ptrb(new B());

	ptra->_ptrb = ptrb;
	ptrb->_ptra = ptra;

	ptra->test();

	return 0;
}




#if 0
int main(int argc, char* argv[])
{
	CSmartPtr<int> ptr1(new int);
	*ptr1 = 30;
	cout << *ptr1 << endl;

	CSmartPtr<int> ptr2(ptr1);
	CSmartPtr<int> ptr3(ptr1);
	ptr2 = ptr3;
	ptr3 = ptr1;


#if 0
	auto_ptr<int> ptr2(new int);
	*ptr2 = 10;
	cout<<*ptr2<<endl;
	auto_ptr<int> ptr3(ptr2);
	*ptr2 = 10;
#endif
	//cout<<*ptr2<<endl;
	//*ptr3 = 20;
	//cout<<*ptr3<<endl;

	return 0;
}

int main(int argc, char* argv[])
{
	CSmartPtr<int> ptr1(new int);
	*ptr1 = 30;
	cout << *ptr1 << endl;

	const CSmartPtr<int> ptr2(new int(40));
	cout << *ptr2 << endl;

	CSmartPtr<A> ptr3(new A());
	(*ptr3).func();

	//ptr3.operator->(func());
	//(ptr3.operator->())->func();
	ptr3->func();

	const CSmartPtr<A> ptr4(new A());
	ptr4->func();

	return 0;
}
#endif
	/*int *p = new int;

	if (xxxx){
		return 0;
	}
	//中间代码抛出异常了

	delete p;*/



