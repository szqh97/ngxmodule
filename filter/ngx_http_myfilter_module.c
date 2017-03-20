#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_flag_t enable;
} ngx_http_myfilter_conf_t;

typedef struct {
    ngx_int_t add_prefix;
} ngx_http_myfilter_ctx_t;


static void* ngx_http_myfilter_create_conf(ngx_conf_t *cf);
static char* ngx_http_myfilter_merge_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_http_myfilter_init(ngx_conf_t *cf);
 
static ngx_command_t ngx_http_myfilter_commands[] = {
    {
        ngx_string("add_prefix"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF |NGX_HTTP_LMT_CONF | NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_myfilter_conf_t, enable),
        NULL
    },
    ngx_null_command
};

static ngx_http_module_t ngx_http_myfilter_module_ctx = {
    NULL,
    ngx_http_myfilter_init,

    NULL,
    NULL,
    
    NULL,
    NULL,
    ngx_http_myfilter_create_conf,
    ngx_http_myfilter_merge_conf,
};

ngx_module_t ngx_http_myfilter_module = {
    NGX_MODULE_V1,
    &ngx_http_myfilter_module_ctx,
    ngx_http_myfilter_commands,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING 
};

static void* ngx_http_myfilter_create_conf(ngx_conf_t*cf)
{
    ngx_http_myfilter_conf_t *mycf;
    mycf = (ngx_http_myfilter_conf_t *) ngx_palloc(cf->pool, 
            sizeof(ngx_http_myfilter_conf_t));
    if (mycf == NULL) {
        return NULL;
    }

    mycf->enable = NGX_CONF_UNSET;
    return mycf;
}

static char* ngx_http_myfilter_merge_conf(ngx_conf_t *cf, void *parent, 
        void *child)
{
    ngx_http_myfilter_conf_t *prev = (ngx_http_myfilter_conf_t *) parent;
    ngx_http_myfilter_conf_t *conf = (ngx_http_myfilter_conf_t *) child;
    ngx_conf_merg_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
    
}

static ngx_http_output_header_filter_pt ngx_http_next_header_filter;
static ngx_http_output_body_filter_pt ngx_http_next_body_filter;

static ngx_int_t ngx_http_myfilter_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_myfilter_header_filter;

    ngx_http_next_body_filter = ngx_http_top_body_filter;
    ngx_http_top_body_filter = ngx_http_myfilter_body_filter;

    return NGX_OK;
}
