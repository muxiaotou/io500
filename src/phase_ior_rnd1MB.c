#include <sys/stat.h>
#include <unistd.h>

#include <phase_ior.h>
#include <io500-phase.h>

opt_ior_rnd ior_rnd1MB_o;

static ini_option_t option[] = {
  {"API", "The API to be used", 0, INI_STRING, NULL, & ior_rnd1MB_o.api},
  {"blockSize", "Size of a random block, change only if explicitly allowed", 0, INI_UINT64, "1073741824", & ior_rnd1MB_o.block_size},
  {"run", "Run this phase", 0, INI_BOOL, "TRUE", & ior_rnd1MB_o.run},
  {"verbosity", "The verbosity level", 0, INI_INT, 0, & ior_rnd1MB_o.verbosity},
  {"randomPrefill", "Prefill the file with this blocksize in bytes, e.g., 2097152", 0, INI_INT, "0", & ior_rnd1MB_o.random_prefill_bytes},
  {NULL} };


static void validate(void){
  opt_ior_rnd d = ior_rnd1MB_o;
  if(d.block_size < 1024){
    FATAL("Random blocksize must be larger than 1024\n");
  }
  if(d.random_prefill_bytes > 0 && (d.block_size % d.random_prefill_bytes) != 0){
    FATAL("Random prefill bytes must divide blocksize\n");
  }
  if(d.random_prefill_bytes > 0 && d.block_size < d.random_prefill_bytes){
    FATAL("Random prefill bytes must be < blocksize\n");
  }
}


void ior_rnd1MB_add_params(u_argv_t * argv){
  opt_ior_rnd d = ior_rnd1MB_o;

  u_argv_push(argv, "./ior");
  u_argv_push_printf(argv, "--dataPacketType=%s", opt.dataPacketType);
  for(int i=0; i < ior_rnd1MB_o.verbosity; i++){
    u_argv_push(argv, "-v");
  }
  if(opt.io_buffers_on_gpu){
    u_argv_push(argv, "-O");
    u_argv_push(argv, "allocateBufferOnGPU=1");
  }
  u_argv_push(argv, "-Q=1");
  //u_argv_push(argv, "-F");
  u_argv_push(argv, "-g");
  int hash = u_phase_unique_random_number("ior-random1MB");
  u_argv_push_printf(argv, "-G=%d", hash);
  u_argv_push(argv, "-z");
  u_argv_push(argv, "--random-offset-seed=11");
  u_argv_push(argv, "-e");
  u_argv_push_printf(argv, "-o=%s/ior-rnd1MB/file", opt.datadir);
  u_argv_push(argv, "-O");
  u_argv_push_printf(argv, "stoneWallingStatusFile=%s/ior-rnd1MB.stonewall", opt.resdir );
  u_argv_push(argv, "-k");
  u_argv_push(argv, "-t=1048576");
  u_argv_push_printf(argv, "-b=%ld", d.block_size);
  u_argv_push_printf(argv, "-s=%d", 10000000);
}

u_phase_t p_ior_rnd1MB = {
  "ior-rnd1MB",
  IO500_PHASE_DUMMY,
  option,
  validate,
  NULL,
  NULL
};
