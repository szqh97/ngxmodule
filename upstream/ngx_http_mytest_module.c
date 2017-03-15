#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_config.h>

typedef struct {
    ngx_http_upstream_conf_t upstream;
} ngx_http_mytest_conf_t;

typedef struct {
    ngx_http_status_t status;
} ngx_http_mytest_ctx_t;

typedef struct 
{} ngx_http_mytest_module;

static ngx_int_t mytest_upstream_process_header(ngx_http_request_t *r);
static void* ngx_http_mytest_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_mytest_conf_t* mycf;
    mycf = (ngx_http_mytest_conf_t*)ngx_pcalloc(cf->pool, sizeof(ngx_http_mytest_conf_t));
    if (mycf == NULL) {
        return NULL;
    }
    mycf->upstream.connect_timeout = 60000;
    mycf->upstream.send_timeout = 60000;
    mycf->upstream.read_timeout = 60000;
    mycf->upstream.send_timeout = 60000;

    mycf->upstream.buffering = 0;
    mycf->upstream.bufs.num = 8;
    mycf->upstream.bufs.size = ngx_pagesize;
    mycf->upstream.buffer_size = ngx_pagesize;
    mycf->upstream.busy_buffers_size = 2*ngx_pagesize;
    mycf->upstream.temp_file_write_size = 2*ngx_pagesize;
    mycf->upstream.max_temp_file_size = 1024*1024*1024;

    mycf->upstream.hide_headers = NGX_CONF_UNSET_PTR;
    mycf->upstream.pass_headers = NGX_CONF_UNSET_PTR;

    return mycf;
}

static char* ngx_http_mytest_merge_loc_conf(ngx_conf_t* cf, void* parent, void* child)
{
    ngx_http_mytest_conf_t* prev = (ngx_http_mytest_conf_t*) parent;
    ngx_http_mytest_conf_t* conf = (ngx_http_mytest_conf_t*) child;
    
    ngx_hash_init_t hash;
    hash.max_size = 100;
    hash.bucket_size = 1024;
    hash.name = "proxy_header_hash";
    if (ngx_http_upstream_hide_headers_hash(cf, &conf->upstream, &prev->upstream,
                ngx_http_upstream_hide_headers_hash, &hash) != NGX_OK){
        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}

static ngx_int_t mytest_upstream_create_request(ngx_http_request_t *r)
{
    static ngx_str_t backendQueryLine = ngx_string("GET /search?q=%V HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n");
    ngx_int_t queryLineLen = backendQueryLine.len + r->args.len - 2; 

    ngx_buf_t* b = ngx_create_temp_buf(r->pool, queryLineLen);
    if (b == NULL) {
        return NGX_ERROR;
    }
    b->last = b->pos + queryLineLen;
    ngx_snprintf(b->pos, queryLineLen, 
            (char*)backendQueryLine.data, &r->args);
    r->upstream->request_bufs = ngx_alloc_chain_link(r->pool);
    if (r->upstream->request_bufs == NULL)
        return NGX_ERROR;

    r->upstream->request_bufs->buf = b;
    r->upstream->request_bufs->next = NULL;

    r->upstream->request_sent = 0;
    r->upstream->header_sent = 0;
    r->header_hash = 1;
    return NGX_OK;
}

static ngx_int_t 
mytest_process_status_line(ngx_http_request_t* r)
{
    size_t len;
    ngx_int_t rc;
    ngx_http_upstream_t *u;

    ngx_http_mytest_ctx_t* ctx = ngx_http_get_module_ctx(r, ngx_http_mytest_module);
    if (ctx == NULL) {
        return NGX_ERROR;
    }

    u = r->upstream;
    rc = ngx_http_parse_status_line(r, &u->buffer, &ctx->status);
    if (rc == NGX_AGAIN)  {
        return rc;
    }

    if (rc == NGX_ERROR) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "upstream sent no valid HTTP/1.0 header");
        r->http_version = NGX_HTTP_VERSION_9;
        u->state->status = NGX_HTTP_OK;
        return NGX_OK;
    }

    if (u->state) {
        u->state->status = ctx->status.code;
    }

    u->headers_in.status_n = ctx->status.code;

    len = ctx->status.end - ctx->status.start;
    u->headers_in.status_line.len = len;

    u->headers_in.status_line.data = ngx_pnalloc(r->pool, len);
    if (u->headers_in.status_line.data == NULL){
        return NGX_ERROR;
    }

    ngx_memcpy(u->headers_in.status_line.data, ctx->status.start, len);
    u->process_header = mytest_upstream_process_header;

    return mytest_upstream_process_header(r);
}

static ngx_int_t mytest_upstream_process_header(ngx_http_request_t *r) 
{
    ngx_int_t   rc;
    ngx_table_elt_t *h;
    ngx_http_upstream_header_t *hh;
    ngx_http_upstream_main_conf_t *umcf;

    umcf = ngx_http_get_module_main_conf(r, ngx_http_upstream_module);
    for (;;) {
        rc = ngx_http_parse_header_line(r, &r->upstream->buffer, 1);
        if (rc == NGX_OK)  {
            h = ngx_list_push(&r->upstream->headers_in.headers);
            if (h == NULL) {
                return NGX_ERROR;
            }
            h->hash = r->header_hash;

            h->key.len = r->header_name_end - r->header_name_start;
            h->value.len = r->header_end - r->header_start;

            h->key.data = ngx_pnalloc(r->pool, 
                h->key.len+ 1 + h->value.len + 1 + h->key.len);
            if (h->key.data == NULL) {
                return NGX_ERROR;
            }

            h->value.data = h->key.data + h->key.len + 1;
        }
    }
}
