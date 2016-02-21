#ifndef SPARTOR_COMMAND_H_
#define SPARTOR_COMMAND_H_

enum NETMODE {
        NM_NONE = 0,
        NM_HOST,
        NM_CLIENT,
};

extern int netmode;

void command(const char *s);
void exec_commands( char *file );

#endif
