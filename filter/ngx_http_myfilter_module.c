#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_flag_t enable;
} ngx_http_myfilter_conf_t;

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
