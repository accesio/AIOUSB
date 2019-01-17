function [n,bins,diffvals] =  octave_usb_idio_speed_test( num_repetitions );
# 
# $Date $Format: %ad$$
# $Author $Format: %an <%ae>$$
# $Release $Format: %h$$
# @brief Sample program to run the USB-DIO-16 from within Matlab
# @note This sample Triggers the relay switches. Note that the
# output is inverted such that if you write a '1' to a bit location
# then any LED connected to that pin will switch OFF, not ON
#

if strcmp(getenv('AIO_LIB_DIR'),'') 
    error('You must first source the sourceme file in ../../ ');
    exit();
end

addpath(getenv('AIO_LIB_DIR')) 

more off;

AIOUSB;
AIOUSB_GetVersion()
AIOUSB_GetVersionDate()
AIOUSB_Init();
deviceMask = GetDevices();
deviceIndex = 0;
AIOUSB_ListDevices();
productId = new_ulp();
ulp_assign(productId,0);
QueryDeviceInfo(0, productId,new_ulp(),"",new_ulp(), new_ulp());

if ulp_value( productId ) == 32792
    stopval = 16
elseif ulp_value( productId ) == 32796
    stopval = 8
else
    tmp = sprintf('Card with board id "0x%x" is not supported by this sample', ulp_value( productId ) );
    disp(tmp);
    exit();
end

timeout = 1000;
AIOUSB_Reset(  deviceIndex );
AIOUSB_SetCommTimeout( deviceIndex, timeout );

outData = new_usp();
usp_assign(outData, 15 );
readData = new_usp();
usp_assign(readData ,0);       


countval = 1;
num_per_bin = 10;
num_repetitions = 10;           
vals = zeros(1,(2^stopval)*num_repetitions/num_per_bin);
tic;
start = tic;
val=sprintf("Starting at %d",start);
disp(val);
%countval = 1;
%num_per_bin = 10;
%num_repetitions = 4;                   
for j = 1:1:num_repetitions
  for i=0:2^stopval-1
    usp_assign(outData, i );
    result = DIO_WriteAll( 0, outData );
    if mod(i, num_per_bin ) == 0 
      vals(countval) = toc();
      countval = countval + 1;
    end
  end
end
stop = toc();
val=sprintf('Completed in seconds %d\nCount per is %f',stop,stop/ ...
            (2^stopval-1)  );
disp(val);
diffvals=diff(vals);
val=sprintf('Mean per %d is %f', num_per_bin, mean(diffvals));
disp(val);
val=sprintf('Stdev per %d is %f', num_per_bin,  sqrt(var(diffvals)));
disp(val);
val=sprintf('Mean per call is %f', mean(diffvals/num_per_bin));
disp(val);
val=sprintf('Stdev per call is %f', sqrt(var(diffvals/num_per_bin)));
disp(val);
val=sprintf('Max per call is %f', max(diffvals/num_per_bin));
disp(val);
[n,bins] = hist(diffvals/num_per_bin)
end
