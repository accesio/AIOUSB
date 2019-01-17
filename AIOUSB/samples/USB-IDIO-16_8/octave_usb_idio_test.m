#!/usr/bin/octave -q
# 
# $Date $Format: %ad$$
# $Author $Format: %an <%ae>$$
# $Release $Format: %h$$
# @brief Sample program to run the USB-DIO-16 from within Matlab
# @note This sample Triggers the relay switches. Note that the
# output is inverted such that if you write a '1' to a bit location
# then any LED connected to that pin will switch OFF, not ON
#
#
#

if strcmp(getenv('AIO_LIB_DIR'),'') 
    error('You must first source the sourceme file in ../../ ');
    exit();
end

addpath(getenv('AIO_LIB_DIR')) 

more off;

AIOUSB;
AIOUSB_GetVersion();
AIOUSB_GetVersionDate();
AIOUSB_Init();
deviceMask = GetDevices();
deviceIndex = 0;
AIOUSB_ListDevices()
productId = new_ulp()  ;
ulp_assign(productId,0);
QueryDeviceInfo(0, productId,new_ulp(),"",new_ulp(), new_ulp());

if ulp_value( productId ) == 32792
    stopval = 15;
elseif ulp_value( productId ) == 0x8012
    stopval = 7;
else
    tmp = sprintf('Card with board id "0x%x" is not supported by this sample', ulp_value( productId ) );
    disp(tmp);
    exit();
end

timeout = 1000;
AIOUSB_Reset(  deviceIndex );
AIOUSB_SetCommTimeout( deviceIndex, timeout );

outData = new_ulp();
ulp_assign(outData, 15 );

DIO_WriteAll( deviceIndex, outData );
readData = new_ulp();
ulp_assign(readData ,0);
if 1
for i=0:255
    val=sprintf('Sending %d',i);
    ulp_assign(outData, i );
    result = DIO_WriteAll( deviceIndex, outData );
    pause(0.04);
end

for i=0:stopval
    val=sprintf('Sending %d',2^i);
    ulp_assign(outData, 2^i );
    DIO_WriteAll(deviceIndex, outData );
    pause(1);    
end
end
ulp_assign(outData, 0x5465 );
DIO_WriteAll(deviceIndex, outData );
disp('Reading back data');
buf = DIOBuf(10)
DIO_ReadAllToDIOBuf( deviceIndex, buf );
val = sprintf("Binary was: %s", DIOBufToString(buf) );
disp(val);
val = sprintf("Hex was: %s", DIOBufToHex(buf) );
disp(val)
tmp = new_ucp();
for i=0:3
  DIO_Read8(deviceIndex,i, tmp );
  printf("read8 = %d\n", ucp_value(tmp) );
end
printf("-----\n");
for i=7:-1:0
        DIO_Read1(deviceIndex,i, tmp );
        printf("%d",ucp_value(tmp));
end
printf("\n-----\n");
for i=15:-1:8
        val = DIO_Read1(deviceIndex,i,tmp );
        printf("%d",ucp_value(tmp));
end


