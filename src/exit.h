#ifndef ENDIAN_EXIT_H_
#define ENDIAN_EXIT_H_

// attempts exit. if cleanup is not ready, function returns
void handle_exit();
void handle_sigint(int);

// to be called right before the bot starts, to let the program know that
// everything has been initialized and a full cleanup can happen
void set_cleanup_ready();

// premature cleanup before everything is initialized, for example to abort
// the program during the cli args initialization phase
void abort_cleanup(int code);

#endif
