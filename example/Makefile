
ADDON_SRC=ngx_http_example_module.c
ADDON_SO=http_example.so

CC=gcc
INCLUDE=-I $(NGINX_DIR)/src/core -I $(NGINX_DIR)/src/event -I $(NGINX_DIR)/src/event/modules -I $(NGINX_DIR)/src/os/unix -I $(NGINX_DIR)/objs -I $(NGINX_DIR)/src/http -I $(NGINX_DIR)/src/http/modules -I $(NGINX_DIR)/src/mail
CFLAGS=-shared -fPIC -pipe -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-variable -Wunused-value -g $(INCLUDE)
ADDON_NAME=`source config; echo $${ngx_addon_name}`

all: $(ADDON_SO)

clean:
	rm -rf *.so ngx_so_meta.c

ngx_so_meta.c:
	echo $(ADDON_NAME)
	echo "#include <nginx.h>" > $@
	echo "const int _ngx_so_nginx_version = nginx_version;" >> $@
	echo "const char *_ngx_so_addon_name = \"$(ADDON_NAME)\";" >> $@

$(ADDON_SO): $(ADDON_SRC) ngx_so_meta.c
	 $(CC) $(CFLAGS) -o $@ $^
