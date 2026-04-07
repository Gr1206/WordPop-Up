#include <stdio.h>
#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QString>
#include <QTimer>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <windows.h>
#include <thread>
#include <mutex>
#include <string>
using namespace std;

struct ClipboardData {
    string data;
    mutex mutex;
};

class canvasForCoord : public QWidget {
    private: //atribute
        bool moving = false;
        QPoint mousePos;
        int widthRec = 20, heightRec = 10;
        int x, y;

        public:
        canvasForCoord(QWidget *parent = nullptr) : QWidget(parent) {
            this->setWindowTitle("Canvas coords");
            this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
            this->setWindowState(Qt::WindowFullScreen); 
            this->x = (this->width() / 2) - int(this->widthRec / 2);
            this->y = (this->height() / 2) - int(this->heightRec / 2);

            this->setWindowOpacity(0.5); 
            this->setStyleSheet("background-color: black;"); 
            this->setMouseTracking(true);
            paintEvent(nullptr);
            
        

            qDebug() << "Canvas criado e pronto para detetar o rato!";
    }
    protected:
        void paintEvent(QPaintEvent *event) override {
            QPainter painter(this);
            painter.setPen(Qt::red);
            painter.setBrush(Qt::NoBrush);
            
            
            painter.drawRect(x, y, widthRec, heightRec);
            painter.setBrush(Qt::red);
            painter.drawEllipse(QPoint(this->width() / 2, this->height() / 2), 2, 2);

        }
    //draw the coordinates cursor




    //protected:
    //mouse events
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
    
    QPushButton button("Click me", &window);
    button.move(110, 100);
    button.resize(100, 30);
    window.show();
    
    canvasForCoord* canvas = new canvasForCoord();
    
    thread keyMangment(hotKeyThread, &clipboardData);
    
    keyMangment.detach();

    QObject::connect(&button, &QPushButton::clicked, [canvas]() {
        canvas->show();
        canvas->raise();
        canvas->activateWindow();
    });

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&clipboardData, &window](){
        lock_guard<mutex> lock(clipboardData.mutex);
        window.setText(QString::fromStdString(clipboardData.data));
        window.show(); 
    });    

    timer.start(100); 
    return app.exec();
    
}