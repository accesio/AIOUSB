% 
% $Date $Format: %ad$$
% $Author $Format: %an <%ae>$$
% $Release $Format: %h$$
% @brief Sample program to run the USB-DIO-16 from within Matlab
% @note This sample Triggers the relay switches. Note that the
% output is inverted such that if you write a '1' to a bit location
% then any LED connected to that pin will switch OFF, not ON
%
%
%

if strcmp(getenv('AIO_LIB_DIR'),'') 
    error('You must first source the sourceme file in ../../ ');
    exit();
end

addpath(getenv('AIO_LIB_DIR')) 
addpath(getenv('AIO_LIB_DIR'));
rootlibdir=getenv('AIO_LIB_INSTALL_DIR');
libaiousb=strcat(rootlibdir,'/libaiousb');

loadlibrary(libaiousb,'libaiousb.h', 'includepath','/usr/include/libusb-1.0', 'includepath',getenv('AIO_LIB_DIR'))

% To see the other functions offered, you can run this
% 
% libfunctions libaiousb -full
%
%

calllib('libaiousb','AIOUSB_GetVersion')
calllib('libaiousb','AIOUSB_GetVersionDate')
result = calllib('libaiousb','AIOUSB_Init' );
deviceMask = calllib('libaiousb','GetDevices');
deviceIndex = 0;
calllib('libaiousb','AIOUSB_ListDevices')          
productId = libpointer('ulongPtr',0)  
calllib('libaiousb','QueryDeviceInfo',0,productId,[],libpointer('cstring'),[],[])

if productId.Value == 32792
    stopval = 15
elseif productId.Value == 32796
    stopval = 7
else
    tmp = sprintf('Card with board id "0x%x" is not supported by this sample', productId.Value );
    disp(tmp);
    exit();
end

timeout = 1000;
result = calllib('libaiousb','AIOUSB_Reset', deviceIndex )
result = calllib('libaiousb','AIOUSB_SetCommTimeout',deviceIndex, timeout );


get(productId)
%    Value: 32796
% DataType: 'ulongPtr'

timeout = 1000



outData = libpointer('uint16Ptr',16) 
result = calllib('libaiousb','DIO_WriteAll', 0,  outData );
readData = libpointer('uint16Ptr',0 )



result = calllib('libaiousb','DIO_WriteAll', deviceIndex,  outData );
for i=0:255
    val=sprintf('Sending %d',i);
    result = calllib('libaiousb','DIO_WriteAll', 0, libpointer('uint16',i) );
    disp(val)
    pause(0.04);
end

for i=0:stopval
    val=sprintf('Sending %d',2^i);
    result = calllib('libaiousb','DIO_WriteAll', 0, libpointer('uint16',2^i) );
    disp(val);
    pause(1);    
end
readData = calllib('libaiousb','NewDIOBuf', 10 );

result = calllib('libaiousb','DIO_ReadIntoDIOBuf', deviceIndex, readData );

calllib('libaiousb','DIOBufToString', readData ) 
result = calllib('libaiousb','DIO_WriteAll', 0, libpointer('uint16', 21605  ) );
result = calllib('libaiousb','DIO_ReadIntoDIOBuf', deviceIndex, readData );
calllib('libaiousb','DIOBufToString', readData );
val = sprintf('String was: %s', calllib('libaiousb','DIOBufToString',readData));
disp(val);
val = sprintf('Binary was: %s', calllib('libaiousb','DIOBufToBinary',readData));
disp(val);
val = sprintf('Hex was: %s', calllib('libaiousb','DIOBufToHex',readData ));
disp(val);


tmp = 0;
[retcode,tmp] = calllib('libaiousb', 'DIO_Read8', deviceIndex,0, tmp );
val = sprintf('read8 = %d\n', tmp );
disp(val);
[retcode,tmp] = calllib('libaiousb', 'DIO_Read8', deviceIndex,1, tmp );
val = sprintf('read8 = %d\n', tmp );
disp(val);
[retcode,tmp] = calllib('libaiousb', 'DIO_Read8', deviceIndex,2, tmp );
val = sprintf('read8 = %d\n', tmp );
disp(val);
[retcode,tmp] = calllib('libaiousb', 'DIO_Read8', deviceIndex,3, tmp );
val = sprintf('read8 = %d\n', tmp );
disp(val);
disp('-----');
val = '';
for i=7:-1:0
    [ret,tmp] = calllib('libaiousb', 'DIO_Read1',deviceIndex,i,tmp );
    val = strcat(val, sprintf('%d',tmp));
end
disp(val);
val = '';
disp('-----');
for i=15:-1:8
    [ret,tmp] = calllib('libaiousb', 'DIO_Read1',deviceIndex,i,tmp );
    val = strcat(val, sprintf('%d',tmp));
end
disp(val);

