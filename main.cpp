#include "main.hpp"
using namespace std;


CanvasForCoord::CanvasForCoord(QWidget *parent) : QWidget(parent) {
    this->setWindowTitle("Canvas coords");
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setWindowState(Qt::WindowFullScreen); 
    this->x = (this->width() / 2) - int(this->widthRec / 2);
    this->y = (this->height() / 2) - int(this->heightRec / 2);

    this->setWindowOpacity(0.5); 
    this->setStyleSheet("background-color: black;"); 
    this->setMouseTracking(true);
}

    
void CanvasForCoord::paintEvent(QPaintEvent *event)  {
    QPainter painter(this);
    painter.setPen(Qt::red);
    painter.setBrush(Qt::NoBrush);
    
    
    painter.drawRect(x, y, widthRec, heightRec);
    painter.setBrush(Qt::red);
    //i need to make this move as well
    painter.drawEllipse(QPoint(x + widthRec / 2, y + heightRec / 2), 2, 2);

}

void CanvasForCoord::mousePressEvent(QMouseEvent *event)  {
    if(event->button() == Qt::LeftButton) {
        mousePos = event->pos();
        this->x = event->position().x() - int(widthRec / 2);
        this->y = event->position().y() - int(heightRec / 2);
        this->configCoords(x, y);
        this->update();
        
    }
}

void CanvasForCoord::keyPressEvent(QKeyEvent *event)  {
    if(event->key() == Qt::Key_Escape) {
        this->close();
        emit windowClosed();
    }
}

void CanvasForCoord::configCoords(int x, int y) {
    this->x = x;
    this->y = y;
    emit coordsChanged(x, y);
    this->update();
}




//organizar esta funcao
MainWindow::MainWindow(ClipboardData* data = nullptr, QWidget *parent) : QWidget(parent) {
    this->clipboardData = data;
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
    createHideButton();
    createInfoLabel();
    createDefinitionLabel();
    //depois quando for só para abrir caso o user clique tirar
    this->show();
    
    
    this->timer = new QTimer(this);
    this->canvas = new CanvasForCoord();
    QObject::connect(canvas, &CanvasForCoord::coordsChanged, this, &MainWindow::updateCoords);
    QObject::connect(canvas, &CanvasForCoord::windowClosed, this, [this]() {
        this->show();
    });


    QObject::connect(&button, &QPushButton::clicked, [this]() {
        this->hide();
        this->canvas->show();

    });

    QObject::connect(&hideButton, &QPushButton::clicked, [this]() {
        this->hide();
    });

    QObject::connect(timer, &QTimer::timeout, [this]() {
        //check se palavra é a mesma para n perder tempo
        lock_guard<mutex> lock(this->clipboardData->mutex);
        if(this->clipboardData->data.empty() || this->clipboardData->data == this->infoLabel->text().toStdString()) {
            return;
        }
        
        if(!this->isMinimized()){
            this->setWindowState(Qt::WindowNoState); 
            this->show();
            this->raise();
            this->activateWindow();
        }

        this->infoLabel->setText(QString::fromStdString(this->clipboardData->data));
        std::string definition = fetchWordDefinition(clipboardData->data);
        this->definitionLabel->setText(QString::fromStdString(apiOutput(definition)));
        
    });    

    this->timer->start(100); 
}

void MainWindow::setText(const QString& text) {
    this->infoLabel->setText(text);
}

void MainWindow::setClipboardData(ClipboardData* data) {
    this->clipboardData = data;
}

void MainWindow::updateCoords(int x, int y) {
    int newX = x - (this->widthM / 2);
    int newY = y - (this->heightM / 2);
    this->move(newX, newY);
    this->update();
}
        
void MainWindow::popWindow() {
    this->show();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape) {
        this->hide();
    }
}

void MainWindow::createConfigButton() {
    button.setText("Config");
    button.setParent(this);
    buttonCoords();
}

