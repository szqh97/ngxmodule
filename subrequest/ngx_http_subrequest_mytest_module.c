#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_str_t stock[6];
} ngx_http_mytest_ctx_t;

static char* ngx_http_subrequest_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_subrequest_mytest_handler(ngx_http_request_t *r);
static ngx_int_t mytest_subrequest_post_handler(ngx_http_request_t *r, void *data, ngx_int_t rc);


static ngx_command_t ngx_http_request_commands[] = {
    {
        ngx_string("subrequest_szqh97"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_HTTP_LMT_CONF | NGX_CONF_NOARGS,
        ngx_http_subrequest_mytest,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    ngx_null_command
};


static ngx_http_module_t ngx_http_subrequest_mytest_module_ctx = {
    NULL,   /* preconfiguation */
    NULL,   /* postconfiguration */
    
    NULL,   /* create main configuration */
    NULL,   /* init main configuration */

    NULL,   /* create server configuation */
    NULL,   /* merge server configuartion */
    
    NULL,   /* create location configuration */
    NULL    /* merge location configuration */
};

ngx_module_t ngx_http_subrequest_mytest_module = {
    NGX_MODULE_V1,
    &ngx_http_subrequest_mytest_module_ctx,         /* module context */
    ngx_http_request_commands,      /* module directives */
    NGX_HTTP_MODULE,    /* module type */
    NULL,               /* init master */
    NULL,               /* init module */
    NULL,               /* init process */
    NULL,               /* init thread */
    NULL,               /* exit thread */
    NULL,               /* exit process */
    NULL,               /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t mytest_subrequest_post_handler(ngx_http_request_t *r, 
        void *data, ngx_int_t rc) {
    ngx_http_request_t *pr = r->parent;
    ngx_http_mytest_ctx_t* myctx = ngx_http_get_module_ctx(r, ngx_http_subrequest_mytest_module);
    
    pr->headers_out.status = r->headers_out.status;
    if (r->headers_out.status == NGX_HTTP_OK) {
        int flag = 0;

        ngx_buf_t *pRecvBuf = &r->upstream->buffer;
        for(;pRecvBuf->pos != pRecvBuf->end; pRecvBuf->pos++) {
            if (*pRecvBuf->pos == '.' || *pRecvBuf->pos == '\"') {
                if (flag > 0) {
                    myctx->stock[flag-1].len = pRecvBuf->pos-myctx->stock[flag-1].data;
                }
                flag++;
                myctx->stock[flag-1].data = pRecvBuf->pos + 1;
            }
            if (flag > 6) 
                break;
        }
        pr->write_event_handler = mytest_subrequest_post_handler;
        return NGX_OK;
    }
}

static void mytest_post_handler(ngx_http_request_t *r)
{
    if (r->headers_out.status != NGX_HTTP_OK) {
        ngx_http_finalize_request(r, r->headers_out.status);
        return;
    }

    ngx_http_mytest_ctx_t* myctx = ngx_http_get_module_ctx(r, 
            ngx_http_subrequest_mytest_module);
}
