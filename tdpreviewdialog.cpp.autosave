/*
 * Table View Print & Preview dialog
 * Copyright (C) 2004-2008 by Gordos Kund / QnD Co Bt.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact gordos.kund@gmail.com with any questions on this license.
 */

#include "tdpreviewdialog.h"
#include <QDebug>
#include <QTextDocument>
#include <QPageSetupDialog>
#include <QGraphicsTextItem>
#include <QHeaderView>
#include <QPainter>
#include <QTime>
#include <QPrintDialog>
#include <QRectF>
#include <QTextCursor>

#pragma execution_character_set("utf-8")

TDPreviewDialog::TDPreviewDialog(BillsView *p_tableView, QPrinter * p_printer, QString customer, QWidget *parent) : QDialog(parent),
    ui(new Ui::TDPreviewDialog)
{
        //variables
        tableView=p_tableView;
        leftMargin=10;
        rightMargin=10;
        topMargin=40;
        bottomMargin=40;
        spacing=20;
        headerSize=60;
        footerSize=60;
        sceneZoomFactor=100;
        columnZoomFactor=1.0;
        gridMode=NoGrid;

        model= (BillModel *)tableView->model();
        m_header_model = (HHeaderModel *)tableView->horizontalHeader()->model();
        //setup printer
        printer=p_printer;
        printer->setFullPage(true);
        switch (QLocale::system().country()) {
        case QLocale::AnyCountry:
        case QLocale::Canada:
        case QLocale::UnitedStates:
        case QLocale::UnitedStatesMinorOutlyingIslands:
                printer->setPageSize(QPrinter::Letter);
                break;
        default:
                printer->setPageSize(QPrinter::A4);
                break;
        }


        setHeaderText("金花布业销售单");
        footerText=trUtf8("制单人: 金花                           司机: 老王                           银行卡号: 3999444444444444444                           ");
        footerText+= "\n页数: ";



        //get column widths
        for (int i=0; i<model->columnCount(); i++) {
                if (tableView->isColumnHidden(i)) continue;
                int colWidth=tableView->columnWidth(i);
                colSizes.append(QTextLength(QTextLength::FixedLength,colWidth));
        }
        
        //title font
        titleFont=tableView->font();
        titleFont.setPointSize(14);
        titleFont.setBold(false);
        titleFont.setUnderline(true);
        titleFmt =new QFontMetrics(titleFont);


        //header font
        headerFont=tableView->font();
        headerFont.setBold(true);
//        headerFont.setUnderline(true);
        headerFont.setPointSize(11);
        headerFmt =new QFontMetrics(headerFont);

        //normal font
        font=tableView->font();
        headerFont.setPointSize(10);
        fmt =new  QFontMetrics(font);


        QString date=QDate::currentDate().toString(QLocale().dateFormat());
        QString time=QTime::currentTime().toString(QLocale().timeFormat(QLocale::ShortFormat));
        headerStdText = "客户名: " + customer +"                        ";
        headerStdText.append("地址: 商丘市梁园区海淀路9号金花布业                        ");
        headerStdText.append("电话: 13939605733                        ");
        headerStdText.append(date/*+"  "+time*/);

        setupPage();
//        tableView->setGeometry(0, 0, pageScene.width(), tableView->height());

        columnMultiplier=pageScene.width()/tableView->width()*columnZoomFactor;
}

//=================================================================

int TDPreviewDialog::exec()
{
        //GUI
        ui->setupUi(this);
        connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), ui->pageSpinBox, SLOT(setValue(int)));
        connect(ui->prevToolButton, SIGNAL(clicked()), ui->pageSpinBox, SLOT(stepDown()));
        connect(ui->nextToolButton, SIGNAL(clicked()), ui->pageSpinBox, SLOT(stepUp()));

        ui->graphicsView->setScene(&pageScene);
        ui->graphicsView->ensureVisible(0,0,10,10);
        setupSpinBox();
        on_pageSpinBox_valueChanged(1);
        return QDialog::exec();
}

//=================================================================

TDPreviewDialog::~TDPreviewDialog()
{}

//=================================================================

void TDPreviewDialog::on_setupToolButton_clicked()
{
        QPageSetupDialog *dialog;
        dialog = new QPageSetupDialog(printer, this);
        if (dialog->exec() == QDialog::Rejected) {
                return;
        }
        setupPage();
        on_pageSpinBox_valueChanged(1);
        setupSpinBox();
}

//=================================================================

void TDPreviewDialog::on_zoomInToolButton_clicked()
{
        ui->graphicsView->scale(1.5,1.5);
}

//=================================================================

void TDPreviewDialog::on_zoomOutToolButton_clicked()
{
        ui->graphicsView->scale(1 / 1.5, 1 / 1.5);
}

//=================================================================

void TDPreviewDialog::on_pageSpinBox_valueChanged(int value)
{
        paintPage(value);
        pageScene.addRect(0,0,pageScene.width(),pageScene.height(),QPen(Qt::black, 2.0));
}

