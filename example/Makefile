CC=gcc
CFLAGS=-shared -fPIC -pipe -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-variable -Wunused-value -g  -I $(NGINX_DIR)/src/core -I $(NGINX_DIR)/src/event -I $(NGINX_DIR)/src/event/modules -I $(NGINX_DIR)/src/os/unix -I $(NGINX_DIR)/objs -I $(NGINX_DIR)/src/http -I $(NGINX_DIR)/src/http/modules -I $(NGINX_DIR)/src/mail

all: example.so

clean:
	rm -rf *.so

example.so: ngx_http_example_module.c
	 $(CC) $(CFLAGS) -o $@ $^
