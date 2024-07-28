#ifndef __THREADS__
#define __THREADS__


extern	int		numthreads;

int	GetThreadWork (void);
void RunThreadsOn (int workcnt, qboolean showpacifier, void(*func)(int));
void ThreadLock (void);
void ThreadUnlock (void);

#endif