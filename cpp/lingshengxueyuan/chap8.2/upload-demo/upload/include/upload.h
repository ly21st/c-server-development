#ifndef _UPLOAD_H
#define _UPLOAD_H

// 内存块中查找字符串
char* memstr(char* full_data, int full_data_len, char* substr);
// 得到上傳文件內容
int get_file_content(char* begin, char* end, int len, char* filename);
// 將文件名和文件ID写入mysql数据库
int store_data(char* filename, char* fileid);

#endif // _UPLOAD_H