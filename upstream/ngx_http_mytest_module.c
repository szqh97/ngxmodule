#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_config.h>

typedef struct {
    ngx_http_upstream_conf_t upstream;
} ngx_http_mytest_conf_t;

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
