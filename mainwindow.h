#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml>
#include <QRegExp>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_btnNewItem_clicked();

    void on_listWidget_currentRowChanged(int currentRow);

    void on_tableWidget_cellChanged(int row, int column);

    void on_actionSave_triggered();

    void on_btnDeleteItem_clicked();

    void on_btnNewVar_clicked();

    void on_btnDeleteVar_clicked();

    void on_actionExit_triggered();

    void on_actionNew_triggered();

    void on_actionSaveAs_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    QDomDocument xmlDoc;
    QString fileName;
    bool changes;
    bool refreshingTable;

    void refreshItemsTable(int row);
    void fillList();
};

#endif // MAINWINDOW_H
