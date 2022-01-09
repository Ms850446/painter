#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QImage>
#include <QMouseEvent>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QDir>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currimage=0;
    mImage =new QImage(QApplication::desktop()->size(),QImage::Format_ARGB32_Premultiplied);
    mImage->fill(Qt::white);
    QImage o=*mImage;
    QImage* a=new QImage();
    *a=o;
    sketch[0]=a;
    mPainter=new QPainter(mImage);
    mEnabled=false;
    mColor=Qt::black;
    mSize=5;
    currentTool= linetool;
    hide_inputDialog();
    rightClick=false;
}

MainWindow::~MainWindow()
{
    delete ui;
     delete mPainter;
delete mImage;
    delete oldImage;
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QRect space=e->rect();
    painter.drawImage(space,*mImage,space);
 update();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
   hide_inputDialog();
    if(e->button()==Qt::LeftButton)
    {
        hide_inputDialog();
    mEnabled=true;
    mBegin=e->pos();
    mEnd = e->pos();

    }
    else
    {
        rightClick=true;
        return;
    }


}

void MainWindow::hide_inputDialog()
{
    ui->length->setVisible(false);
    ui->height->setVisible(false);
    ui->ok->setVisible(false);
//    ui->radiusLabel->setVisible(false);
//    ui->lineLengthLabel->setVisible(false);
    ui->width_length_raduisLabel->setVisible(false);
    ui->heightLabel->setVisible(false);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{

    if(!mEnabled)
    {
        e->accept();
        return ;
    }
    if(rightClick)
        return;
    if(mImage->isNull())
        return;
    mEnd=e->pos();
    QPen pen;
   pen.setWidth(mSize);
    pen.setColor(mColor);
    mPainter->setPen(pen);
    update();

}

QString MainWindow::getShapeName()
{
    bool ok;
            QString text = QInputDialog::getText(this, tr("Get Shape Name"),
                                                 tr("Enter Shape Name:"), QLineEdit::Normal,
                                                 QDir::home().dirName(), &ok);
            while (!ok && text.isEmpty())
            {
                text = QInputDialog::getText(this, tr("Get Shape Name"),
                                                                 tr("Please Enter the Name:"), QLineEdit::Normal,
                                                                 QDir::home().dirName(), &ok);
            }
            while(!isValidName(text))
            {
                text = QInputDialog::getText(this, tr("Get Shape Name"),
                                                                 tr("Please Enter another Name:"), QLineEdit::Normal,
                                                                 QDir::home().dirName(), &ok);
            }
            return text;
}

bool MainWindow::isValidName(QString name)
{
    for(int i=0;i<shapes.size();i++)
    {
        if(name==shapes[i]->getName())
        {
          return false;
        }
    }
    return true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if (mBegin == mEnd)
        return;
    if(rightClick)
    {

        rightClick=false;
        return;
    }
    if(currentTool==linetool)
    {
        undotimes=0;
        QImage m=*mImage;
        QImage* s=new QImage();
        *s=m;
        currimage++;
        sketch[currimage]=s;
        line=QLine(mBegin,mEnd);
        mPainter->drawLine(line);

        Shape *shape=new StraightLine(getShapeName(),line.p1(),line.p2(),mSize,mColor);
        shapes.push_back(shape);
    }
    else if(currentTool==rectangletool)
    {

        undotimes=0;
        QImage m=*mImage;
        QImage* s=new QImage();
        *s=m;
        currimage++;
        sketch[currimage]=s;
  rect=directionOfDraw(mEnd);
  mPainter->drawRect(rect);
  Shape *shape=new Rectangle(getShapeName(),mBegin,mEnd,mSize,mColor);
  shapes.push_back(shape);

    }
    else{

        draw_circle(mBegin,mEnd);
     }


    mEnabled=false;
    e->accept();
    update();
}



void MainWindow::mouseDoubleClickEvent( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton )
    {
        mBegin=e->pos();
       on_actionManual_input_triggered();
    }


}



void MainWindow::on_actionQuit_triggered()
{
    close();
}


QRect MainWindow::directionOfDraw(const QPoint &endPoint)
{
    QRect rect;
    if(endPoint.x() < mBegin.x())
    {
        rect = QRect(endPoint, mBegin);
   QPoint p;
   p=mBegin;
   mBegin=endPoint;
   mEnd=p;
    }
    else
        rect = QRect(mBegin, endPoint);
    return rect;
}

