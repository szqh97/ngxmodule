#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static ngx_conf_enum_t test_enums[] = {
    {ngx_string("apple"), 1},
    {ngx_string("banana"), 2},
    {ngx_string("orange"), 3},
    {ngx_null_string, 0}
};

static ngx_conf_bitmask_t test_bitmasks[] = {
    {ngx_string("good"), 0x0002},
    {ngx_string("better"), 0x0004},
    {ngx_string("best"), 0x0008},
    {ngx_null_string, 0}
};

typedef struct {
    ngx_str_t       test_str;
    ngx_int_t       test_num;
    ngx_flag_t      test_flag;
    size_t          test_size;
    ngx_array_t*    test_str_array;
    ngx_array_t*    test_keyval;
    off_t           test_off;
    ngx_msec_t      test_msec;
    time_t          test_sec;
    ngx_bufs_t      test_bufs;
    ngx_uint_t      test_enum_seq;
    ngx_uint_t      test_bitmask;
    ngx_uint_t      test_access;
    ngx_path_t*     test_path;
} ngx_http_mytest_conf_t;

typedef struct {
    ngx_str_t my_config_str;
    ngx_int_t my_config_num;
} ngx_http_mytest_config2_t;

static char* ngx_conf_set_myconfig(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char* ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);


static void* ngx_http_mytest_create_loc_conf(ngx_conf_t *cf) 
{
    ngx_http_mytest_conf_t *mycf;
    
    mycf = (ngx_http_mytest_conf_t *) ngx_pcalloc(cf->pool, sizeof(ngx_http_mytest_conf_t));
    if (NULL == mycf) {
        return NULL;
    }

    mycf->test_flag = NGX_CONF_UNSET;
    mycf->test_num = NGX_CONF_UNSET;
    mycf->test_str_array = NGX_CONF_UNSET_PTR;
    mycf->test_keyval = NULL;
    mycf->test_off = NGX_CONF_UNSET;
    mycf->test_msec = NGX_CONF_UNSET_MSEC;
    mycf->test_size = NGX_CONF_UNSET_SIZE;
    return mycf;
}
static ngx_command_t ngx_http_mytest_commands[] = {
    {
        ngx_string("mytest"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,
        ngx_http_mytest,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    {
        ngx_string("test_str"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_str),
        NULL
    },
    {
        ngx_string("test_str_array"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_str_array_slot,NGX_HTTP_LOC_CONF_OFFSET,
        NGX_HTTP_LOC_CONF_OFFSET,
        NULL
    },
    {
        ngx_string("test_keyval"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE2,
        ngx_conf_set_keyval_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_keyval),
        NULL
    },
    {
        ngx_string("test_num"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_num_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_num),
        NULL
    },

    {
        ngx_string("test_size"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_size_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_size),
        NULL
    },
     
    {
        ngx_string("test_off"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_off_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_off),
        NULL
    },

    {
        ngx_string("test_msec"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_msec_slot, 
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_msec),
        NULL
    },

    {
        ngx_string("test_sec"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_sec_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_sec),
        NULL
    },

    {
        ngx_string("test_bufs"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE2,
        ngx_conf_set_bufs_slot , 
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_bufs),
        NULL
    },
    {
        ngx_string("test_enum"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_enum_slot, 
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_enum_seq),
        NULL
    },

    {

        ngx_string("test_bitmask"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_bitmask_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_bitmask),
        test_bitmasks
    },

    {
        ngx_string("test_access"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE123,
        ngx_conf_set_access_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_access),
        NULL
    },
    
    {
        ngx_string("test_path"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1234,
        ngx_conf_set_path_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, test_path),
        NULL
    },

    {
        ngx_string("test_myconfig"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE12,
        ngx_conf_set_myconfig,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    ngx_null_command
};

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)
{
    if (!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD))) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    ngx_int_t rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK) {
        return rc;
    }

    ngx_str_t type = ngx_string("text/plain");
    ngx_str_t response = ngx_string("team working [ss](http://example.com)");
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = response.len;
    r->headers_out.content_type = type;

    rc =  ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only ) {
        return rc;
    }

    ngx_buf_t *b;
    b = ngx_create_temp_buf(r->pool, response.len);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    ngx_memcpy(b->pos, response.data, response.len);
    b->last = b->pos + response.len;
    b->last_buf = 1;

    ngx_chain_t out ;
    out.buf = b;
    out.next = NULL;


}



static ngx_http_module_t ngx_http_mytest_module_ctx = {
    NULL,           /* preconfiguartion */
    NULL,           /* postconfiguration */

    NULL,           /* create main configuration */
    NULL,           /* init main configuration */

    NULL,           /* create server cofiguation */
    NULL,           /* merge server configuation */

    NULL,           /* create location configuration */
    NULL            /* merge location configuartion */
};

ngx_module_t ngx_http_mytest_module = {
    NGX_MODULE_V1,
    &ngx_http_mytest_module_ctx,    /* modulte context */
    ngx_http_mytest_commands,       /* module directives */
    NGX_HTTP_MODULE,                /* module type */
    NULL,                           /* init master */
    NULL,                           /* init module */
    NULL,                           /* init process */
    NULL,                           /* init thread */
    NULL,                           /* exit thread */
    NULL,                           /* exit process */
    NGX_MODULE_V1_PADDING           /* exit master */
};



static char* ngx_conf_set_myconfig(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_mytest_config2_t *myconf = conf;
    ngx_str_t* value = cf->args->elts;

    if (cf->args->nelts > 1)  {
        myconf->my_config_str = value[1];
    }

    if (cf->args->nelts > 2) {
        myconf->my_config_num = ngx_atoi(value[2].data, value[2].len);
        if (myconf->my_config_num == NGX_ERROR) {
            return "invalid number";
        }
    }

    return NGX_CONF_OK;
}

static char*
ngx_http_mytest_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_mytest_conf_t *prev = (ngx_http_mytest_conf_t*) parent;
    ngx_http_mytest_conf_t *conf = (ngx_http_mytest_conf_t*) child;
    ngx_conf_merge_str_value(conf->test_str, prev->test_str, "defaultstr");
    return NGX_CONF_OK;
}

static char* ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf;
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_mytest_handler;
    return NGX_CONF_OK;
}