void MainWindow::buttonCoords() {
    button.move(this->widthM - 100, 0);
    button.resize(100, 30);
}

void MainWindow::createHideButton() {
    //implementar
    hideButton.setText("Hide");
    hideButton.setParent(this);
    hideButtonCoords();
        
}

void MainWindow::hideButtonCoords() {
    hideButton.move(this->widthM - 100, 30);
    hideButton.resize(100, 30);
}

void MainWindow::createInfoLabel() { //palavra selecionada
    this->infoLabel = new QLabel("WORDS", this);
    this->infoLabel->setParent(this);
    this->infoLabel->move(10, 10);
    this->infoLabel->resize(100, 20); //posso fazer dinamico dependendo no sizeword
    this->infoLabel->setStyleSheet("color: black; background-color: yellow; font-size: 18px; font-weight: bold;");
}

void MainWindow::createDefinitionLabel() { //definição da palavra
    this->definitionLabel = new QLabel("DEFINITION", this);
    this->definitionLabel->setParent(this);
    this->definitionLabel->move(10, 70);
    this->definitionLabel->resize(350, 200); //posso fazer dinamico dependendo no sizeword
    this->definitionLabel->setStyleSheet("color: black; background-color: lightgray; font-size: 12px; padding: 5px;");
    this->definitionLabel->setWordWrap(true);
    this->definitionLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->definitionLabel->setGeometry(10, 70, 350, 200);
}


void getAllDefs(const json& jsonData, string* definitions) {
    for (const auto& entry : jsonData){
        for (const auto& meaning : entry["meanings"]) {
            for (const auto& definition : meaning["definitions"]) {
                *definitions += definition["definition"].get<std::string>() + "\n";
                break;
            }
        }
    }
}

string apiOutput(const string& output) {
    try {
        json jsonData = json::parse(output);

        if(jsonData.is_object() && jsonData.contains("title") && jsonData["title"] == "No Definitions Found") {
            return "No definitions found.";
        }

        if(jsonData.is_array() && jsonData.empty()) {
            return "No definitions found.";
        }
        
        string definitions;
        
        //processar erros da library
        getAllDefs(jsonData, &definitions);
        return definitions;
    }
    catch (const nlohmann::json::parse_error& e) {
        printf("JSON Parse Error: %s\n", e.what());
        //quero falhar sem crash
        return "erro";
    }

}

void hotKeyThread(ClipboardData* clipboardData, MainWindow* mainWindow) {
    
    if(RegisterHotKey(NULL, 1, MOD_CONTROL, 'H')) { ///basicamente e a base de evento
        printf("sucesso\n");
    } else {
        printf("fail.\n");
        
    }
    MSG msg = {0};
    for(;;) {   
        if(GetMessage(&msg, NULL, 0, 0) > 0) {
            if(msg.message == WM_HOTKEY) {
                mainWindow->popWindow();
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
                Sleep(100); //proc controlc
                OpenClipboard(NULL);
                //perceber problemas de lock aqui

                const char* clipboard_ptr = (const char*)GetClipboardData(CF_TEXT);
                if(clipboard_ptr) {
                    clipboardData->data = std::string(clipboard_ptr);
                }
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
        string cleanWord = word;
        cleanWord.erase(cleanWord.find_last_not_of(" \n\r\t") + 1);
        cleanWord.erase(0, cleanWord.find_first_not_of(" \n\r\t"));
        std::string url = "https://api.dictionaryapi.dev/api/v2/entries/en/" + cleanWord;
        printf("URL : %s\n", url.c_str());
        
        
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
    
    //std::string result = fetchWordDefinition("hello");
    //printf("API Response: %s\n", result.c_str());
    
    MainWindow window(&clipboardData);

    thread keyMangment(hotKeyThread, &clipboardData, &window);
    keyMangment.detach();



    int result_code = app.exec();
    

    
    curl_global_cleanup();
    return result_code;
    
}
