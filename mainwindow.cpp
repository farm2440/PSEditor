#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QStringList>
#include <QTextStream>
#include <dlgNewItem.h>
#include "dlgabout.h"

MainWindow::MainWindow(QWidget *parent) :    QMainWindow(parent),    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(3);

    QStringList headers;
    headers.append(tr("Променлива"));
    headers.append(tr("Стойност"));
    headers.append(tr("Свободен"));
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    fileName="";
    changes=false;
    qDebug() << "changes=true : 7";
    refreshingTable=false;
    this->setWindowTitle("PSEditor");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Отвори файл"), "C:\\TFTP-Root", tr("*.xml"));

    qDebug() << "fileName=" << fileName;
    //Зареждата се настройките от settings.xml
    QFile xmlFile(fileName);
    if(! xmlFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "ERROR: Unable to open xml file for reading";
        QMessageBox::critical(this, tr("ГРЕШКА"), tr("Липсва xml файл"));
    }
    else
    {
        qDebug() << "Opened xml file ...";
        xmlDoc.setContent(&xmlFile);
        xmlFile.close();
        this->setWindowTitle("PSEditor: " + fileName);
        fillList();

        ui->btnDeleteItem->setEnabled(true);
        ui->btnDeleteVar->setEnabled(true);
        ui->btnNewItem->setEnabled(true);
        ui->btnNewVar->setEnabled(true);
    }
}

void MainWindow::on_btnNewItem_clicked()
{
    DlgNewItem dlg;
    QString newItem;
    bool result;

     result = dlg.exec();
     if(result==QDialog::Rejected) return;
     newItem = dlg.getItemName().trimmed();
     if(newItem.length()==0) return;

    for(int i=0 ; i!=ui->listWidget->count() ; i++)
    {
        if(ui->listWidget->item(i)->text()== newItem)
        {
            QMessageBox::warning(this,tr("ГРЕШКА!"), tr("Вече има артикул с такова име!"));
            return;
        }
    }
    ui->listWidget->addItem(newItem);

    QDomElement root = xmlDoc.documentElement();
    QDomElement newNode = xmlDoc.createElement("item");
    QDomElement newChild = xmlDoc.createElement("title");
    QDomText value = xmlDoc.createTextNode(newItem);
    newChild.appendChild(value);
    newNode.appendChild(newChild);
    root.appendChild(newNode);
    changes=true;
    qDebug() << "changes=true : 6";
    QString winti = this->windowTitle();
    if(!winti.endsWith("*")) this->setWindowTitle(winti + "*");
}

void MainWindow::on_btnDeleteItem_clicked()
{
    if(ui->listWidget->currentRow()==-1) return;
    QDomElement root = xmlDoc.documentElement();
    QString itemName = ui->listWidget->currentItem()->text();
    QDomNodeList items = root.elementsByTagName("item");

    if(items.count()==0)
    {
        qDebug() << " Nyama izbran element za iztrivane.";
        QMessageBox::critical(this, tr("ГРЕШКА"), tr("Няма избран артикул!"));
        return;
    }

    for(int i=0 ; i!= items.count() ; i++)
    {
        QDomNodeList titles = items.item(i).toElement().elementsByTagName("title");
        QString title = titles.item(0).toElement().text();

        if(title==itemName)
        {
            qDebug() << "removing " << titles.item(0).toElement().tagName() << " : " << titles.item(0).toElement().text();
            root.removeChild(items.item(i));
            changes=true;
            qDebug() << "changes=true : 5";
            QString winti = this->windowTitle();
            if(!winti.endsWith("*")) this->setWindowTitle(winti + "*");
            fillList();
            refreshItemsTable(-1);
            return;
        }
    }
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    refreshItemsTable(currentRow);
}

