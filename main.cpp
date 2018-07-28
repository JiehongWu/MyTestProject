#include"nginx.h"
#include<iostream>
using namespace std;

int main()
{
	NgxMemPool gnxpool1;
	gnxpool1.ngx_create_pool(1024);
	cout << gnxpool1.ngx_palloc(8) << endl;
	cout << gnxpool1.ngx_palloc(8) << endl;
	cout << gnxpool1.ngx_pnalloc(15) << endl;
	cout << gnxpool1.ngx_pnalloc(15) << endl;
	gnxpool1.ngx_reset_pool();
	cout << gnxpool1.ngx_palloc(8) << endl;
	void * p = gnxpool1.ngx_palloc(1024);
	cout << p << endl;
	gnxpool1.ngx_pfree(p);
	gnxpool1.ngx_destroy_pool();
	return 0;
}