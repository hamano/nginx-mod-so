
#include <dlfcn.h>

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static char *ngx_http_so(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t  ngx_http_so_commands[] = {
    { ngx_string("module"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE2,
      ngx_http_so,
      0,
      0,
      NULL },
      ngx_null_command
};

static ngx_http_module_t  ngx_http_so_module_ctx = {
    NULL,                          /* preconfiguration */
    NULL,                          /* postconfiguration */

    NULL,                          /* create main configuration */
    NULL,                          /* init main configuration */

    NULL,                          /* create server configuration */
    NULL,                          /* merge server configuration */

    NULL,                          /* create location configuration */
    NULL                           /* merge location configuration */
};

ngx_module_t  ngx_http_so_module = {
    NGX_MODULE_V1,
    &ngx_http_so_module_ctx,       /* module context */
    ngx_http_so_commands,          /* module directives */
    NGX_HTTP_MODULE,               /* module type */
    NULL,                          /* init master */
    NULL,                          /* init module */
    NULL,                          /* init process */
    NULL,                          /* init thread */
    NULL,                          /* exit thread */
    NULL,                          /* exit process */
    NULL,                          /* exit master */
    NGX_MODULE_V1_PADDING
};

static char *
ngx_http_so(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;
	void *handle;
	ngx_module_t *so_sym;
	ngx_str_t *value = cf->args->elts;
	char *so_mod = (char *)value[1].data;
	char *so_file = (char *)value[2].data;
	int i;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

	handle = dlopen(so_file, RTLD_LAZY);
	if(!handle){
		fprintf(stderr, "dlerror: %s\n", dlerror());
		return NGX_CONF_ERROR;
	}
	so_sym = dlsym(handle, so_mod);
	if(!so_sym){
		fprintf(stderr, "dlerror: %s\n", dlerror());
		return NGX_CONF_ERROR;
	}

	for(i=0; ngx_modules[i]; i++){}
	ngx_modules[i] = so_sym;

	//dlclose(handle);
    return NGX_CONF_OK;
}
