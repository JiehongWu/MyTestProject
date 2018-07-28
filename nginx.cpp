#include"nginx.h"
#include<malloc.h>
#include<iostream>
using namespace std;

//����ngx���ڴ��
void NgxMemPool::ngx_create_pool(size_t size)
{
	_pool = (ngx_pool_t*)malloc(size);
	if (_pool == NULL)
	{
		throw"malloc error";
	}

	//��ʼ��ͷ��ngx_pool_data_t
	_pool->d.last = (u_char*)_pool + sizeof(ngx_pool_t);
	_pool->d.end = (u_char*)_pool + size;
	_pool->d.next = NULL;
	_pool->d.failed = 0;

	size = size - sizeof(ngx_pool_t);
	_pool->max = (size < NGX_MAX_ALLOC_FROM_POOL) ? size : NGX_MAX_ALLOC_FROM_POOL;

	//��ʼ��ngx_pool_t
	_pool->current = _pool;
	_pool->large = NULL;
}

//����ngx�ڴ��
void NgxMemPool::ngx_destroy_pool()
{
	ngx_pool_large_t* i;
	//�ͷŴ���ڴ�
	for (i = _pool->large;i!=NULL; i = i->next)
	{
		if (i->alloc != NULL)
		{
			free(i->alloc);
		}
	}

	//�ͷ��ڴ���е��ڴ��
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

//�����ڴ��
void NgxMemPool::ngx_reset_pool()
{
	//�ͷŴ���ڴ�
	ngx_pool_large_t  *l = _pool->large;

	while (l != NULL)
	{
		if (l->alloc != NULL)
		{
			free(l->alloc);
		}
		l = l->next;
	}

	//����С���ڴ棬���ͷ��Ѿ�����С���ڴ棬ֻ���޸�ָ��λ�ú�failed
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

//�����ڴ棬����
void* NgxMemPool::ngx_palloc(size_t size)
{
	//С���ڴ����
	if (size <= _pool->max)
	{
		return ngx_palloc_small(_pool, size,1);
	}
	//����ڴ����
	return ngx_palloc_large(_pool, size);
}

//															   aΪ1 �ڴ����
//С���ڴ����                                                 aΪ0 �ڴ治����
void* NgxMemPool::ngx_palloc_small(ngx_pool_t *_pool,size_t size,unsigned int a)
{
	//����ָ��ָ����õ��ڴ��
	ngx_pool_t *p = _pool->current;
	u_char *m;
	do
	{
		m = p->d.last;
		//�ж��Ƿ���Ҫ�ڴ����
		if (a)
		{
			m = ngx_align_ptr(m, sizeof(unsigned long));//�ֽڶ���
		}
		//�жϵ�ǰ�ڴ��ʣ���С����������
		if ((size_t)(p->d.end - m) >= size)
		{
			//��������䣬ͨ��ָ��ƫ��size��С�������ؿ��ÿռ����ʼ��ַ
			p->d.last = m + size;
			return m;
		}
		//��ǰ�ڴ��ʣ���С�������䣬Ѱ����һ��
		p = p->d.next;
	} while (p);

	//���������е��ڴ�飬���ֶ����������¿����µ��ڴ��
	return ngx_palloc_block(_pool, size);
}

//�����µ��ڴ��
void* NgxMemPool::ngx_palloc_block(ngx_pool_t *_pool, size_t size)
{
	//�����ڴ���� ���� �ڴ��Ĵ�С�������ٿռ�
	size_t psize = (size_t)(_pool->d.end - (u_char*)_pool);
	u_char *m = (u_char*)malloc(psize);

	//��ʼ���ڴ������Ϣ
	ngx_pool_t *newpool = (ngx_pool_t*)m;

	newpool->d.end = m + psize;
	newpool->d.next = NULL;
	newpool->d.failed = 0;

	//����mƫ��ngx_pool_data_t��Խ���ڴ����Ϣ����
	m += sizeof(ngx_pool_data_t);
	//����m�ڴ����
	m = ngx_align_ptr(m, sizeof(unsigned long));
	//������������Ҫ�����size�ռ�
	newpool->d.last = m + size;

	//�������Ǳ���ԭ�п����ڴ�飬δ�ҵ��ʺϵ��ڴ�飬�Ӷ��¿�����һ���ڲ��飬
	//����Ҫ�Ѳ��ҹ��������ڴ���failed++
	//ʹ��һ�β��Ҹ���Ч
	ngx_pool_t *p = _pool->current;
	while (p->d.next != NULL)
	{
		if (p->d.failed++ > 4)
		{
			_pool->current = p->d.next;
		}
		p = p->d.next;
	}
	//�������¿��ٵ��ڴ�飬���ӵ��ڴ����
	p->d.next = newpool;
	return m;
}

//����ڴ�ķ���
//���䷽����1���ж�largeָ�룬Ϊ�� =>���ڴ����Ѱ��һ���㹻���ٽ���С���ڴ�飬
//			�����ٽ�㣬��largeָ����,����allocָ������Ҫ���ٵ�size
//			2��large��ΪNULL,����nextָ��˳��������ң���4�Σ�Դ�����ǵļ�����n�Ǵ�0~3��
//			3�����վ����4�����ҵ�һ�����е�alloc����allocָ������Ҫ������size
//			4�������4��alloc����ռ�ã����ټ���Ѱ�ң�ֱ�ӿ����µĽ�㣬��allocָ������Ҫ���ٵ�size
//				���ռ任ʱ�䣩
void* NgxMemPool::ngx_palloc_large(ngx_pool_t *_pool, size_t size)
{
	//������������Ҫ��size   ������ڴ�����size��С�ڴ涼���ٲ�������Ҳû�б�Ҫ�ҽ��
	//						   ���ٺú��Ժ�Ҫ�õ�ֱ�Ӱѵ�ַ����alloc�������Ժ�Ĳ���
	void *p = malloc(size);
	if (p == NULL)
	{
		return NULL;
	}

	//n���Ǽ�¼���������ĸ���  - ������
	unsigned int n = 0;

	//������ǰ�ڴ���largeָ��ָ����������
	ngx_pool_large_t *large = _pool->large;
	while (large != NULL)
	{
		//����һ��δ��ʹ�õ�alloc
		if (large->alloc == NULL)
		{
			//���������ȿ��ٺõĽ��������ȥ
			large->alloc = p;
			return p;
		}
		//���������䷽�� => Ϊʲô�ж�n���жϣ�
		if (n++ > 3)
		{
			break;
		}
		large = large->next;
	}

	//���ڴ����Ѱ��һ���㹻���ٽ���С���ڴ��
	large = (ngx_pool_large_t*)ngx_palloc_small(_pool, sizeof(ngx_pool_large_t), 1);
	if (large == NULL) 
	{
		free(p);
		return NULL;
	}

	//�ѽ��嵽�����ͷ��
	large->alloc = p;
	large->next = _pool->large;
	_pool->large = large;

	return p;
}

//�����ڴ棬������
void* NgxMemPool::ngx_pnalloc(size_t size)
{
	if (size <= _pool->max)
	{
		return ngx_palloc_small(_pool, size, 0);
	}
	return ngx_palloc_large(_pool, size);
}

//���ڴ�黹���ڴ��
void NgxMemPool::ngx_pfree(void *p)
{
	ngx_pool_large_t *l = _pool->large;
	//����Ѱ��������Ҫ�ͷŵ�allocָ��
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