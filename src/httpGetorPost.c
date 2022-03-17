#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include "jansson.h"

#define POSTURL    "http://172.21.22.113:8000/query/"
#define POSTFIELDS "百度"
typedef struct{
	int allocSize;
	int usedSize;
	char* buf;
}curl_respv_t;
#define CURL_INC_SIZE 256
#define BURSIZE 2048

struct memory {
   char *response;
   size_t size;
};



static int parsehttpret(char *chunk);


static int curlBack(void* ptr, int size, int nmemb, void* userp)
{

	int read_bytes = size * nmemb;
	curl_respv_t* rcv = (curl_respv_t*)userp;

    if(rcv->buf == NULL)
    {
		rcv->buf = (char*)malloc(read_bytes + CURL_INC_SIZE);
		if(rcv->buf == NULL)
		{
			printf("malloc failed, errno: %d\n", 1);
			return read_bytes;
		}
		rcv->allocSize = read_bytes + CURL_INC_SIZE;
		memset(rcv->buf, 0, rcv->allocSize);
    }
    else if((read_bytes + rcv->usedSize) > rcv->allocSize)
    {
		rcv->buf = realloc(rcv->buf, read_bytes + rcv->allocSize + CURL_INC_SIZE);
		if(rcv->buf == NULL)
		{
			printf("realloc failed, errno: %d\n", 1);
			return read_bytes;
		}
		rcv->allocSize = read_bytes + rcv->allocSize + CURL_INC_SIZE;
    }

    memcpy(rcv->buf + rcv->usedSize, ptr, read_bytes);
    rcv->usedSize += read_bytes;
//    printf("http->>>>>>>>>>>>>>>>>ret=%s\n",rcv->buf);

    return read_bytes;
}



char dec2hex(short int c)
{
    if (0 <= c && c <= 9) 
    {
        return c + '0';
    } 
    else if (10 <= c && c <= 15) 
    {
        return c + 'A' - 10;
    } 
    else 
    {
        return -1;
    }
}

void urlencode(char url[])
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;
    char res[BURSIZE];
    for (i = 0; i < len; ++i) 
    {
        char c = url[i];
        if (    ('0' <= c && c <= '9') ||
                ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z') || 
                c == '/' || c == '.') 
        {
            res[res_len++] = c;
        } 
        else 
        {
            int j = (short int)c;
            if (j < 0)
                j += 256;
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1 * 16;
            res[res_len++] = '%';
            res[res_len++] = dec2hex(i1);
            res[res_len++] = dec2hex(i0);
        }
    }
    res[res_len] = '\0';
    strcpy(url, res);
}



 
 static size_t cb(void *data, size_t size, size_t nmemb, void *userp)
 {
   size_t realsize = size * nmemb;
   struct memory *mem = (struct memory *)userp;

//    printf("size =%d,nmemb=%d,realsize=%d\n",size,nmemb,realsize);
   char *ptr = realloc(mem->response, mem->size + realsize + 1);
//    printf("mem->size=%d,mem->response size =%d\n",mem->size,mem->size + realsize + 1);
//    printf("data=%s\n",data);

   if(ptr == NULL)
     return 0;  /* out of memory! */
 
   mem->response = ptr;
   memcpy(&(mem->response[mem->size]), data, realsize);
   mem->size += realsize;
   mem->response[mem->size] = 0;
//    printf("http->>>>>>>>>>>>>>>>>ret=%s\n",mem->response);
   return realsize;
 }





 
struct memory chunk = {0};

static int parsehttpret(char *chunk)
{ 
	json_error_t err;
	json_t* root = NULL;
	json_t *jvalue = NULL;
    char * tmp =NULL;
    tmp =strstr(chunk,"{");

	root = json_loads(tmp, 0, &err);
    if (!root){
        printf("json_loads[%s].\n", err.text);
        return 1;
    }
	jvalue = json_object_get(root, "index");
    if(jvalue){
        printf("The text classification result is [%d]\n",(int)json_integer_value(jvalue)); 
		json_decref(root);
    }
	return 0;
}



int main(int argc,char *argv[])
{
	CURL *curl;
	CURLcode res;
    curl_respv_t buf;
	memset(&buf, 0, sizeof(curl_respv_t));
	struct curl_slist *http_header = NULL;
 
	curl = curl_easy_init();
	if (!curl)
	{
		fprintf(stderr,"curl init failed\n");
		return -1;
	}
    char url[1024*8] = "&#x5730;&#x5740;&#x5730;&#x70ED;&#x53D1;&#x7535;&#x4E2A;&#x4EFB;&#x52A1;";
    char geturl[1024*9];
    char *output = curl_easy_escape(curl,url, 0);
    sprintf(geturl, "%s%s", POSTURL,output);
    curl_free(output);

	// curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	// curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
	// curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	// curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1l);
	// curl_easy_setopt(curl, CURLOPT_HEADER, 1l);
    // curl_easy_setopt(curl,CURLOPT_URL,geturl); //url地址
    // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); //不检查ssl，可访问https
    // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);  //不检查ssl，可访问https
    // curl_easy_setopt(curl,CURLOPT_VERBOSE,1); //打印调试信息
    // http_header = curl_slist_append(NULL, "Content-Type:application/json;charset=UTF-8");
    // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_header);
    // curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1); //设置为非0,响应头信息location
    // curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
    // curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);//接收数据时超时设置，如果10秒内数据未
	// res = curl_easy_perform(curl);

    curl_easy_setopt(curl, CURLOPT_URL, geturl);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0l);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0l);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1l);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlBack);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1l);
	curl_easy_setopt(curl, CURLOPT_HEADER, 1l);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5000);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5000);

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers,"Content-Type: application/json;charset=UTF-8");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


	res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);


    printf("chunk >>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<ret=\n%s\n",buf.buf);

    parsehttpret(buf.buf);
    return 0;
}