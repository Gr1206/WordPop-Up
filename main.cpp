#include <stdio.h>
#include <curl/curl.h>
#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QString>
#include <QTimer>
#include <QLabel>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>
#include <windows.h>
#include <thread>
#include <mutex>
#include <string>
using namespace std;

class MainWindow;

struct ClipboardData {
    string data;
    mutex mutex;
};

class canvasForCoord : public QWidget {
    Q_OBJECT
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
            

    }
    protected:
        void paintEvent(QPaintEvent *event) override {
            QPainter painter(this);
            painter.setPen(Qt::red);
            painter.setBrush(Qt::NoBrush);
            
            
            painter.drawRect(x, y, widthRec, heightRec);
            painter.setBrush(Qt::red);
            //i need to make this move as well
            painter.drawEllipse(QPoint(x + widthRec / 2, y + heightRec / 2), 2, 2);

        }
    //draw the coordinates cursor

        void mousePressEvent(QMouseEvent *event) override {
            if(event->button() == Qt::LeftButton) {
                moving = true;
                mousePos = event->pos();
                this->x = event->position().x() - int(widthRec / 2);
                this->y = event->position().y() - int(heightRec / 2);
                this->configCoords(x, y);
                this->update();
                
            }
        }

        void keyPressEvent(QKeyEvent *event) override {
            if(event->key() == Qt::Key_Escape) {
                this->close();
                emit windowClosed();
            }
        }
        
        void configCoords(int x, int y) {
            this->x = x;
            this->y = y;
            emit coordsChanged(x, y);
            this->update();
        }

        signals :
            void windowClosed(); 
            void coordsChanged(int x, int y);



    
        
};

class MainWindow : public QWidget {
    private: 
        int widthM;
        int heightM;
        QPushButton button;
        canvasForCoord* canvas;
        QTimer *timer;
        ClipboardData* clipboardData;
        QLabel *infoLabel;
    public:
        MainWindow(ClipboardData* data = nullptr, QWidget *parent = nullptr) : QWidget(parent) {
            this->clipboardData = data;
            this->widthM = 400;
            this->heightM = 300;
            this->setWindowTitle("Qt Window Example");
            //this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
            this->resize(widthM, heightM);
            this->setStyleSheet(
                "QWidget { background-color: white;}"
                "QPushButton { background-color: black; padding: 5px; opacity: 0.8; }"
                "QPushButton:hover { background-color: #555; }"
                "QLabel { font-size: 14px; font-weight: bold; }"
            );
            this->setWindowOpacity(0.7);
            
            createConfigButton();
            createInfoLabel();
            this->show();
            this->timer = new QTimer(this);
            this->canvas = new canvasForCoord();
            QObject::connect(canvas, &canvasForCoord::coordsChanged, this, &MainWindow::updateCoords);
            QObject::connect(canvas, &canvasForCoord::windowClosed, this, [this]() {
                this->show();
            });


            QObject::connect(&button, &QPushButton::clicked, [this]() {
                this->hide();
                this->canvas->show();

            });

            QObject::connect(timer, &QTimer::timeout, [this]() {
                //check se palavra é a mesma para n perder tempo
                lock_guard<mutex> lock(this->clipboardData->mutex);
                this->infoLabel->setText(QString::fromStdString(this->clipboardData->data));
            });    

            this->timer->start(100); 
        }
        
        void setText(const QString& text) {
            this->infoLabel->setText(text);
        }

        void setClipboardData(ClipboardData* data) {
            this->clipboardData = data;
        }

        void updateCoords(int x, int y) {
            int newX = x - (this->widthM / 2);
            int newY = y - (this->heightM / 2);
            this->move(newX, newY);
            this->update();
        }

        private:
            void createConfigButton() {
                button.setText("Config");
                button.setParent(this);
                buttonCoords();
            }

            void buttonCoords() {
                button.move(this->widthM - 100, 0);
                button.resize(100, 30);
            }

            void createInfoLabel() { //palavra selecionada
                this->infoLabel = new QLabel("WORDS", this);
                this->infoLabel->setParent(this);
                this->infoLabel->move(10, 10);
                this->infoLabel->resize(20, 20); //posso fazer dinamico dependendo no sizeword
                this->infoLabel->setStyleSheet("color: red; background-color: yellow;");
            }
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


size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string fetchWordDefinition(const std::string& word) {
    CURL* curl = curl_easy_init();
    std::string readBuffer;
    
    if(curl) {
        std::string url = "https://api.dictionaryapi.dev/api/v2/entries/en/" + word;
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            printf("CURL Error: %s\n", curl_easy_strerror(res));
            readBuffer = "Erro ao conectar à API";
        }
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

int main(int argc, char *argv[]) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    ClipboardData clipboardData;
    
    QApplication app(argc, argv);
    
    std::string result = fetchWordDefinition("hello");
    printf("API Response: %s\n", result.c_str());
    thread keyMangment(hotKeyThread, &clipboardData);
    keyMangment.detach();

    MainWindow window(&clipboardData);

    window.show();    

    int result_code = app.exec();
    

    fflush(stdout);
    curl_global_cleanup();
    return result_code;
    
}
#include "main.moc"