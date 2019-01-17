function [n,bins,diffvals] = matlab_usb_idio_speed_test( num_repetitions );
% USB_IDIO_SPEED_TEST Plot variable correlations
%
% Syntax:
%
%   matlab_usb_idio_speed_test(X)
%   matlab_usb_idio_speed_test(X,param,val,...)
%   [n,freq,diffvals] = matlab_usb_idio_speed_test(...)
%
% Description:
%
%  Runs a stress test on ACCES I/O's USB-IDIO series of cards
%
% Input Arguments:
%
%   num_repetitions - Number of times to rerun the test for
%   increasing averaging.
%
% $Date $Format: %ad$$
% $Author $Format: %an <%ae>$$
% $Release $Format: %h$$
% @brief Sample program to run the USB-DIO-16 from within Matlab
% @note This sample Triggers the relay switches. Note that the
% output is inverted such that if you write a '1' to a bit location
% then any LED connected to that pin will switch OFF, not ON
%

if strcmp(getenv('AIO_LIB_DIR'),'') 
    error('You must first source the sourceme file in ../../ ');
    exit();
end

addpath(getenv('AIO_LIB_DIR'));
rootlibdir=getenv('AIO_LIB_INSTALL_DIR');
libaiousb=strcat(rootlibdir,'/libaiousb');

loadlibrary(libaiousb,'libaiousb.h', 'includepath','/usr/include/libusb-1.0', 'includepath',getenv('AIO_LIB_DIR'));

% To see the other functions offered by this library , you can run
% the following
% 
% libfunctions libaiousb -full
%
%

calllib('libaiousb','AIOUSB_GetVersion');
calllib('libaiousb','AIOUSB_GetVersionDate');
result = calllib('libaiousb','AIOUSB_Init' );
deviceMask = calllib('libaiousb','GetDevices');
deviceIndex = 0;
calllib('libaiousb','AIOUSB_ListDevices');      
productId = libpointer('ulongPtr',0);
calllib('libaiousb','QueryDeviceInfo',0,productId,[],libpointer('cstring'),[],[]);

if productId.Value == 32792
    stopval = 16;
elseif productId.Value == 32796
    stopval = 8;
else
    tmp = sprintf('Card with board id "0x%x" is not supported by this sample', productId.Value );
    disp(tmp);
    exit();
end

timeout = 1000;
result = calllib('libaiousb','AIOUSB_Reset', deviceIndex );
result = calllib('libaiousb','AIOUSB_SetCommTimeout',deviceIndex, timeout );
format long;

start = tic;
val=sprintf('Starting at %d',start);
disp(val);
countval = 1;
num_per_bin = 10;
num_repetitions = 4;                   
vals = zeros(1,(2^stopval)*num_repetitions/num_per_bin);
for j = 1:1:num_repetitions
    for i=0:2^stopval-1
        result = calllib('libaiousb','DIO_WriteAll', 0, libpointer('uint16',i) );
        if mod(i, num_per_bin ) == 0 
            vals(countval) = toc(start);
            countval = countval + 1;
        end
    end
end
stop = toc(start);
val=sprintf('Stopped at %d',stop);
disp(val);
diffvals=diff(vals);
val=sprintf('Completed in seconds %d\nCount per is %f',stop,stop/ ...
            (2^stopval-1)  );
disp(val)
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




