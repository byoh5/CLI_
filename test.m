%% Test 
% clear all; 
close all; clc;
%% Global variables
% global nTimes
%  
% if(nTimes<1),
%     nTimes = 5;
% end
% keyboard
bCreateLibrary = 1; % 1: create MATLAB library, 0: deployed application
CLI_DLL = 'CLI';


%% Load library
if(libisloaded(CLI_DLL))
    unloadlibrary(CLI_DLL);
end

if(bCreateLibrary),
    % To create a prototype MATLAB file
    loadlibrary([CLI_DLL,'.dll'], 'CLI.h','mfilename','CLI_proto');
    libfunctionsview CLI;
else
    % In the deployed application    
    loadlibrary([CLI_DLL,'.dll'], @CLI_proto);
end


%% Call library:
% Sends one of the following keys
% 1 - LEFT
% 2 - RIGHT
% 3 - UP
% 4 - DOWN
    time = 100;
    for t=1:time
    fd_data = calllib(CLI_DLL, 'NetCon','169.254.169.188','5556');
    bufsizen= 1024;
    bufnew=libpointer('stringPtr',blanks(bufsizen));
    ret = calllib(CLI_DLL, 'getDataFromRemote',fd_data,134217728, 1024,3,bufnew);
    calllib(CLI_DLL, 'NetClo',fd_data);
    end
    
fd = calllib(CLI_DLL, 'NetCon','169.254.169.188','5556');

if(fd>0),
    nTimes =10;
    nFrames = 30;

    for k=1:nTimes
        ret = calllib(CLI_DLL, 'getImagefromRemote',fd,nFrames);
        if ret<0
            unloadlibrary(CLI_DLL);
            return
        end    
        for n = 0:nFrames-1
            bufsize = calllib(CLI_DLL, 'getImgSizeFromList',n);
            bufIn=libpointer('stringPtr',blanks(bufsize));
            [err, buf] = calllib(CLI_DLL,'getImgDataFromList',n,bufIn);
            setdatatype(buf,'uint8Ptr',1,bufsize);
            img = decodeJpeg(buf.Value);
            figure(1);
            imshow(img);
            title(sprintf('frame %d/%d',k,nFrames));
        end
        
        calllib(CLI_DLL, 'deleteList_ALL');
    end

    calllib(CLI_DLL, 'NetClo',fd);
end

%% Unload library
unloadlibrary(CLI_DLL);

