
#define MAX_MESSAGE_LENGTH 256
#define MAX_NAME_LENGTH 24
#define LOCAL_TYPE 999
#define INTERNET_TYPE 1000
struct server
{
    int sock;
    int type;
    struct sockaddr_in addr;
    struct sockaddr_un addrLocal;
};

struct connection
{
    int sock;
    struct sockaddr_in addr;
    struct sockaddr_in local;
    char name[MAX_NAME_LENGTH];
    int type;
};

struct msg
{
    char name[MAX_NAME_LENGTH];
    char buff[MAX_MESSAGE_LENGTH];
};
