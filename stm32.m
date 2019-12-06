function varargout = stm32(varargin)
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @stm32_OpeningFcn, ...
                   'gui_OutputFcn',  @stm32_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end
if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
end

% --- Executes just before stm32 is made visible.
function stm32_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to stm32 (see VARARGIN)

% Choose default command line output for stm32
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes stm32 wait for user response (see UIRESUME)
% uiwait(handles.figure1);
end


function varargout = stm32_OutputFcn(hObject, eventdata, handles) 
varargout{1} = handles.output;
end

function RESET_Callback(hObject, eventdata, handles)
while true
    delete(instrfindall);
    global s
    try
        s=serial('com3');
        set(s,'BaudRate',9600,'DataBits',8,'StopBits',1,'Parity','none','FlowControl','none')
        s.BytesAvailableFcnMode='byte';
        %     s.ReadAsyncMode = 'manual';
        fopen(s);
        %     readasync(s)
        global t;t=0;
        out=fread(s,9,'uint8');
        temperaturestr=char(out(1:2))';
        temperature=str2double(temperaturestr);%温度值
        humiditystr=char(out(3:4))';
        humidity=str2double(humiditystr);%湿度值
        voltagestr=char(out(5:9))';
        voltage=str2double(voltagestr);%电压值
        if voltagestr(2) ~='.'
            continue
        end
        set(handles.temperature,'String',temperaturestr);
        set(handles.humidity,'String',humiditystr);
        hs=msgbox('找到COM3！','成功','Help')
        ht = findobj(hs, 'Type', 'text');
        set(ht, 'FontSize', 15, 'Unit', 'normal');
        % 改变对话框大小
        set(hs, 'Resize', 'on'); % 手动改变
        break;
    catch
        hs=msgbox({'未找到COM3！','请检查端口是否打开或被占用！'},'出错了(/ □ \)','Error')
        ht = findobj(hs, 'Type', 'text');
        set(ht, 'FontSize', 11, 'Unit', 'normal');
        % 改变对话框大小
        set(hs, 'Resize', 'on'); % 手动改变
        break
    end
end
end

function V1_Callback(hObject, eventdata, handles)
end


function V1_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
end


function axes1_CreateFcn(hObject, eventdata, handles)
end



function READ_Callback(hObject, eventdata, handles)
global t;global s;
while true
    delete(instrfindall);
    global s
    s=serial('com3');
    set(s,'BaudRate',9600,'DataBits',8,'StopBits',1,'Parity','none','FlowControl','none')
    s.BytesAvailableFcnMode='byte';
    fopen(s);
    out=fread(s,9,'uint8');
    temperaturestr=char(out(1:2))';
    temperature=str2double(temperaturestr);%温度值
    humiditystr=char(out(3:4))';
    humidity=str2double(humiditystr);%湿度值
    voltagestr=char(out(5:9))';
    voltage=str2double(voltagestr);%电压值
    if voltagestr(2) ~= '.' %校验数据格式是否准确
        continue
    end
    set(handles.temperature,'String',temperaturestr);
    set(handles.humidity,'String',humiditystr);
    if t==0
        set(handles.V1,'String',strcat(voltagestr,'V'));t=t+1;
    elseif t==1
        set(handles.V2,'String',strcat(voltagestr,'V'));t=t+1;
    elseif t==2
        set(handles.V3,'String',strcat(voltagestr,'V'));t=0;
    end
    break
end
end


function V2_Callback(hObject, eventdata, handles)
end
function V2_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
end



function PLOT_Callback(hObject, eventdata, handles)
delete(instrfindall);
global s
s=serial('com3');
set(s,'BaudRate',9600,'DataBits',8,'StopBits',1,'Parity','none','FlowControl','none')
s.BytesAvailableFcnMode='byte';
fopen(s);
global s
tt=0;
cla reset
tic
V=[];tic
while tt<100
    out=fread(s,9,'uint8');%九个原始ASCII数据
    if char(out(6)) ~= '.'
        continue
    end
    temperaturestr=char(out(1:2))';
    temperature=str2double(temperaturestr);%温度值
    humiditystr=char(out(3:4))';
    humidity=str2double(humiditystr);%湿度值
    voltagestr=char(out(5:9))';
    voltage=str2double(voltagestr);%电压值
    V=[V voltage];%电压值矩阵
    tt=tt+1;
end
V
plot([0:99],V,'-o'),
axis([0 100 -0.3 3.5]);grid on;ylabel('电压值/V');
set(gca,'ytick',-0.3:0.1:3.5);
set(handles.TIME,'String',toc);
time=string(fix(clock))%字符串数组格式储存的日期时间
easystr=strcat(time(1),'年',time(2),'月',time(3),'日',time(4),'时',time(5),'分',time(6),'秒','生成');
set(handles.systemtime,'String',easystr);
end

function TIME_Callback(hObject, eventdata, handles)
end
function TIME_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
end



function V3_Callback(hObject, eventdata, handles)
end
function V3_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
end



function systemtime_Callback(hObject, eventdata, handles)
% hObject    handle to systemtime (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of systemtime as text
%        str2double(get(hObject,'String')) returns contents of systemtime as a double
end

% --- Executes during object creation, after setting all properties.
function systemtime_CreateFcn(hObject, eventdata, handles)
% hObject    handle to systemtime (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
end



function temperature_Callback(hObject, eventdata, handles)
% hObject    handle to temperature (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of temperature as text
%        str2double(get(hObject,'String')) returns contents of temperature as a double
end

% --- Executes during object creation, after setting all properties.
function temperature_CreateFcn(hObject, eventdata, handles)
% hObject    handle to temperature (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

end

function humidity_Callback(hObject, eventdata, handles)
% hObject    handle to humidity (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of humidity as text
%        str2double(get(hObject,'String')) returns contents of humidity as a double
end

% --- Executes during object creation, after setting all properties.
function humidity_CreateFcn(hObject, eventdata, handles)
% hObject    handle to humidity (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
end


% --- Executes on button press in clearV123.
function clearV123_Callback(hObject, eventdata, handles)
% hObject    handle to clearV123 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
set(handles.V1,'String','')
set(handles.V2,'String','')
set(handles.V3,'String','')
global t;t=0;
end


% --- Executes on button press in help.
function help_Callback(hObject, eventdata, handles)
% hObject    handle to help (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
hs=msgbox({'电压采样间隔：0.05s';'温度采样间隔:1s';'波形准确范围0.3Hz~10Hz';'作者：朱永辉 '},'帮助','Help')
ht = findobj(hs, 'Type', 'text');
set(ht, 'FontSize', 9, 'Unit', 'normal');
% 改变对话框大小
set(hs, 'Resize', 'on'); % 手动改变
end


% --------------------------------------------------------------------
function Untitled_1_Callback(hObject, eventdata, handles)
% hObject    handle to Untitled_1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
end


% --- Executes on button press in release.
function release_Callback(hObject, eventdata, handles)
% hObject    handle to release (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
delete(instrfindall);
hs=msgbox({'COM3已释放！'},'成功','Help')
ht = findobj(hs, 'Type', 'text');
set(ht, 'FontSize', 15, 'Unit', 'normal');
% 改变对话框大小
set(hs, 'Resize', 'on'); % 手动改变
end