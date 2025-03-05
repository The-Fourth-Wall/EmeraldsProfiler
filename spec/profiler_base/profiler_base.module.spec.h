#include "../../libs/cSpec/export/cSpec.h"

#include <unistd.h>

#define EMERALDS_ENABLE_PROFILER
#include "../../src/profiler_base/profiler_base.h"

#include <unistd.h>

#ifndef usleep
extern int usleep(unsigned int __useconds);
#endif

module(T_profiler_base, {
  it("tests random profiles", {
    int i;
    PROFILE({ usleep(10000); });
    for(i = 0; i < 10; ++i) {
      PROFILE("foo for", {
        usleep(10000);
        usleep(10000);
        usleep(10000);
        usleep(10000);
      });
    }

    profiler_display();
  });
})
