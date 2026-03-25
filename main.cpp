#include <stdio.h>
#include <windows.h>

int main() {
    
    if(RegisterHotKey(NULL, 1, MOD_CONTROL, 'H')) { ///basicamente e a base de evento
        printf("sucesso\n");
    } else {
        printf("fail.\n");
        return 1;
    }
    MSG msg = {0};
    for(;;) {   
        if(GetMessage(&msg, NULL, 0, 0) > 0) {
            if(msg.message == WM_HOTKEY) {
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
                printf("Clipboard: %s\n", (char*)GetClipboardData(CF_TEXT));
                CloseClipboard();
            }
        } else {
            printf("fail.\n");
            break; 
        }
    }
 
 
    return 0;
}