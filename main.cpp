#include <stdio.h>
#include <QApplication>
#include <QWidget>
#include <QString>
#include <QTimer>
#include <QLabel>
#include <windows.h>
#include <thread>
#include <mutex>
#include <string>
using namespace std;

struct ClipboardData {
    string data;
    mutex mutex;
};



void hotKeyThread(ClipboardData* clipboardData){
    
    if(RegisterHotKey(NULL, 1, MOD_CONTROL, 'H')) { ///basicamente e a base de evento
        printf("sucesso\n");
    } else {
        printf("fail.\n");
        
    }
    MSG msg = {0};
    for(;;) {   
        if(GetMessage(&msg, NULL, 0, 0) > 0) {
            if(msg.message == WM_HOTKEY) {
                lock_guard<mutex> lock(clipboardData->mutex);
                printf("pressed\n");
                Sleep(100); //interferencia com o controlH
                INPUT input[4] = {};
                input[0].type = INPUT_KEYBOARD;
                input[0].ki.wVk = VK_CONTROL;

                input[1].type = INPUT_KEYBOARD;
                input[1].ki.wVk = 'C'; 

                //soltar
                input[2].type = INPUT_KEYBOARD;
                input[2].ki.wVk = 'C';
                input[2].ki.dwFlags = KEYEVENTF_KEYUP;
                
                input[3].type = INPUT_KEYBOARD;
                input[3].ki.wVk = VK_CONTROL;
                input[3].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(4, input, sizeof(INPUT));
                OpenClipboard(NULL);
                //perceber problemas de lock aqui
                clipboardData->data = (char*)GetClipboardData(CF_TEXT);
                printf("Clipboard: %s\n", clipboardData->data.c_str());
                CloseClipboard();
                
            }
        } else {
            printf("fail.\n");
            break; 
        }

    }
}



int main(int argc, char *argv[]) {
    
    ClipboardData clipboardData;
    QApplication app(argc, argv);
    QLabel window;
    window.resize(320, 240);
    window.setWindowTitle("Qt Window Example");
    window.show();
    
    thread keyMangment(hotKeyThread, &clipboardData);
    
    keyMangment.detach();

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&clipboardData, &window](){
        lock_guard<mutex> lock(clipboardData.mutex);
        window.setText(QString::fromStdString(clipboardData.data));
        window.show(); 
    });    

    timer.start(100); 
    return app.exec();
    
}