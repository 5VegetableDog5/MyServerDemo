#include "config.h"


//日志信息窗口（全局变量）
extern QTextBrowser *logBrowser;

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