//=================================================================

void TDPreviewDialog::setupSpinBox()
{
        ui->pageSpinBox->setPrefix(QString::number(pages)+" / ");
        ui->pageSpinBox->setMaximum(pages);
        ui->horizontalSlider->setMaximum(pages);
}

//=================================================================

void TDPreviewDialog::setupPage()
{
        rowHeight=font.pointSize()+spacing;
        QRectF rect=printer->paperRect();
        QRectF rectNew = QRectF(0,0,rect.width() / printer->logicalDpiX() * sceneZoomFactor, rect.height() / printer->logicalDpiY() * sceneZoomFactor);
        pageScene.setSceneRect(rectNew);

        //count lines
        lines = ((pageScene.height()-topMargin-bottomMargin-/*headerSize*/-footerSize)/rowHeight)/*-3*/;

        //count pages
        QAbstractItemModel *model= tableView->model();
        int rowCount=model->rowCount();
        double div = rowCount / lines;
        int modulo = rowCount % lines;
        if (modulo == 0 ) {
                pages = QVariant(div).toInt();
        } else {
                div = div+1.0;
                pages = QVariant(div).toInt();
        }
}

//=================================================================

void TDPreviewDialog::setHeaderText(const QString &text)
{
        headerText=text;
}

//=================================================================

void TDPreviewDialog::print()
{
        //printDialog
        printer->setFromTo(1,pages);
        printer->setOutputFormat(QPrinter::NativeFormat);
        printer->setOutputFileName("");
        QPrintDialog dialog(printer, this);
        dialog.setWindowTitle("打印文档");
        if (dialog.exec() == QDialog::Rejected) {
                return;
        }
        setupPage();


        // get num copies
        int doccopies;
        int pagecopies;
        if (printer->collateCopies()) {
                doccopies = 1;
                pagecopies = printer->numCopies();
        } else {
                doccopies = printer->numCopies();
                pagecopies = 1;
        }

        // get page range
        int firstpage = printer->fromPage();
        int lastpage = printer->toPage();
        if (firstpage == 0 && lastpage == 0) { // all pages
                firstpage = 1;
                lastpage =pages;
        }

        // print order
        bool ascending = true;
        if (printer->pageOrder() == QPrinter::LastPageFirst) {
                int tmp = firstpage;
                firstpage = lastpage;
                lastpage = tmp;
                ascending = false;
        }

        // loop through and print pages
        QPainter painter(printer);
        for (int dc=0; dc<doccopies; dc++) {
            int pagenum = firstpage;
            while (true) {
                for (int pc=0; pc<pagecopies; pc++) {
                    if (printer->printerState() == QPrinter::Aborted ||
                                    printer->printerState() == QPrinter::Error) {
                            break;
                    }
                    // print page
                    paintPage(pagenum);
                    pageScene.render(&painter);
                    if (pc < pagecopies-1) printer->newPage();
                }
                if (pagenum == lastpage) break;
                if (ascending) {
                        pagenum++;
                } else {
                        pagenum--;
                }
                printer->newPage();
            }

            if (dc < doccopies-1) printer->newPage();
        }

}

//=================================================================

void TDPreviewDialog::exportPdf(const QString &filename)
{
    // file save dialog
    QString dialogcaption = "Export PDF";
    QString exportname = QFileDialog::getSaveFileName(this, dialogcaption, filename, "*.pdf");
    if (exportname.isEmpty()) return;
    if (QFileInfo(exportname).suffix().isEmpty())
            exportname.append(".pdf");

    // setup printer
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(exportname);

    // print pdf
    QPainter painter(printer);
    for (int pagenum=1; pagenum<=pages; pagenum++) {
            paintPage(pagenum);
            pageScene.render(&painter);
            if (pagenum < pages) {
                    printer->newPage();
            }
    }
}

//=================================================================

