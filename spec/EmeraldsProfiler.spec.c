#include "../libs/cSpec/export/cSpec.h"
#include "profiler_base/profiler_base.module.spec.h"

int main(void) {
  cspec_run_suite("all", { T_profiler_base(); });
}
