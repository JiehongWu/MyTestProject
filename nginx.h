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
小块内存chunk的头信息
*/
typedef struct {
	u_char               *last;
	u_char               *end;
	ngx_pool_t           *next;
	ngx_uint_t            failed;
} ngx_pool_data_t;

/*
nginx大块内存类型信息
*/
struct ngx_pool_large_s {
	ngx_pool_large_t     *next;
	void                 *alloc;
};

/*
表示nginx内存池的大类型
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
	//创建ngx的内存池
	void ngx_create_pool(size_t size);
	//销毁ngx内存池
	void ngx_destroy_pool();
	//重置内存池
	void ngx_reset_pool();
	//开辟内存，对齐
	void* ngx_palloc(size_t size);
	//开辟内存，不对齐
	void* ngx_pnalloc(size_t size);
	//把内存归还给内存池
	void ngx_pfree(void *p);
private:
	ngx_pool_t *_pool;

	void* ngx_palloc_small(ngx_pool_t *_pool, size_t size, unsigned int a);
	void* ngx_palloc_large(ngx_pool_t *_pool, size_t size);
	void* ngx_palloc_block(ngx_pool_t *_pool, size_t size);
};

#endif // !NGINX_H