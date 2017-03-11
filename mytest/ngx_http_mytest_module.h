#include <ngx_config.h>
#include <ngx_core.h>

static char* ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t ngx_http_mytest_commands[] = {
    {
        ngx_string("mytest"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|HGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,
        ngx_http_mytest,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    ngx_null_command
};