void MainWindow::drawRectangle(QPoint begin, QPoint end)
{
       rect = QRect(begin, end);
    undotimes=0;
    QImage m=*mImage;
    QImage* s=new QImage();
    *s=m;
    currimage++;
    sketch[currimage]=s;
    mPainter->drawRect(rect);
    Shape *shape=new Rectangle(getShapeName(),begin,end,mSize,mColor);
     shapes.push_back(shape);
}




void MainWindow::draw_circle(const QPoint startPoint,const QPoint endPoint){
    int d=(startPoint.x()-endPoint.x())*(startPoint.x()-endPoint.x())+(startPoint.y()-endPoint.y())*(startPoint.y()-endPoint.y());
       d=sqrt(d);
       int x,y;
       x=(startPoint.x()+endPoint.x())/2;
       y=(startPoint.y()+endPoint.y())/2;
       QPoint p;
       p.setX(x);
       p.setY(y);
       undotimes=0;
       QImage m=*mImage;
       QImage* s=new QImage();
       *s=m;
       currimage++;
       sketch[currimage]=s;
    mPainter->drawEllipse(p,d/2,d/2);
    Shape *shape=new Circle(getShapeName(),startPoint,endPoint,mSize,mColor);
       shapes.push_back(shape);
}

QLine MainWindow::directionOfDrawLine(const QPoint &endPoint)
{
    QLine Line_;
    if(endPoint.x() < mBegin.x())
    {
        Line_ = QLine(endPoint, mBegin);
        QPoint p;
        p=mBegin;
        mBegin=endPoint;
        mEnd=p;
    }
    else
        Line_ = QLine(mBegin, endPoint);
    return Line_;
}
void MainWindow::DrawLine(QPoint begin,QPoint end)
{
    QLine line;
        line= QLine(begin, end);
        undotimes=0;
        QImage m=*mImage;
        QImage* s=new QImage();
        *s=m;
        currimage++;
        sketch[currimage]=s;
mPainter->drawLine(line);
Shape *shape=new StraightLine(getShapeName(),line.p1(),line.p2(),mSize,mColor);
       shapes.push_back(shape);
}


/*QRect MainWindow::directionOfDrawCircle(const QPoint &endPoint)
{
        QPoint p;
    if(endPoint.x() < mBegin.x())
    {
        p=mEnd;
        mEnd=mBegin;
        mBegin=p;
        mEnd=QPoint(mEnd.x()-mBegin.x(),mEnd.x()-mBegin.x());
        rect=QRect(mBegin,mEnd);
    }
    else
    {
         mEnd=QPoint(mEnd.x()-mBegin.x(),mEnd.x()-mBegin.x());
        rect = QRect(mBegin, endPoint);
    }
    return rect;
}*/




void MainWindow::on_actionLine_triggered()
{
        currentTool=linetool;
       hide_inputDialog();
}


void MainWindow::on_actionRectangle_triggered()
{
    currentTool=rectangletool;
 hide_inputDialog();
}


void MainWindow::on_actionCircle_triggered()
{
    currentTool=circletool;
  hide_inputDialog();
}

void MainWindow::on_actionColor_triggered()
{
    mColor=QColorDialog::getColor(Qt::black,this,"Color");
}

void MainWindow::on_actionThickness_triggered()
{
    mSize=QInputDialog::getInt(this,"Size","Enter the Size",5,1);
}

void MainWindow::on_actionNew_triggered()
{
    oldImage=mImage;
    mImage =new QImage(QApplication::desktop()->size(),QImage::Format_ARGB32_Premultiplied);
    mImage->fill(Qt::white);
    mPainter=new QPainter(mImage);
}



void MainWindow::on_actionUndo_triggered()
{
    if(currimage>0)
    {
        removedshapes.push(shapes.back());
        shapes.erase(shapes.end()-1);
     delete mPainter;
        redoImgs[redoCounter++]=mImage;
     mImage=sketch[currimage];
     mPainter=new QPainter(mImage);
     mPainter->setPen(Qt::black);
     currimage--;
     undotimes++;

    }
}


void MainWindow::on_actionRedo_triggered()
{
    if(undotimes>0)
    {
        shapes.push_back(removedshapes.top());
        removedshapes.pop();
        delete mPainter;
         currimage+=1;
        mImage=redoImgs[--redoCounter];
        mPainter=new QPainter(mImage);
        mPainter->setPen(Qt::black);
        undotimes--;
    }
}



