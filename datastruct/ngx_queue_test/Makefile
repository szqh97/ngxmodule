CC=gcc
C_FLAGS = -g -Wall -Wextra  
DIR=/root/Projects/nginx-1.0.15
TARGETS=ngx_queue_test
TARGETS_FILE=$(TARGETS).c

all:$(TARGETS)


clean:
	rm -f $(TARGETS) *.o

CORE_INCS=-I $(DIR)/src/core/ \
		  -I $(DIR)/objs/ \
		  -I $(DIR)/src/event \
		  -I $(DIR)/src/event/modules \
		  -I $(DIR)/src/os/unix \
		  #-I $(DIR)/Nginx_Pre/pcre-8.32/

NGX_OBJ =  $(DIR)/objs/src/core/ngx_queue.o 

$(TARGETS):$(TARGETS_FILE)
	$(CC) $(C_FLAGS) $(TARGETS_FILE) $(CORE_INCS) $(NGX_OBJ) -o $@
