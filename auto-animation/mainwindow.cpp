#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QImage>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QProcess>
#include <bits/stdc++.h>
#include <QImageReader>
#include <QMatrix>
#include <QTextStream>

using namespace  std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    ui->setupUi(this);
    QString val;
    QFile file2;
    file2.setFileName("D:\\animation\\script.txt");

    file2.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file2);
    QString words;
    QString text=in.readAll();
    for (int i=0,backgroundid=0,imageid=0,faceid=0;i<text.length()+1;i++) {
        if(text[i]=='<'){imageid++;}
        else if(text[i]=='>')imageid--;
        else if(text[i]=='['){faceid++;}
        else if(text[i]==']')faceid--;
        else if(text[i]=='{'){backgroundid++;}
        else if(text[i]=='}'){backgroundid--; }
        else if(backgroundid!=0);
        else if(imageid!=0);
        else if(faceid!=0);
        else
            words+=text[i];}
    QFile gg( "D:\\animation\\montreal-forced-aligner\\test\\gg.txt" );
    gg.remove();
    if ( gg.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &gg );
        stream << words << endl;
    }

    map<QString,pair<pair <int, pair <int,int > >, pair <QString ,QString > > >mouths;
    map<QString,QString> phones;
    QString cadre="grey";
    QProcess process;
    process.start("D:\\animation\\montreal-forced-aligner\\bin\\mfa_align  \"D:\\animation\\montreal-forced-aligner\\test\" \"D:\\animation\\montreal-forced-aligner\\dict\\librispeech-lexicon.txt\" english  \"D:\\animation\\montreal-forced-aligner\\result\" -q");
    process.waitForFinished(-1);
    process.start("python D:\\animation\\textgrid\\textgrid.py -o D:\\animation\\textgrid\\100.csv \"D:\\animation\\montreal-forced-aligner\\result\\test\\gg.TextGrid\"");
    process.waitForFinished(-1);
    qDebug() << process.readAllStandardError();
    process.start("C:\\Users\\mahdi\\.windows-build-tools\\python27\\python.exe D:\\animation\\textgrid\\parser.py -i D:\\animation\\textgrid\\100.csv -o D:\\animation\\textgrid\\output.json");
    process.waitForFinished(-1);
    qDebug() << process.readAllStandardError();
    process.kill();

    QFile emotions("D:\\animation\\emotion.txt");
    emotions.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in2(&emotions);
    QString carac=in2.readAll();
    QFile filephone("D:\\animation\\phones.txt");
    filephone.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in3(&filephone);
    QString phonetics=in3.readAll(),number,pp;
    for(int i=0,k=0;i<phonetics.length();i++)
    {
        if(phonetics[i]==' '){

            k++;
            continue;
        }
        if(phonetics[i]=='\n'){
            k=0;
            phones[pp]=number;
            pp="",number="";
            continue;
        }
        if(k==0)
            pp+=phonetics[i];
        if(k==1)
            number+=phonetics[i];


    }
    QString name,folder,x,y,pos,rot;
    for(int i=0,k=0;i<carac.length();i++)
    {
        if(carac[i]==' '){

            k++;
            continue;
        }
        if(carac[i]=='\n'){
            k=0;
            mouths[name].first.first=x.toInt(),mouths[name].first.second.first=y.toInt(),mouths[name].first.second.second=rot.toInt(),mouths[name].second.first=folder,mouths[name].second.second=pos;
            name="";
            x="";
            y="";
            folder="";
            pos="";
            rot="";
            continue;
        }
        if(k==0)
            name+=carac[i];
        if(k==1)
            x+=carac[i];
        if(k==2)
            y+=carac[i];
        if(k==3)
            folder+=carac[i];
        if(k==4)
            pos+=carac[i];
        if(k==5)
            rot+=carac[i];
    }
    QFile file;
    file.setFileName("D:\\animation\\textgrid\\output.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonParseError jsonError;
    QJsonDocument flowerJson = QJsonDocument::fromJson(file.readAll(),&jsonError);
    if (jsonError.error != QJsonParseError::NoError){
        qDebug() << jsonError.errorString();
    }
    QList<QVariant> list = flowerJson.toVariant().toList();
    double lastseen=0;
    int lastindex=-1,lastphone=0,scene=1;
    QString backgroundstr,imagestr,facestr,word;
    QString obackgroundstr,oimagestr,ofacestr,oword;
    QPair<double, QPair < int, QString > > blinkeye;
    blinkeye.first=0;
    blinkeye.second.first=0;
    int mirror=-1,omirror=0;
    for (int i=0,backgroundid=0,imageid=0,faceid=0;i<text.length()+1;i++) {
        if(i==text.length())
            goto doit;
        if(text[i]=='<'){oimagestr=imagestr;imagestr="";imageid++;}
        else if(text[i]=='>')imageid--;
        else if(text[i]=='['){ofacestr=facestr;facestr="";faceid++;}
        else if(text[i]==']')faceid--;
        else if(text[i]=='{'){obackgroundstr=backgroundstr;backgroundstr="";backgroundid++;}
        else if(text[i]=='}'){
            backgroundid--; mirror++;}
        else if(backgroundid!=0)
            backgroundstr+=text[i];
        else if(imageid!=0)
            imagestr+=text[i];
        else if(faceid!=0)
            facestr+=text[i];
        else if (text[i]!=' '&&text[i]!='\n')
            word+=text[i];
        else if(word!=""){
            doit:
            word.replace(" ","");
            for(int j=lastindex+1;j<list.size();j++){
                qDebug()<<word.compare(list[j].toMap()["name"].toString(),Qt::CaseInsensitive)<<word <<list[j].toMap()["name"].toString();
                if(word.compare(list[j].toMap()["name"].toString(),Qt::CaseInsensitive)==0)
                {
                    lastindex=j;break;
                }
                else if("<unk>"==list[j].toMap()["name"].toString()){
                    lastindex=j;break;
                }

            }
            //qDebug()<<list[lastindex].toMap()["name"].toString()<<endl;
            QMap<QString, QVariant> map = list[lastindex].toMap();
            double wordbegin = map["start"].toDouble();
            if(wordbegin-0.02>=lastseen)
                wordbegin-=0.08;
            double wordend = map["stop"].toDouble();
            if(lastseen==0.00){
                obackgroundstr="beach";
                ofacestr="demonstrathappy";
                blinkeye.second.second=ofacestr;
           }
            while (lastseen<wordbegin) {
                for(int f=lastphone;f<list.size();f++)
                    if("phones"==list[f].toMap()["tier"].toString()&& lastseen+0.04>list[f].toMap()["start"].toDouble()&& lastseen < list[f].toMap()["stop"].toDouble())
                    {lastphone=f;break;}
                QString phone=list[lastphone].toMap()["name"].toString();
                if(phone[phone.length()-1].isDigit())
                    phone.remove(phone[phone.length()-1]);
                if(!phones[phone][0].isDigit())phones[phone]="6";
                lastseen+=0.04;
                QString oldfacestr;
                oldfacestr=ofacestr;
                //draw scene without image and default face if it s start
                QString txt2="D:\\animation\\background\\"+obackgroundstr+".png";
                QImage background(txt2);
                if(background.load(txt2,"png"));
                else if (background.load(txt2,"jpg"));
                else if (background.load(txt2,"jpeg"));
                else if(background.load(txt2,"bmp"));
                else if (background.load(txt2,"xpm"));
                else if(background.load(txt2,"xbm"));
                else if (background.load(txt2,"gif"));
                QImage cadreimg("D:\\animation\\cadre\\"+cadre+".png");
                if(blinkeye.first>=2.0&&QFile("D:\\animation\\assets\\"+ofacestr+"1.png").exists())
                {
                    if(blinkeye.second.first==0)
                        ofacestr+="1";
                    if(blinkeye.second.first==1)
                        ofacestr+="2";
                    if(blinkeye.second.first==2)
                        ofacestr+="2";
                    blinkeye.second.first++;
                    if(blinkeye.second.first==4){
                        ofacestr+="1";
                        blinkeye.second.first=0;
                        blinkeye.first=0;
                    }
                }
                QImage stick("D:\\animation\\assets\\"+ofacestr+".png");
                ofacestr=oldfacestr;
                if(oimagestr!=""&&!QFile("D:\\animation\\image\\"+oimagestr+".png").exists()){
                    process.start("python D:\\animation\\image\\api.py "+imagestr);
                    process.waitForFinished(-1);
                    QString stdout2 = process.readAllStandardOutput();
                   // qDebug()<<stdout2;
                    download(stdout2,oimagestr);
                }
                QString txt="D:\\animation\\image\\"+oimagestr+".png";
                QImage image;
                if(image.load(txt,"png"));
                else if (image.load(txt,"jpg"));
                else if (image.load(txt,"jpeg"));
                else if(image.load(txt,"bmp"));
                else if (image.load(txt,"xpm"));
                else if(image.load(txt,"xbm"));
                else if (image.load(txt,"gif"));
                if(omirror%2==1)
                    image=image.mirrored(1,0);
                QImage mouth("D:\\animation\\mouth\\"+mouths[ofacestr].second.first+"\\"+phones[phone]+".png");
                int distance=0;
                int ymouth;
                if(mouths[ofacestr].second.second=="reverse"){
                    mouth=mouth.mirrored(1,0);
                    ymouth=mouths[ofacestr].first.first-mouth.width()+1200;
                }
                else{
                    ymouth=1200+mouths[ofacestr].first.first;
                }
                QTransform rotating;
                rotating.rotate(mouths[ofacestr].first.second.second);
                mouth = mouth.transformed(rotating);
               // qDebug() <<("D:\\animation\\mouth\\"+mouths[ofacestr].second+"\\"+phones+".png");
                QImage result(background.width(), 1080 ,QImage::Format_ARGB32_Premultiplied);
                QPainter painter;
                painter.begin(&result);
                painter.drawImage(0, 0, background);
                if(omirror%2==1){
                QMatrix m;
                m.translate( 1920,0);
                 m.scale( -1, 1 );
                 painter.setMatrix( m );}
                if((image.height()>1003)||(image.height()>image.width()&&image.height()<1000)){
                    double scaling= 1003.0/image.height();
                    double width = scaling * image.width();
                    image = image.scaled(width,1003);
                }
                if((image.width()>882)||(image.width()>image.height()&&image.width()<880)){
                    double scaling= 882.0/image.height();
                    double height = scaling * image.height();

                    image = image.scaled(882,height);
                }
                painter.drawImage(0, 0, cadreimg);
                painter.drawImage(479-(image.width()/2),539-(image.height()/2),image);
                painter.drawImage(1200, 1080-stick.height(), stick);
                painter.drawImage(ymouth, 1080-stick.height()+mouths[ofacestr].first.second.first, mouth);

                painter.end();

                result.save("D:\\animation\\scene\\"+QString::number(scene)+".png",0,100);
                scene++;
                blinkeye.first+=0.04;

            }
            omirror=mirror;
            obackgroundstr=backgroundstr;
            oimagestr=imagestr;
            ofacestr=facestr;
            if(blinkeye.second.second!=facestr){
                blinkeye.second.first=0;
                blinkeye.first=0;
                blinkeye.second.second=facestr;
            }
            int step=0,previ=-1;
            while (lastseen<wordend) {
                for(int f=lastphone;f<list.size();f++)
                    if("phones"==list[f].toMap()["tier"].toString()&& lastseen+0.04>list[f].toMap()["start"].toDouble()&& lastseen < list[f].toMap()["stop"].toDouble())
                    {lastphone=f;break;}
                if(previ!=lastphone-1)
                {
                    previ=lastphone-1;
                    step=0;
                }
                QString phone=list[lastphone].toMap()["name"].toString();
                if(phone[phone.length()-1].isDigit())
                    phone.remove(phone[phone.length()-1]);
                if(!phones[phone][0].isDigit())
                    phones[phone]="6";
                QString prevphone;
                if(lastphone!=0)
                    prevphone=list[lastphone-1].toMap()["name"].toString();
                else
                    prevphone="M";
                if(prevphone[prevphone.length()-1].isDigit())
                    prevphone.remove(prevphone[prevphone.length()-1]);
                if(!phones[prevphone][0].isDigit())
                    phones[prevphone]="6";
                QString mouthpos;
                if(phones[prevphone]=="1"){
                    if(phones[phone]=="1")
                        mouthpos="1";
                    else if(phones[phone]=="2")
                        mouthpos="2";
                    else if(phones[phone]=="3"){
                        if(step==0)
                            mouthpos="8";
                        else if(step==1)
                            mouthpos="10";
                        else if(step>=2)
                            mouthpos="3";
                    }
                    else if(phones[phone]=="4"){
                        if(step==0)
                            mouthpos="9";
                        else if(step==1)
                            mouthpos="2";
                        else if(step>=2)
                            mouthpos="4";
                    }
                    else if(phones[phone]=="5"){
                        if(step==0)
                            mouthpos="9";
                        else if(step==1)
                            mouthpos="5";
                        else if(step>=2)
                            mouthpos="5";
                    }
                    else if(phones[phone]=="6"){
                        if(step==0)
                        mouthpos="9";
                    else if(step==1)
                        mouthpos="6";
                    else if(step>=2)
                        mouthpos="6";
                    }
                }
            if(phones[prevphone]=="2"){
                if(phones[phone]=="1")
                    mouthpos="1";
                else if(phones[phone]=="2")
                    mouthpos="2";
                else if(phones[phone]=="3"){
                    if(step==0)
                        mouthpos="11";
                    else if(step==1)
                        mouthpos="10";
                    else if(step>=2)
                        mouthpos="3";
                }
                else if(phones[phone]=="4"){
                    if(step==0)
                        mouthpos="4";
                    else if(step==1)
                        mouthpos="4";
                    else if(step>=2)
                        mouthpos="4";
                }
                else if(phones[phone]=="5"){
                    if(step==0)
                        mouthpos="5";
                    else if(step==1)
                        mouthpos="5";
                    else if(step>=2)
                        mouthpos="5";
                }
                else if(phones[phone]=="6"){
                    if(step==0)
                    mouthpos="6";
                else if(step==1)
                    mouthpos="6";
                else if(step>=2)
                    mouthpos="6";
                }
            }
            if(phones[prevphone]=="3"){
                if(phones[phone]=="1")
                {
                                    if(step==0)
                                        mouthpos="10";
                                    else if(step==1)
                                        mouthpos="9";
                                    else if(step>=2)
                                        mouthpos="1";
                                }
                else if(phones[phone]=="2")
                {
                                    if(step==0)
                                        mouthpos="10";
                                    else if(step==1)
                                        mouthpos="2";
                                    else if(step>=2)
                                        mouthpos="2";
                                }
                else if(phones[phone]=="3"){
                    if(step==0)
                        mouthpos="3";
                    else if(step==1)
                        mouthpos="3";
                    else if(step>=2)
                        mouthpos="3";
                }
                else if(phones[phone]=="4"){
                    if(step==0)
                        mouthpos="10";
                    else if(step==1)
                        mouthpos="4";
                    else if(step>=2)
                        mouthpos="4";
                }
                else if(phones[phone]=="5"){
                    if(step==0)
                        mouthpos="10";
                    else if(step==1)
                        mouthpos="5";
                    else if(step>=2)
                        mouthpos="5";
                }
                else if(phones[phone]=="6"){
                    if(step==0)
                    mouthpos="10";
                else if(step==1)
                    mouthpos="6";
                else if(step>=2)
                    mouthpos="6";
                }
            }
            if(phones[prevphone]=="4"){
                if(phones[phone]=="1")
                {
                                    if(step==0)
                                        mouthpos="2";
                                    else if(step==1)
                                        mouthpos="9";
                                    else if(step>=2)
                                        mouthpos="1";
                                }
                else if(phones[phone]=="2")
                    mouthpos="2";
                else if(phones[phone]=="3"){
                    if(step==0)
                        mouthpos="10";
                    else if(step==1)
                        mouthpos="3";
                    else if(step>=2)
                        mouthpos="3";
                }
                else if(phones[phone]=="4"){
                    if(step==0)
                        mouthpos="4";
                    else if(step==1)
                        mouthpos="4";
                    else if(step>=2)
                        mouthpos="4";
                }
                else if(phones[phone]=="5"){
                    if(step==0)
                        mouthpos="5";
                    else if(step==1)
                        mouthpos="5";
                    else if(step>=2)
                        mouthpos="5";
                }
                else if(phones[phone]=="6"){
                    if(step==0)
                    mouthpos="6";
                else if(step==1)
                    mouthpos="6";
                else if(step>=2)
                    mouthpos="6";
                }
            }
            if(phones[prevphone]=="5"){
                if(phones[phone]=="1")
                {
                                    if(step==0)
                                        mouthpos="2";
                                    else if(step==1)
                                        mouthpos="9";
                                    else if(step>=2)
                                        mouthpos="1";
                                }
                else if(phones[phone]=="2")
                    mouthpos="2";
                else if(phones[phone]=="3"){
                    if(step==0)
                        mouthpos="10";
                    else if(step==1)
                        mouthpos="3";
                    else if(step>=2)
                        mouthpos="3";
                }
                else if(phones[phone]=="4"){
                    if(step==0)
                        mouthpos="4";
                    else if(step==1)
                        mouthpos="4";
                    else if(step>=2)
                        mouthpos="4";
                }
                else if(phones[phone]=="5"){
                    if(step==0)
                        mouthpos="5";
                    else if(step==1)
                        mouthpos="5";
                    else if(step>=2)
                        mouthpos="5";
                }
                else if(phones[phone]=="6"){
                    if(step==0)
                    mouthpos="6";
                else if(step==1)
                    mouthpos="6";
                else if(step>=2)
                    mouthpos="6";
                }
            }
            if(phones[prevphone]=="6"){
                if(phones[phone]=="1")
                {
                                    if(step==0)
                                        mouthpos="2";
                                    else if(step==1)
                                        mouthpos="9";
                                    else if(step>=2)
                                        mouthpos="1";
                                }
                else if(phones[phone]=="2")
                    mouthpos="2";
                else if(phones[phone]=="3"){
                    if(step==0)
                        mouthpos="10";
                    else if(step==1)
                        mouthpos="3";
                    else if(step>=2)
                        mouthpos="3";
                }
                else if(phones[phone]=="4"){
                    if(step==0)
                        mouthpos="4";
                    else if(step==1)
                        mouthpos="4";
                    else if(step>=2)
                        mouthpos="4";
                }
                else if(phones[phone]=="5"){
                    if(step==0)
                        mouthpos="5";
                    else if(step==1)
                        mouthpos="5";
                    else if(step>=2)
                        mouthpos="5";
                }
                else if(phones[phone]=="6"){
                    if(step==0)
                    mouthpos="6";
                else if(step==1)
                    mouthpos="6";
                else if(step>=2)
                    mouthpos="6";
                }

            }
                step++;
                QString oldfacestr;
                 oldfacestr=facestr;
                lastseen+=0.04;
                //draw scene without image and default face if it s start
                QString txt2="D:\\animation\\background\\"+backgroundstr+".png";
                QImage background(txt2);
                if(background.load(txt2,"png"));
                else if (background.load(txt2,"jpg"));
                else if (background.load(txt2,"jpeg"));
                else if(background.load(txt2,"bmp"));
                else if (background.load(txt2,"xpm"));
                else if(background.load(txt2,"xbm"));
                else if (background.load(txt2,"gif"));
                if(blinkeye.first>=2.0&&QFile("D:\\animation\\assets\\"+facestr+"1.png").exists())
                {

                    if(blinkeye.second.first==0)
                        facestr+="1";
                    if(blinkeye.second.first==1)
                        facestr+="2";
                    if(blinkeye.second.first==2)
                        facestr+="2";
                    blinkeye.second.first++;
                    if(blinkeye.second.first==4){
                        facestr+="1";
                        blinkeye.second.first=0;
                        blinkeye.first=0;
                    }
                }
                QImage stick("D:\\animation\\assets\\"+facestr+".png");
                facestr=oldfacestr;
                QImage cadreimg("D:\\animation\\cadre\\"+cadre+".png");
                if(imagestr!=""&&!QFile("D:\\animation\\image\\"+imagestr+".png").exists()){
                    process.start("python D:\\animation\\image\\api.py \""+imagestr+"\" ");
                    process.waitForFinished(-1);
                    QString stdout2 = process.readAllStandardOutput();
                    download(stdout2,imagestr);
                }
                QString txt="D:\\animation\\image\\"+imagestr+".png";
                QImage image;
                if(image.load(txt,"png"));
                else if (image.load(txt,"jpg"));
                else if (image.load(txt,"jpeg"));
                else if(image.load(txt,"bmp"));
                else if (image.load(txt,"xpm"));
                else if(image.load(txt,"xbm"));
                else if (image.load(txt,"gif"));
                if(mirror%2==1)
                    image=image.mirrored(1,0);
                 //("D:\\animation\\image\\"+imagestr+".png");

                QImage mouth("D:\\animation\\mouth\\"+mouths[facestr].second.first+"\\"+mouthpos+".png");
                int ymouth;
                if(mouths[facestr].second.second=="reverse"){
                    mouth=mouth.mirrored(1,0);
                    ymouth=mouths[facestr].first.first-mouth.width()+1200;
                }
                else{
                    ymouth=1200+mouths[facestr].first.first;
                }

                QTransform rotating;
                rotating.rotate(mouths[facestr].first.second.second);
                mouth = mouth.transformed(rotating);
                QImage result(background.width(), 1080 ,QImage::Format_ARGB32_Premultiplied);
                QPainter painter;
                painter.begin(&result);
                painter.drawImage(0, 0, background);
                if(mirror%2==1){
                QMatrix m;
                m.translate( 1920,0);
                 m.scale( -1, 1 );
                 painter.setMatrix( m );}
                if((image.height()>1003)||(image.height()>image.width()&&image.height()<1000)){
                    double scaling= 1003.0/image.height();
                    double width = scaling * image.width();
                    image = image.scaled(width,1003);
                }
                if((image.width()>882)||(image.width()>image.height()&&image.width()<880)){
                    double scaling= 882.0/image.height();
                    double height = scaling * image.height();

                    image = image.scaled(882,height);
                }
                painter.drawImage(0, 0, cadreimg);
                painter.drawImage(479-(image.width()/2),539-(image.height()/2),image);

                painter.drawImage(1200, 1080-stick.height(), stick);
                 painter.drawImage(ymouth, 1080-stick.height()+mouths[facestr].first.second.first, mouth);
                painter.end();
                result.save("D:\\animation\\scene\\"+QString::number(scene)+".png",0,100);
                scene++;
                blinkeye.first+=0.04;
            }
           // qDebug()<<1;
            word="";
        }

    }
    qDebug()<<"hello";
    process.start("D:\\animation\\ffmpeg\\bin\\ffmpeg.exe -y -r 25 -f image2 -s 1920x1080 -i D:\\animation\\scene\\%d.png -i \"D:\\animation\\montreal-forced-aligner\\test\\gg.mp3\" -vcodec libx264 -b 4M  -acodec copy D:\\animation\\OUTPUT.mp4");
    process.waitForFinished(-1);
    qDebug() << process.readAllStandardError();
    QImage image1("D:\\animation\\assets\\annoncing happy.png");
    QImage image2("D:\\animation\\assets\\angry.png");
    QImage result(image1.width() + image2.width(), image1.height() ,QImage::Format_ARGB32_Premultiplied);
    QPainter painter;
    painter.begin(&result);
    painter.drawImage(0, 0, image1);
    painter.drawImage(50, 0, image2);
    painter.end();
    result.save("D:\\animation\\test.png",0,100);
}
void MainWindow::download(QString url,QString name)
{
    QFile output("D:\\animation\\image\\out.txt");
    output.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in2(&output);
    QString strr=in2.readAll();
    url=strr;
    //std::string t=url.toUtf8().constData();
    QProcess process;
    process.start("D:\\animation\\image\\wget.exe -O \"D:\\animation\\image\\"+name+".png\"  -q "+url+"");
    process.waitForFinished(-1);
}
MainWindow::~MainWindow()
{
    delete ui;
}