void MainWindow::on_ok_clicked()
{   bool isNumber;
    QPen pen;
   pen.setWidth(mSize);
    pen.setColor(mColor);
    mPainter->setPen(pen);
    hide_inputDialog();


    if(currentTool==linetool)
    { double angle;
      double length=ui->length->text().toDouble(&isNumber);
      if(isNumber)
        angle=ui->height->text().toDouble(&isNumber)/180.0*3.14*-1;
      if(!isNumber)
        QMessageBox::warning(this,"invalid data","please enter a valid input");
        else
     { QPoint p2;
      p2.setX(mBegin.x()+length*cos(angle));
      p2.setY(mBegin.y()+length*sin(angle));
      DrawLine(mBegin,p2);}
    }
    else if(currentTool==rectangletool)
    {
     double h,w;
     w=ui->length->text().toDouble(&isNumber);
     if(isNumber)
     h=ui->height->text().toDouble(&isNumber);
     if(!isNumber)
      QMessageBox::critical(this,"invalid data","please enter a valid input");
     else{
     QPoint p2;
     p2.setX(mBegin.x()+w);
     p2.setY(mBegin.y()+h);
     drawRectangle(mBegin,p2);
        }
            }
    else{

   double r;
    r=ui->length->text().toDouble(&isNumber);
    if(!isNumber)
    QMessageBox::critical(this,"invalid data","please enter a valid input");
    else{
    QPoint p2;
    p2.setX(mBegin.x()+r);
    p2.setY(mBegin.y());
    mBegin.setX(mBegin.x()-r);
    draw_circle(mBegin,p2);
}
}

}


void MainWindow::on_actionManual_input_triggered()
{
     ui->length->setVisible(true);
     ui->ok->setVisible(true);
     ui->width_length_raduisLabel->setVisible(true);

    if(currentTool==rectangletool)
    {
    ui->width_length_raduisLabel->setText("Enter width");
    ui->heightLabel->setVisible(true);  
    ui->height->setVisible(true);
    ui->heightLabel->setText("Enter the Height");
    }
    if(currentTool==circletool)
    ui->width_length_raduisLabel->setText("Enter Raduis");

    else if(currentTool==linetool)
    {
     ui->width_length_raduisLabel->setText("Enter Length");
     ui->heightLabel->setVisible(true);
     ui->heightLabel->setText("angle in Degree");
     ui->height->setVisible(true);
        }


}


void MainWindow::on_actionSearch_triggered()
{
    bool ok;
            QString text = QInputDialog::getText(this, tr("Search"),
                                                 tr("Enter Shape Name:"), QLineEdit::Normal,
                                                 QDir::home().dirName(), &ok);
            if (ok && !text.isEmpty())
            {
                for(int i=0;i<shapes.size();i++)
                {
                 if(shapes[i]->getName()==text)
                 {
                     if(shapes[i]->getType()==linetool)
                       {
                         QMessageBox::information(this,"Shape data","name is "+shapes[i]->getName()+" perimeter is "+QString::number(shapes[i]->perimeter(),'d',3)+" the type is line ");
                         return;
                       }
                     if(shapes[i]->getType()==circletool)
                       {
                         QMessageBox::information(this,"Shape data","name is "+shapes[i]->getName()+" perimeter is "+QString::number(shapes[i]->perimeter())+" the type is Circle ");
                         return;
                       }
                     if(shapes[i]->getType()==rectangletool)
                       {

                         QMessageBox::information(this,"Shape data","name is "+shapes[i]->getName()+" perimeter is "+QString::number(shapes[i]->perimeter())+" the type is rectangle");
                         return;
                       }
                 }
            }
                QMessageBox::information(this,"Shape data","Shape not found!");

            }
}


void MainWindow::on_actionSort_triggered()
{
    bool isSorted=false;
    for(int i=0;i<shapes.size();i++)
    {

        for(int j=0;j<shapes.size()-1-i;j++)
        {
            Shape*t;
            if(shapes[j]->perimeter()>shapes[j+1]->perimeter())
            {
                t=shapes[j];
                shapes[j]=shapes[j+1];
                shapes[j+1]=t;
                isSorted=true;
            }
        }
        if(!isSorted)
            break;
    }
    QString name;
       for(int i=0;i<shapes.size();i++)
       {
           name+=shapes[i]->getName();
           name+=" ";
       }
       ui->statusbar->showMessage(name);
}

