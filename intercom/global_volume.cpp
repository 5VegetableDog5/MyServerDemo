#include "config.h"
#include "callingclientsuiitem.h"

#if DEBUG
//日志信息窗口（全局变量）
extern QTextBrowser *logBrowser;
#endif

QList<CallingClientsUIItem*> uiCallingItems;

#if DEBUG
//将指定的字符串打印到窗口中
void printfLog(QString string){
    if(logBrowser!=nullptr){
        logBrowser->append(string);
        // Move the cursor to the end of the document
        static QTextCursor cursor = logBrowser->textCursor();
        cursor.movePosition(QTextCursor::End);

        // Ensure the cursor is visible
        logBrowser->setTextCursor(cursor);
        logBrowser->ensureCursorVisible();
    }
}
#endif

#if MONITOR

/*
@说明：将当前所有通话监听设置为否，再由具体的按钮将具体的uiCallingItem通话设置为监听
*/
bool closeMONITOR(){

    for(int i=0; i<uiCallingItems.count();i++){
        uiCallingItems[i]->closeMonitor();
    }

    return true;
}

#endif
