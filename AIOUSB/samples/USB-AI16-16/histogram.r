#!/usr/bin/env r

RMS <- function(num) { sqrt(sum((num-ave(num))^2)/length(num)) }

## tmp <- hist(tmpdat$V1,freq=FALSE,breaks=5,xlim=c(range(tmpdat$V1)[1] - 200,range(tmpdat$V1)[2]+200),ylim=c(0,2000))
AIOQual <- function( channel, tmpdata , hexvalues , exactbreaks ) { 
  retval = paste("")
  stats <- fivenum(tmpdata)
  if( hexvalues ) {
    retval <- paste(retval, sprintf("Channel: %d\nMin:%X Max:%X\nSamples:%d\nAvg: %X\n",channel,stats[1],stats[5], length(tmpdata), as.integer(mean(tmpdata))),sep="")
  } else {
    retval <- paste(retval, sprintf("Channel: %d\nMin:%f Max:%f\nSamples:%d\nAvg: %f\n",channel,stats[1],stats[5], length(tmpdata), as.integer(mean(tmpdata))),sep="")
  }
  if ( exactbreaks ) {
      tmpstats <- hist(tmpdata,plot=FALSE,breaks=unique(tmpdata))
  } else {
      tmpstats <- hist(tmpdata,plot=FALSE)
  }
  ## retval <- paste(retval, sprintf("Bins: %d RMS: %f\n", length(tmpstats$counts[tmpstats$counts > 0]),RMS(tmpdata)),sep="" )
  if( is.integer(stats[5]-stats[1] ) ) {
    retval <- paste(retval, sprintf("Bins: %d RMS: %f\n", stats[5]-stats[1],RMS(tmpdata)),sep="" )
  } else {
    retval <- paste(retval, sprintf("Bins: %f RMS: %f\n", stats[5]-stats[1],RMS(tmpdata)),sep="" )
  }
  for ( i in 1:length(tmpstats$counts) ) {
    ## printf("%d\n", i )
    if( tmpstats$counts[i] != 0 ) {
      if( hexvalues ) {
        retval <- paste(retval,sprintf("%X  %d   %.2f\n", as.integer(tmpstats$breaks[i]), tmpstats$counts[i], (100*tmpstats$counts[i] / sum(tmpstats$counts)),sep=""))
      } else {
        retval <- paste(retval,sprintf("%f  %d   %.2f\n", tmpstats$breaks[i], tmpstats$counts[i], (100*tmpstats$counts[i] / sum(tmpstats$counts)),sep=""))
      }
    }
  }
  retval 
}

if( length(argv) < 1 || length(argv) > 4 ) {
  print("Error: Usage:  histogram.r CSVFILE.txt [ USE_HEXVALUES ] [ EXACT_BREAKS ] ")
  quit(save="no")
} else {
  tmpdat <- read.csv(argv[1],header=FALSE)
  if( is.na(argv[2]) ) {
    hexdata <- FALSE
  } else {
    hexdata <- TRUE 
  }

  if( is.na(argv[3]) ) {
    exactbreaks <- FALSE
  } else {
    exactbreaks <- TRUE 
  }
  
  retval = ""
  for ( i in 1:(dim(tmpdat)[2]-1) ) {
    ## printf("i=%d\n",i)
    retval <- paste(retval, "\n\n", AIOQual( i, tmpdat[,i], hexdata , exactbreaks ),sep="" )
  }
  print(cat(retval))
}