void MainWindow::refreshItemsTable(int row)
{
    /* Актуализира съдържанието на таблицата с променливите
    // Прочита xml документа и нанася в таблицата променливите за текущо избрания
    // артикул. Първата колона е име на променлива, втората е стойност и третата
    // е checkbox който ако е маркиран, значи потребителя може да въведе друга
    // стойност при печат на принтсървъра
    */
    refreshingTable=true;
    ui->tableWidget->clear();
    while(ui->tableWidget->rowCount()!=0) ui->tableWidget->removeRow(0);

    QStringList headers;
    headers.append(tr("Променлива"));
    headers.append(tr("Стойност"));
    headers.append(tr("Свободен"));
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    if(row==-1) return;

    QDomNodeList items = xmlDoc.elementsByTagName("item");
    QDomNodeList vars = items.item(row).childNodes();
    if(vars.count()==0) return;
    for(int i=0 ; i!= vars.count() ; i++)
    {        
        QString vName = vars.at(i).toElement().tagName();
        QString vValue = vars.at(i).toElement().text();
        if(vName=="title") continue;
        ui->tableWidget->insertRow(0);
        QTableWidgetItem *itm = new QTableWidgetItem(vName);
        itm->setFlags(0);
        ui->tableWidget->setItem(0,0,itm);
        itm= new QTableWidgetItem(vValue);
        ui->tableWidget->setItem(0,1,itm);   
        itm = new QTableWidgetItem();
        if(vars.at(i).toElement().attribute("free")=="1") itm->setCheckState(Qt::Checked);
        else itm->setCheckState(Qt::Unchecked);;
        ui->tableWidget->setItem(0,2,itm);
    }
    refreshingTable=false;
}

void MainWindow::fillList()
{
    ui->listWidget->setCurrentRow(-1);
    ui->listWidget->clear();
    //http://3gfp.com/wp/2014/07/three-ways-to-parse-xml-in-qt/
    QDomNodeList items = xmlDoc.elementsByTagName("item");
    for (int i = 0; i < items.size(); i++) //обхождат се артикулите
    {
        QDomNode itm = items.item(i);
        QDomNodeList vars = itm.childNodes();
        for(int j=0 ; j<vars.count() ; j++) //обхождат се данните за артикула и се запълв combo с имената на артикулите
        {
            QDomNode var = vars.at(j);
            QDomElement elm = var.toElement();
            if(elm.tagName() == "title") ui->listWidget->addItem(elm.text());
        }
    }
}

void MainWindow::on_tableWidget_cellChanged(int row, int column)
{
    QRegExp reXmlForbidden("[&,<,>]");
    if(refreshingTable) return; // Aко промяната е поради работа на функцията refreshItemsTable()
                                // този флаг е вдигнат. Така се отработва събитие само когато е промяната е от потребителя.

    //item е лемента в който има променлива title="текущ артикул"
    //var е променливата която е била редактирана

    qDebug() << " VARIABLE IS CHANGED!";

    QString varName = ui->tableWidget->item(row, 0)->text();
    QString title = ui->listWidget->currentItem()->text();
    QString varNewValue = ui->tableWidget->item(row, 1)->text();
    QString free;
    if(ui->tableWidget->item(row,2)->checkState()==Qt::Checked) free="1";
    else free="0";
    qDebug() << " item title =" << title;
    qDebug() << " varName=" << varName << "  new value=" << varNewValue << " free=" << free;

    //Проверка за xml несъвместими символи
    if(varNewValue.contains(reXmlForbidden))
    {
        QMessageBox::warning(this,tr("ПРЕДУПРЕЖДЕНИЕ"),tr("Полето съдържа недопустими символи,\nкоито ще бъдат премахнати!"));
        varNewValue = varNewValue.remove(reXmlForbidden);

    }

    // Намирам елемента <item> с съдържащ <title>=<избрания вляво артикул>
    QDomElement root = xmlDoc.documentElement();
    QDomNodeList items = root.elementsByTagName("item");
    qDebug() << "root.elementsByTagName(\"item\") found " << items.count() << " items";

    int itemIndex = -1;
    for(int i=0 ; i!= items.count() ; i++)
    {
        qDebug() << " checking for item " << i;
        QDomNodeList titles = items.item(i).toElement().elementsByTagName("title");
        qDebug() << " for this item found titles.count()=" << titles.count();
        if(titles.count()==0) continue;
        if(titles.item(0).toElement().text()==title)
        {
            qDebug() << " title match!";
            itemIndex=i;
            break;
        }
    }
    if(itemIndex==-1)
    {
        QMessageBox::critical(this, tr("ГРЕШКА"), tr("артикула не е открит по title!"));
        return;
    }

    //в items.item(itemIndex) намирам променливата с име varName
    QDomNodeList vars = items.item(itemIndex).toElement().elementsByTagName(varName);
    if(vars.count()==0)
    {
        QMessageBox::critical(this, tr("ГРЕШКА"), tr("променливата не е открита по varName!"));
        return;
    }

    QDomNode var = vars.item(0);
    qDebug() << " var: tagName()=" << var.toElement().tagName() << "  text()=" << var.toElement().text() << " free=" << var.toElement().attribute("free","?");

    QDomElement newVar = xmlDoc.createElement(varName);
    QDomText txtVal = xmlDoc.createTextNode(varNewValue);
    newVar.appendChild(txtVal);
    if(ui->tableWidget->item(row,2)->checkState()==Qt::Checked) free="1";
    else free="0";
    newVar.setAttribute("free",free);
    qDebug() << " newVar: tagName()=" << newVar.toElement().tagName() << "  text()=" << newVar.toElement().text() << " free=" << newVar.toElement().attribute("free","?");

    QDomNode item = items.item(itemIndex);
    item.replaceChild(newVar,var);
    changes=true;
    qDebug() << "changes=true : 4";
    QString winti = this->windowTitle();
    if(!winti.endsWith("*")) this->setWindowTitle(winti + "*");
    refreshItemsTable(ui->listWidget->currentRow());
}

