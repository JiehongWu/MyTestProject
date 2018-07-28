#include"nginx.h"
#include<malloc.h>
#include<iostream>
using namespace std;

//创建ngx的内存池
void NgxMemPool::ngx_create_pool(size_t size)
{
	_pool = (ngx_pool_t*)malloc(size);
	if (_pool == NULL)
	{
		throw"malloc error";
	}

	//初始化头部ngx_pool_data_t
	_pool->d.last = (u_char*)_pool + sizeof(ngx_pool_t);
	_pool->d.end = (u_char*)_pool + size;
	_pool->d.next = NULL;
	_pool->d.failed = 0;

	size = size - sizeof(ngx_pool_t);
	_pool->max = (size < NGX_MAX_ALLOC_FROM_POOL) ? size : NGX_MAX_ALLOC_FROM_POOL;

	//初始化ngx_pool_t
	_pool->current = _pool;
	_pool->large = NULL;
}

//销毁ngx内存池
void NgxMemPool::ngx_destroy_pool()
{
	ngx_pool_large_t* i;
	//释放大块内存
	for (i = _pool->large;i!=NULL; i = i->next)
	{
		if (i->alloc != NULL)
		{
			free(i->alloc);
		}
	}

	//释放内存池中的内存块
	ngx_pool_t *p = _pool;
	ngx_pool_t *n = p->d.next;
	while (1)
	{
		free(p);
		p = NULL;
		if (n == NULL)
		{
			break;
		}
		p = n;
		n = n->d.next;
	}
}

//重置内存池
void NgxMemPool::ngx_reset_pool()
{
	//释放大块内存
	ngx_pool_large_t  *l = _pool->large;

	while (l != NULL)
	{
		if (l->alloc != NULL)
		{
			free(l->alloc);
		}
		l = l->next;
	}

	//重置小块内存，不释放已经开辟小块内存，只是修改指针位置和failed
	ngx_pool_t *p = _pool;
	while (p != NULL)
	{
		p->d.last = (u_char *)p + sizeof(ngx_pool_t);
		p->d.failed = 0;
		p = p->d.next;
	}

	_pool->current = _pool;
	_pool->large = NULL;
}

//开辟内存，对齐
void* NgxMemPool::ngx_palloc(size_t size)
{
	//小块内存分配
	if (size <= _pool->max)
	{
		return ngx_palloc_small(_pool, size,1);
	}
	//大块内存分配
	return ngx_palloc_large(_pool, size);
}

//															   a为1 内存对齐
//小块内存分配                                                 a为0 内存不对齐
void* NgxMemPool::ngx_palloc_small(ngx_pool_t *_pool,size_t size,unsigned int a)
{
	//先让指针指向可用的内存池
	ngx_pool_t *p = _pool->current;
	u_char *m;
	do
	{
		m = p->d.last;
		//判断是否需要内存对齐
		if (a)
		{
			m = ngx_align_ptr(m, sizeof(unsigned long));//字节对齐
		}
		//判断当前内存块剩余大小够不够分配
		if ((size_t)(p->d.end - m) >= size)
		{
			//如果够分配，通过指针偏移size大小，并返回可用空间的起始地址
			p->d.last = m + size;
			return m;
		}
		//当前内存块剩余大小不过分配，寻找下一个
		p = p->d.next;
	} while (p);

	//遍历了所有的内存块，发现都不够，重新开辟新的内存块
	return ngx_palloc_block(_pool, size);
}

//开辟新的内存块
void* NgxMemPool::ngx_palloc_block(ngx_pool_t *_pool, size_t size)
{
	//计算内存池中 单个 内存块的大小，并开辟空间
	size_t psize = (size_t)(_pool->d.end - (u_char*)_pool);
	u_char *m = (u_char*)malloc(psize);

	//初始化内存块中信息
	ngx_pool_t *newpool = (ngx_pool_t*)m;

	newpool->d.end = m + psize;
	newpool->d.next = NULL;
	newpool->d.failed = 0;

	//先让m偏移ngx_pool_data_t，越过内存块信息部分
	m += sizeof(ngx_pool_data_t);
	//在让m内存对齐
	m = ngx_align_ptr(m, sizeof(unsigned long));
	//并分配我们需要分配的size空间
	newpool->d.last = m + size;

	//由于我们遍历原有可用内存块，未找到适合的内存块，从而新开辟了一个内部块，
	//所以要把查找过的所有内存块的failed++
	//使下一次查找更高效
	ngx_pool_t *p = _pool->current;
	while (p->d.next != NULL)
	{
		if (p->d.failed++ > 4)
		{
			_pool->current = p->d.next;
		}
		p = p->d.next;
	}
	//把我们新开辟的内存块，连接到内存池中
	p->d.next = newpool;
	return m;
}

//大块内存的分配
//分配方案：1、判断large指针，为空 =>在内存池中寻找一块足够开辟结点大小的内存块，
//			并开辟结点，让large指向它,并让alloc指向我们要开辟的size
//			2、large不为NULL,根据next指针顺序往后查找，找4次（源码中是的计数器n是从0~3）
//			3、如果站在这4个中找到一个空闲的alloc则让alloc指向我们要开辟是size
//			4、如果这4个alloc都被占用，则不再继续寻找，直接开辟新的结点，让alloc指向我们要开辟的size
//				（空间换时间）
void* NgxMemPool::ngx_palloc_large(ngx_pool_t *_pool, size_t size)
{
	//先申请我们需要的size   ，如果内存中连size大小内存都开辟不出来，也没有必要找结点
	//						   开辟好后以后要用到直接把地址赋给alloc，方便以后的操作
	void *p = malloc(size);
	if (p == NULL)
	{
		return NULL;
	}

	//n就是记录所遍历结点的个数  - 计数器
	unsigned int n = 0;

	//遍历当前内存块的large指针指向的三个结点
	ngx_pool_large_t *large = _pool->large;
	while (large != NULL)
	{
		//发现一个未被使用的alloc
		if (large->alloc == NULL)
		{
			//把我们事先开辟好的结点连接上去
			large->alloc = p;
			return p;
		}
		//看上述分配方案 => 为什么有对n的判断？
		if (n++ > 3)
		{
			break;
		}
		large = large->next;
	}

	//在内存池中寻找一块足够开辟结点大小的内存块
	large = (ngx_pool_large_t*)ngx_palloc_small(_pool, sizeof(ngx_pool_large_t), 1);
	if (large == NULL) 
	{
		free(p);
		return NULL;
	}

	//把结点插到链表的头部
	large->alloc = p;
	large->next = _pool->large;
	_pool->large = large;

	return p;
}

//开辟内存，不对齐
void* NgxMemPool::ngx_pnalloc(size_t size)
{
	if (size <= _pool->max)
	{
		return ngx_palloc_small(_pool, size, 0);
	}
	return ngx_palloc_large(_pool, size);
}

//把内存归还给内存池
void NgxMemPool::ngx_pfree(void *p)
{
	ngx_pool_large_t *l = _pool->large;
	//遍历寻找我们需要释放的alloc指针
	while (l != NULL)
	{
		if (p == l->alloc)
		{
			free(l->alloc);
			l->alloc = NULL;
		}
		l=l->next;
	}
}