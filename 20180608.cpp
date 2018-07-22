// 20180608.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


/*
C++23�ο�   ����ָ��

int *p = new int; ��ָ��
��ָ���ȱ�㣺
1.�����ֶ��ͷ���Դ
2.���ڳ����߼��Ĵ��󣬵����ͷ���Դ�Ĵ���û�б����õ�
3.�������й����У������쳣�������ͷ���Դ�Ĵ���û�б����õ�

����ָ��   ���ܣ����ܳ������з���ʲô����������һ������û�����Դ�ͷŵ���
���ݶ�   ��    ջ


���е�����ָ��   ����Ҫ  ������ô���ǳ��������!!!
auto_ptr ���µ�auto_ptr����ָ�����������Դ��֮ǰ��auto_ptr��ʧЧ��


����ָ�룺
�������ü���������ָ��(��Դû�����ü���)
auto_ptr
scope_ptrauto_ptr �ڿ��������ƣ�ʱ���Դ auto_ptr �Զ�����ӵ��Ȩ���� scoped_ptr ���������ƣ�������һ���ƣ�ʹ���Ƕ�scoped_ptr ��ʹ�ñ�������������ף���
unique_ptr



�����ü���������ָ��(���е���Դ������Ӧ�����ü���)
1.ǿ����ָ��   =��   �����Ըı���Դ�����ü���
�����ü�����ǿ����ָ���ȱ�ݣ�
ѭ��/�������õ�ʱ�������Դ�޷��ͷ�  
��ô����أ�
      ������ָ�루�������Ըı���Դ�����ü�����    

shared_ptr��weak_ptr
�����̰߳�ȫ��  ����ֱ���ڶ��̻߳�����ʹ��




*/
#include <iostream>
#include <memory>    // ���ͷ�ļ��У�������C++�����е�����ָ��
#include <unordered_map>
using namespace std;

/*
ר������ͳ����Դ�����ü���  void*  int

*/
class ReferenceCount
{
public:
	//��ptrָ�����Դ�������ü���
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
	//��ptrָ�����Դ�������ü���
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
	//map��key��ʾ��Դ��ַ�� value��ʾ��Դ�����ü���
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
	//ר�Ź�����ָ�룬�鿴��ǰ�������õ���Դ�����ü���
	static ReferenceCount refCnt;

	void incRef(){ refCnt.incRef(mptr); }
	int decRef(){ return refCnt.decRef(mptr); }
};

template<typename T>
ReferenceCount CSmartPtr<T>::refCnt;

//���ǰ������
class B;
class A
{
public:
	A(){ cout << "A()" << endl; }
	~A(){ cout << "~A()" << endl; }
	void test();
	//��Դ���õĵط���������������ָ��
	weak_ptr<B> _ptrb;
};
class B
{
public:
	B(){ cout << "B()" << endl; }
	~B(){ cout << "~B()" << endl; }
	void func(){ cout << "call B::func" << endl; }

	//��Դ���õĵط���������������ָ��
	weak_ptr<A> _ptra;
};


void A::test()
{
	//_ptrb->func(); 
	//��ô��Ȼ������ָ�벻��������Դ�����ü������ӣ�
	//������Ҳ����ֱ�ӷ��ʶ���
	//lock ��������ָ��   ����  ǿ����ָ��
	shared_ptr<B> ptr = _ptrb.lock();
	if (ptr != NULL)
	{
		//�ͱ�ʾ�����ɹ���
		ptr->func();
	}
}

int main()
{
	/*
	������Դ��ʱ��һ����ʹ��ǿ����ָ��
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
	//�м�����׳��쳣��

	delete p;*/



