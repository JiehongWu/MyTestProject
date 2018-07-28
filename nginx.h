#ifndef NGINX_H
#define NGINX_H

typedef unsigned int ngx_uint_t;
typedef unsigned int u_int;
typedef unsigned char u_char;
typedef struct ngx_pool_s ngx_pool_t;
typedef struct ngx_pool_large_s  ngx_pool_large_t;
#define NGX_MAX_ALLOC_FROM_POOL 4095
#define ngx_align_ptr(p, a)                                                   \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

/*
С���ڴ�chunk��ͷ��Ϣ
*/
typedef struct {
	u_char               *last;
	u_char               *end;
	ngx_pool_t           *next;
	ngx_uint_t            failed;
} ngx_pool_data_t;

/*
nginx����ڴ�������Ϣ
*/
struct ngx_pool_large_s {
	ngx_pool_large_t     *next;
	void                 *alloc;
};

/*
��ʾnginx�ڴ�صĴ�����
*/
struct ngx_pool_s 
{
	ngx_pool_data_t		d;
	size_t				max;
	ngx_pool_t			*current;
	ngx_pool_large_t	*large;
};

class NgxMemPool
{
public:
	//����ngx���ڴ��
	void ngx_create_pool(size_t size);
	//����ngx�ڴ��
	void ngx_destroy_pool();
	//�����ڴ��
	void ngx_reset_pool();
	//�����ڴ棬����
	void* ngx_palloc(size_t size);
	//�����ڴ棬������
	void* ngx_pnalloc(size_t size);
	//���ڴ�黹���ڴ��
	void ngx_pfree(void *p);
private:
	ngx_pool_t *_pool;

	void* ngx_palloc_small(ngx_pool_t *_pool, size_t size, unsigned int a);
	void* ngx_palloc_large(ngx_pool_t *_pool, size_t size);
	void* ngx_palloc_block(ngx_pool_t *_pool, size_t size);
};

#endif // !NGINX_H