void TDPreviewDialog::paintPage(int pagenum)
{
    //Delete all
    QList<QGraphicsItem*> L = pageScene.items();
    while ( ! L.empty() ) {
            pageScene.removeItem( L.first() );
            delete L.first();
            L.removeFirst();
    }

    //Table header
    int csize=0;
    pageScene.addLine(leftMargin,topMargin+spacing+headerSize,pageScene.width()-rightMargin, topMargin+spacing+headerSize,QPen(Qt::black, 1.0));

    for (int i=0; i<model->columnCount(); i++) {
        if (tableView->isColumnHidden(i)) continue;
        int logicalIndex=tableView->horizontalHeader()->logicalIndex(i);
        QString txt = m_header_model->item(0, i);
        QGraphicsTextItem *item=new QGraphicsTextItem();
        if (i <3 || i > 7)
        {
            int headerLineX = leftMargin + i*(tableView->horizontalHeader()->width()/tableView->horizontalHeader()->count());
            pageScene.addLine(headerLineX,topMargin+spacing+headerSize,headerLineX, headerSize+topMargin+spacing+tableView->horizontalHeader()->height(),QPen(Qt::black, 1.0));
        }
        item->setTextWidth(item->boundingRect().width());
        item->setPlainText(txt);
        QTextBlockFormat format;
        format.setAlignment(Qt::AlignHCenter);
        QTextCursor cursor = item->textCursor();
        cursor.select(QTextCursor::Document);
        cursor.mergeBlockFormat(format);
        cursor.clearSelection();
        item->setTextCursor(cursor);
        item->setFont(headerFont);
        txt=headerFmt->elidedText(txt,Qt::ElideRight,QVariant(colSizes[logicalIndex].rawValue()).toInt()/*-8*/);


        //item->moveBy((csize*columnMultiplier)+leftMargin,topMargin+headerSize+(spacing*2));
        item->moveBy(csize+leftMargin,topMargin+headerSize+(spacing));
        csize=csize+QVariant(colSizes[logicalIndex].rawValue()).toInt();
        pageScene.addItem(item);
    }
    pageScene.addLine(leftMargin+spacing,headerSize+topMargin,pageScene.width()-rightMargin, headerSize+topMargin+spacing,QPen(Qt::black, 1.0));

    //Table rows
    qreal dx,dy;
    QPen pen(Qt::gray, 1);
    QBrush brush(Qt::gray,Qt::SolidPattern);
    int borderAdjust=rowHeight / 5;

    for (int i=0; i<lines; i++) {
        csize=0;
        dy=(i*rowHeight)+topMargin+headerSize+(rowHeight*2);
        int modelIdxY=(pagenum-1)*lines+i;
        if (modelIdxY>=model->rowCount()) {
                break;
        }

        for (int j=0; j<model->columnCount(); j++) {
            if (tableView->isColumnHidden(j)) continue;
            int logicalIndex=tableView->horizontalHeader()->logicalIndex(j);
            int actColSize=QVariant(colSizes[logicalIndex].rawValue()).toInt();
            QString txt = model->data(model->index(modelIdxY,logicalIndex)).toString();
            QGraphicsTextItem *item=new QGraphicsTextItem();
            item->setTextWidth(item->boundingRect().width());
            QTextBlockFormat format;
            format.setAlignment(Qt::AlignCenter);
            QTextCursor cursor = item->textCursor();
            cursor.select(QTextCursor::Document);
            cursor.mergeBlockFormat(format);
            cursor.clearSelection();
            item->setTextCursor(cursor);
            item->setFont(font);
            txt=fmt->elidedText(txt,Qt::ElideRight,actColSize/*-8*/);
            item->setPlainText(txt);
            //qreal dx=(csize*columnMultiplier)+leftMargin;
            dx=csize+leftMargin;
            item->moveBy(dx,dy);

            //rectangle
            if (gridMode==NormalGrid || gridMode==AlternateWithGrid) {
                    pageScene.addRect(dx,dy+borderAdjust,actColSize,rowHeight);
            }
            pageScene.addItem(item);
            csize=csize+actColSize;
        }

        if (gridMode==AlternateColor || gridMode==AlternateWithGrid) {

                int modulo=i % 2;
                if (modulo==0) {
                    //rectangle grey
                    QGraphicsRectItem *rItem=pageScene.addRect(leftMargin,dy+borderAdjust,csize,rowHeight,pen,brush);
                    rItem->setZValue(-1);
                }
        }

    }

    // Page header
    if (headerSize > 0) {
        //line
        pageScene.addLine(leftMargin,headerSize+topMargin,pageScene.width()-rightMargin, headerSize+topMargin,QPen(Qt::black, 1.0));

        //tile
        QGraphicsTextItem *titleItem=new QGraphicsTextItem();
        titleItem->setFont(titleFont);
        titleItem->setPlainText(headerText);
        int titleWidth=titleFmt->width(headerText);
        double xpos=(pageScene.width()/2)-(titleWidth/2);
        double ypos=(topMargin+(titleFmt->height()/2));
        titleItem->moveBy(xpos,ypos);
        pageScene.addItem(titleItem);

        //std text
        QGraphicsTextItem *item=new QGraphicsTextItem();
        item->setFont(headerFont);
        item->setPlainText(headerStdText);
        item->moveBy(leftMargin,headerSize/2 + rowHeight-spacing+topMargin);
        pageScene.addItem(item);
    }

    // footer
    if (footerSize > 0) {
        pageScene.addLine(leftMargin,pageScene.height()-footerSize-bottomMargin,pageScene.width()-rightMargin, pageScene.height()-footerSize-bottomMargin,QPen(Qt::black, 1.0));
        QGraphicsTextItem *item=new QGraphicsTextItem();
        item->setFont(headerFont);
        item->setPlainText(footerText+QString::number(pages)+" / "+QString::number(pagenum));
        item->moveBy(leftMargin,pageScene.height()-footerSize-bottomMargin+spacing);
        pageScene.addItem(item);
    }
    //pageScene.update();
}

//=================================================================

void TDPreviewDialog::setGridMode(Grids _gridMode)
{
        gridMode=_gridMode;
}
