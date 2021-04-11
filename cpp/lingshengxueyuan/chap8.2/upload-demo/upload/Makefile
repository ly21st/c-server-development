
# 使用的编译器
CC=gcc
# 预处理参数
CPPLFAGS=-I ./include 	\
		 -I /usr/include/fastdfs 	\
		 -I /usr/include/fastcommon \
		 -I /usr/include/mysql

CFLAGS=-Wall
# 需要链接的动态库
LIBS=-lfcgi	\
	 -lfastcommon	\
	 -lfdfsclient	\
	 -lmysqlclient

CGIPATH=./src_cgi
BINPATH=./bin_cgi
COMMONPATH=./common

# 子目标, 因为是测试,所有需要单独生成很多子目标
upload=$(BINPATH)/upload.cgi

# 最终目标
target=$(upload)
ALL:$(target)

# 生成所有的.o 文件
%.o:%.c
	$(CC) -c $< -o $@ $(CPPLFAGS) $(CFLAGS)

# 生成每一个子目标, 
# main程序
$(upload):$(CGIPATH)/upload.o \
	$(COMMONPATH)/make_log.o  \
	$(COMMONPATH)/fdfs_api.o  \
	$(COMMONPATH)/deal_mysql.o
	$(CC) $^ $(LIBS) -o $@

# 项目清除
clean:
	-rm -rf *.o $(target) $(CGIPATH)/*.o $(COMMONPATH)/*.o

# 声明伪文件
.PHONY:clean ALL
