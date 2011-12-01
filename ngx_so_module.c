/*
 * Copyright (C) Tsukasa Hamano <hamano@osstech.co.jp>
 */

#include <dlfcn.h>

#include <nginx.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static char *ngx_so(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t  ngx_so_commands[] = {
    { ngx_string("module"),
      NGX_MAIN_CONF|NGX_CONF_TAKE1,
      ngx_so,
      0,
      0,
      NULL },
      ngx_null_command
};

static ngx_core_module_t  ngx_so_module_ctx = {
    ngx_string("so"),
    NULL,
    NULL
};

ngx_module_t  ngx_so_module = {
    NGX_MODULE_V1,
    &ngx_so_module_ctx,            /* module context */
    ngx_so_commands,               /* module directives */
    NGX_CORE_MODULE,               /* module type */
    NULL,                          /* init master */
    NULL,                          /* init module */
    NULL,                          /* init process */
    NULL,                          /* init thread */
    NULL,                          /* exit thread */
    NULL,                          /* exit process */
    NULL,                          /* exit master */
    NGX_MODULE_V1_PADDING
};

static int ngx_so_check_abi(ngx_conf_t *cf, void *handle){
    int *addon_nginx_version;

    addon_nginx_version = dlsym(handle, "_ngx_so_nginx_version");
    if(!addon_nginx_version){
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "dlsym error: %s", dlerror());
        return NGX_ERROR;
    }

    if(nginx_version != *addon_nginx_version){
        ngx_conf_log_error(NGX_LOG_ERR, cf, 0,
                           "nginx version mismatch! "
                           "working nginx version is %d "
                           "but module was compiled with nginx version %d "
                           "you need recompile this module.",
                           nginx_version, *addon_nginx_version);
        return NGX_ERROR;
    }
    return NGX_OK;
}

static ngx_module_t *ngx_so_get_module(ngx_conf_t *cf, void *handle){
    ngx_module_t *module;
    char **ngx_so_addon_name;

    ngx_so_addon_name = (char **)dlsym(handle, "_ngx_so_addon_name");
    if(!ngx_so_addon_name){
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "dlsym error: %s", dlerror());
        return NULL;
    }

    module = dlsym(handle, *ngx_so_addon_name);
    if(!module){
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "dlsym error: %s", dlerror());
        return NULL;
    }

    return module;
}

static int so_num = 0;
static int so_max = 16; // see auto/modules and objs/ngx_modules.c

static char *
ngx_so(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    void *handle;
    ngx_module_t *module;
    ngx_str_t *value = cf->args->elts;
    char *filename = (char *)value[1].data;
    int i;

    ngx_conf_log_error(NGX_LOG_DEBUG, cf, 0, "loading %s", filename);
    handle = dlopen(filename, RTLD_LAZY);
    if(!handle){
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "dlopen error: %s", dlerror());
        return NGX_CONF_ERROR;
    }

    if(ngx_so_check_abi(cf, handle) != NGX_OK){
        ngx_conf_log_error(NGX_LOG_ERR, cf, 0,
                           "abi version missmatch %s", filename);
        return NGX_CONF_ERROR;
    }

    module = ngx_so_get_module(cf, handle);
    if(!module){
        ngx_conf_log_error(NGX_LOG_ERR, cf, 0,
                           "cannot find module symbol %s", filename);
        return NGX_CONF_ERROR;
    }

    if(so_num >= so_max){
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "so_max limit reached.");
        return NGX_CONF_ERROR;
    }

    // deter duplicate loading.
    for(i=0; ngx_modules[i]; i++){
        if(module == ngx_modules[i]){
            ngx_conf_log_error(NGX_LOG_WARN, cf, 0,
                               "already loaded %s", filename);
            return NGX_CONF_OK;
        }
    }

    module->index = ngx_max_module;
    ngx_modules[ngx_max_module++] = module;
    so_num++;

    ngx_conf_log_error(NGX_LOG_DEBUG, cf, 0, "loaded %s", filename);
    return NGX_CONF_OK;
}