void MainWindow::on_actionSave_triggered()
{
    if(fileName=="") return;
    QFile xmlFile(fileName);
    if(! xmlFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "ERROR: Unable to open xml file for writing";
        QMessageBox::critical(this, tr("ГРЕШКА"), tr("Не же да се отвори xml файл за запис"));
    }
    else
    {
        qDebug() << "Opened xml file. Saving ...";
        QTextStream strm(&xmlFile);
        xmlDoc.save(strm, 0);
        xmlFile.close();
        changes=false;
        qDebug() << "changes=false : 3";
        QString winti = this->windowTitle();
        if(winti.endsWith("*")) this->setWindowTitle(winti.left(winti.length()-1));
    }
}

void MainWindow::on_btnNewVar_clicked()
{
    DlgNewItem dlg;
    QString newItem;
    bool result;

    if(ui->listWidget->currentRow()==-1) return;
    QString title = ui->listWidget->currentItem()->text();

    result = dlg.exec();
    if(result==QDialog::Rejected) return;
    newItem = dlg.getItemName().trimmed();
    if(newItem.length()==0) return;

    QDomElement root = xmlDoc.documentElement();
    QDomNodeList items = root.elementsByTagName("item");

    if(items.count()==0)
    {
        QMessageBox::critical(this, tr("ГРЕШКА"), tr("Няма избран артикул!"));
        return;
    }

    for(int i=0 ; i!= items.count() ; i++)
    {
        QDomNodeList titles = items.item(i).toElement().elementsByTagName("title");
        if( title==titles.item(0).toElement().text() )
        {
            qDebug() << "inserting for title=" << title ;
            QDomElement child = xmlDoc.createElement(newItem);
            QDomText txt = xmlDoc.createTextNode("");
            child.appendChild(txt);
            items.item(i).appendChild(child);
            refreshItemsTable(ui->listWidget->currentRow());
            changes=true;
            qDebug() << "changes=true : 2";
            QString winti = this->windowTitle();
            if(!winti.endsWith("*")) this->setWindowTitle(winti + "*");
            return;
        }
    }
}

