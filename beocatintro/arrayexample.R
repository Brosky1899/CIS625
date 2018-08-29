# Constants
jobnum = as.integer(Sys.getenv('SGE_TASK_ID'))

source(paste("setup-",jobnum,".R",sep=""))

source("SomeOtherProgram.R")
