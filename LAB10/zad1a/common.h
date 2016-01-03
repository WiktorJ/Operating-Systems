#define MAX_MESSAGE_LENGTH 256
#define MAX_NICK_LENGTH 12
#define INTERNET 666
#define LOCAL_TYPE 667
#include <sys/un.h>
struct msg
{
    char buf[MAX_MESSAGE_LENGTH];
    char name[MAX_NICK_LENGTH];
    int sock;
};

struct clientProfile
{
    struct sockaddr_in inter;
    char name[MAX_NICK_LENGTH ];
    long last_activity;
    int active;
    int type;
    int sock;
};