void MainWindow::on_btnDeleteVar_clicked()
{
    if(ui->listWidget->currentRow()==-1) return;
    QString title = ui->listWidget->currentItem()->text();
    if(ui->tableWidget->currentRow()==-1) return;
    QString var = ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text();

    QDomElement root = xmlDoc.documentElement();
    QDomNodeList items = root.elementsByTagName("item");

    if(items.count()==0)
    {
        qDebug() << " Nyama izbran element za iztrivane.";
        QMessageBox::critical(this, tr("ГРЕШКА"), tr("Няма избран артикул!"));
        return;
    }

    for(int i=0 ; i!= items.count() ; i++)
    {
        QDomNodeList titles = items.item(i).toElement().elementsByTagName("title");
        if( title==titles.item(0).toElement().text() )
        {
            QDomNodeList vars = items.item(i).toElement().elementsByTagName(var);
            QDomElement child = vars.item(0).toElement();
            qDebug() << "title=" << title << " var=" << var << " : nodeName()=" << child.nodeName()  << " text()=" << child.text();

            vars.item(0).parentNode().removeChild(vars.item(0));
            refreshItemsTable(ui->listWidget->currentRow());
            changes = true;
            qDebug() << "changes=true : 1";
            QString winti = this->windowTitle();
            if(!winti.endsWith("*")) this->setWindowTitle(winti + "*");
            return;
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    QMessageBox msgBox;
    int ret;

    if(changes)
    {
        msgBox.setText(tr("ПРЕДУПЕЖДЕНИЕ"));
        msgBox.setInformativeText(tr("Промените не са записани!"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        ret = msgBox.exec();
    }
    else ret = QMessageBox::Discard;

    switch (ret)
    {
    case QMessageBox::Save :
        on_actionSave_triggered();
        this->close();
        break;
    case QMessageBox::Discard :
        this->close();
        break;
    case QMessageBox::Cancel :
        return;
        break;
    }
}

void MainWindow::on_actionNew_triggered()
{
    if(changes)
    {
        int res = QMessageBox::warning(this, tr("ПРЕДУПРЕЖДЕНИЕ"), tr("Промените не са записани и ще бъдата загубени!\nСигурни ли сте?"), (QMessageBox::Yes | QMessageBox::Cancel));
        if(res==QMessageBox::Cancel) return;
    }
    QFileDialog dlg;
    QString _fileName = dlg.getSaveFileName(this, tr("Нов файл"), "C:\\TFTP-Root", tr("*.xml"));
    int res = dlg.result();
    qDebug() << "result = " << res  << "  filename=" << _fileName;

    if(_fileName=="") return;

    if(!_fileName.endsWith(".xml")) _fileName.append(".xml");

    QFile xmlFile(_fileName);
    if (!xmlFile.open(QIODevice::WriteOnly))//Изтрива всичко от файла при отваряне
    {
        QMessageBox::critical(this, tr("ГРЕШКА"), tr("Не може да се отвори файла за запис!"));
        return;
    }
    else
    {
        xmlDoc.clear();
        QDomElement elm = xmlDoc.createElement("items");
        xmlDoc.appendChild(elm);

        QTextStream strm(&xmlFile);
        xmlDoc.save(strm, 0);
        xmlFile.close();
        fileName = _fileName;
        this->setWindowTitle("PSEditor: " + _fileName);
        ui->btnDeleteItem->setEnabled(true);
        ui->btnDeleteVar->setEnabled(true);
        ui->btnNewItem->setEnabled(true);
        ui->btnNewVar->setEnabled(true);
    }
}

void MainWindow::on_actionSaveAs_triggered()
{
    QFileDialog dlg;
    QString _fileName = dlg.getSaveFileName(this, tr("Запази като нов файл"), "C:\\TFTP-Root", tr("*.xml"));
    int res = dlg.result();
    qDebug() << "result = " << res  << "  filename=" << _fileName;

    if(_fileName=="") return;

    if(!_fileName.endsWith(".xml")) _fileName.append(".xml");

    QFile xmlFile(_fileName);
    if (!xmlFile.open(QIODevice::WriteOnly))//Изтрива всичко от файла при отваряне
    {
        QMessageBox::critical(this, tr("ГРЕШКА"), tr("Не може да се отвори файла за запис!"));
        return;
    }
    else
    {
        fileName = _fileName;
        this->setWindowTitle("PSEditor: " + _fileName);
        qDebug() << "Opened xml file. Saving ...";
        QTextStream strm(&xmlFile);
        xmlDoc.save(strm, 0);
        xmlFile.close();
        changes=false;
    }
}

void MainWindow::on_actionAbout_triggered()
{
    DlgAbout dlg;
    dlg.exec();
}
