struct stat;
struct rtcdate;

// system calls
<<<<<<< HEAD
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int*);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int mmap(void); //TODO: ADD CORRECT PARAMETERS
int munmap(void); //TODO: ADD CORRECT PARAMETERS
=======
int fork(void); //1
int exit(int) __attribute__((noreturn)); //2
int wait(int*); //3
int pipe(int*); //4
int write(int, const void*, int); //16
int read(int, void*, int); //5
int close(int); //21
int kill(int); //6
int exec(char*, char**); //7
int open(const char*, int); //15
int mknod(const char*, short, short); //17
int unlink(const char*); //18
int fstat(int fd, struct stat*); //8
int link(const char*, const char*); //19
int mkdir(const char*); //20
int chdir(const char*); //9
int dup(int); //10
int getpid(void); //11
char* sbrk(int); //12
int sleep(int); //13
int uptime(void); //14
int strace(int); //22
>>>>>>> d2ceabe08ace3f854ce65023121265019166eb34

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...);
void printf(const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
