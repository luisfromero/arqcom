//High resolution timer

#ifndef hrtime

#if defined(__LP64__) || defined(_LP64)

#define hrtime   unsigned long
#define hrtime_t unsigned long

static inline hrtime gethrtime() {
  hrtime t;
  unsigned int _a_, _d_;
  asm volatile( "rdtsc" : "=a" (_a_), "=d" (_d_));
  t = ((unsigned long) _a_) | (((unsigned long)_d_) << 32);
  return t;
}

#else

#define hrtime   unsigned long long 
#define hrtime_t unsigned long long 

static inline hrtime gethrtime() {
  hrtime t;
  asm volatile( "rdtsc" : "=A" (t));
  return t;  
}

#endif
#endif

