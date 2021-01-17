#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fdfs_client.h"

//编译语句：
//gcc fdfs_upload_file2.c -o test -I/usr/include/fastdfs -lfdfsclient -lfastcommon

int fdfs_upload_file(const char* filename, char* fileid)
{
    char group_name[FDFS_GROUP_NAME_MAX_LEN + 1];
    ConnectionInfo *pTrackerServer;
    int result;
    int store_path_index;
    ConnectionInfo storageServer;
    
    log_init();
    g_log_context.log_level = LOG_ERR;
    ignore_signal_pipe();
    
    // 加载配置文件, 并初始化
    const char* conf_file = "/etc/fdfs/client.conf";
    if ((result=fdfs_client_init(conf_file)) != 0)
    {
        return result;
    }

    // 通过配置文件信息连接tracker, 并得到一个可以访问tracker的句柄
    pTrackerServer = tracker_get_connection();
    if (pTrackerServer == NULL)
    {
        fdfs_client_destroy();
        return errno != 0 ? errno : ECONNREFUSED;
    }
    *group_name = '\0';
    // 通过tracker句柄得到一个可以访问的storage句柄
    if ((result=tracker_query_storage_store(pTrackerServer, \
                    &storageServer, group_name, &store_path_index)) != 0)
    {
        fdfs_client_destroy();
        
        return result;
    }
    // 通过得到的storage句柄, 上传本地文件
    result = storage_upload_by_filename1(pTrackerServer, \
            &storageServer, store_path_index, \
            filename, NULL, \
            NULL, 0, group_name, fileid);
    if (result == 0)
    {
        
    }
    else
    {
        
    }
    // 断开连接, 释放资源
    tracker_close_connection_ex(pTrackerServer, true);
    fdfs_client_destroy();
    return result;
}

int main(int argc, char *argv[])
{
    char filename[2048] = {0}; //文件名
    sprintf(filename, "/home/milo/share/temp/cloud-disk/1.png");
    char fileid[2048] = {0};    //文件上传到fastDFS后的文件id
    fdfs_upload_file(filename, fileid);
    return 0;
}