#include "../libs/cSpec/export/cSpec.h"
#include "profiler_base/profiler_base.module.spec.h"

spec_suite({ T_profiler_base(); });

int main(void) { run_spec_suite("all"); }
