/*****************************************************************************/
/*  Asciifile.cpp - ascii file importer                                          */
/*                                                                           */
/*  Copyright (C) 2011 Rallaz, rallazz@gmail.com                             */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 2 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

#include <QtPlugin>
#include <QPicture>
#include <QPainter>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QFormLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QSettings>
#include <cmath>

#include <QMessageBox>

#include "document_interface.h"
#include "asciifile.h"

PluginCapabilities AsciiFile::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("plugins_menu", tr("Read ascii points"));
    return pluginCapabilities;
}

QString AsciiFile::name() const
 {
     return (tr("Read ascii points"));
 }

void AsciiFile::execComm(Document_Interface *doc,
                             QWidget *parent, QString cmd)
{
    Q_UNUSED(cmd);
    dibPunto pdt(parent, doc);
    int result = pdt.exec();
    if (result == QDialog::Accepted)
        pdt.procesFile(doc);
}

#define POINT 12

imgLabel::imgLabel(QWidget * parent, Qt::WindowFlags f) :
    QLabel(parent, f)
{
    posimage = new QPicture;
    posimage->setBoundingRect(QRect(0,0,POINT*8,POINT*8));
    currPos = DPT::N;
    drawImage();
    setPicture(*posimage);
}

void imgLabel::drawImage()
{
    int a1, a2, a3, a4;
    int b1, b2, b3, b4;
    QPainter painter;
    painter.begin(posimage);
    painter.fillRect ( 0, 0, POINT*8,POINT*8, Qt::black );
    a1 = POINT*1.75;
    a2 = POINT*3.5;
    a3 = POINT*5.25;
    a4 = POINT*6;
    painter.fillRect ( a1, a1, POINT, POINT, Qt::white );//NO
    painter.fillRect ( a2, POINT, POINT, POINT, Qt::white );//N
    painter.fillRect ( POINT, a2, POINT, POINT, Qt::white );//O
    painter.fillRect ( a3, a1, POINT, POINT, Qt::white );//NE
    painter.fillRect ( a1, a3, POINT, POINT, Qt::white );//SO
    painter.fillRect ( a3, a3, POINT, POINT, Qt::white );//SE
    painter.fillRect ( a4, a2, POINT, POINT, Qt::white );//E
    painter.fillRect ( a2, a4, POINT, POINT, Qt::white );//S
    painter.setPen ( Qt::white );
    b1 = POINT*3.2;
    b2 = POINT*3.6;
    b3 = POINT*4;
    b4 = POINT*4.4;
    painter.drawLine ( b2, b2, b4, b2 );
    painter.drawLine ( b2, b2, b2, b4 );
    painter.drawLine ( b4, b2, b4, b4 );
    painter.drawLine ( b2, b4, b4, b4 );
    b4 = POINT*4.8;
    painter.drawLine ( b1, b3, b4, b3 );
    painter.drawLine ( b3, b1, b3, b4 );

    switch (currPos) {
    case DPT::NO:
        a2 = a1 = POINT*1.75;
        break;
    case DPT::O:
        a1 = POINT;
        a2 = POINT*3.5;
        break;
    case DPT::NE:
        a1 = POINT*5.25;
        a2 = POINT*1.75;
        break;
    case DPT::SO:
        a1 = POINT*1.75;
        a2 = POINT*5.25;
        break;
    case DPT::SE:
        a2 = a1 = POINT*5.25;
        break;
    case DPT::E:
        a1 = POINT*6;
        a2 = POINT*3.5;
        break;
    case DPT::S:
        a1 = POINT*3.5;
        a2 = POINT*6;
        break;
    default: //N
        a1 = POINT*3.5;
        a2 = POINT;
    }
    painter.fillRect ( a1, a2, POINT, POINT, Qt::red );
    painter.end();
    update ();
}

void imgLabel::changePos(int x, int y)
{
    if (x < POINT*3.1) {
        if (y < POINT*3.1) { setPos(DPT::NO); }
        else if (y < POINT*4.9) { setPos(DPT::O); }
        else { setPos(DPT::SO); }

    } else if (x < POINT*4.9) {
        if (y < POINT*4) { setPos(DPT::N); }
        else { setPos(DPT::S); }

    } else {
        if (y < POINT*3.1) { setPos(DPT::NE); }
        else if (y < POINT*4.9) { setPos(DPT::E); }
        else { setPos(DPT::SE); }
    }
}

void imgLabel::setPos(DPT::txtposition pos)
{
    currPos = pos;
    drawImage();
}

void imgLabel::mouseReleaseEvent(QMouseEvent *event)
 {
    if (event->button() == Qt::LeftButton) {
        changePos(event->x(), event->y());
     } else {
         QLabel::mousePressEvent(event);
     }
 }

/*****************************/
pointBox::pointBox(const QString & title, const QString & label, QWidget * parent ) :
    QGroupBox(title, parent)
{
    rb = new QCheckBox(label);
    rb->setTristate (false );
    vbox = new QVBoxLayout;
    vbox->addWidget(rb);
    QLabel *but = new QLabel(tr("Layer:"));
    layedit = new QLineEdit();
    QHBoxLayout *lolayer = new QHBoxLayout;
    lolayer->addWidget(but);
    lolayer->addWidget(layedit);
    vbox->addLayout(lolayer);
    setLayout(vbox);
}
void pointBox::setInLayout(QLayout *lo)
{
    vbox->addLayout(lo);
}
pointBox::~pointBox()
{

}
/*****************************/
textBox::textBox(const QString & title, const QString & label,
		 const QStringList& fonts, QWidget * parent) :
    pointBox(title, label, parent)
{
    combostyle = new QComboBox();
    combostyle->addItems(fonts);
    QDoubleValidator *val = new QDoubleValidator(0);
    val->setBottom ( 0.0 );
    heightedit = new QLineEdit();
    heightedit->setValidator(val);
    sepedit = new QLineEdit();
    sepedit->setValidator(val);

    QFormLayout *flo = new QFormLayout;
    flo->addRow( tr("Style:"), combostyle);
    flo->addRow( tr("Height:"), heightedit);
    flo->addRow( tr("Separation"), sepedit);
//    posimage.fill(Qt::black);
    img = new imgLabel();
    QHBoxLayout *loimage = new QHBoxLayout;
    loimage->addLayout(flo);
    loimage->addWidget(img);

    setInLayout(loimage);
}

textBox::~textBox()
{

}

void textBox::setStyleStr(const QString& sty)
{
  combostyle->setCurrentIndex(0);
  combostyle->setCurrentText(sty);
}

/*****************************/
dibPunto::dibPunto(QWidget *parent, Document_Interface* doc) :  QDialog(parent)
{
//    setParent(parent);
    setWindowTitle(tr("Read ascii points"));

    QGridLayout *mainLayout = new QGridLayout;
//readSettings();

    QPushButton *filebut = new QPushButton(tr("File..."));
    fileedit = new QLineEdit();
    QHBoxLayout *lofile = new QHBoxLayout;
    lofile->addWidget(filebut);
    lofile->addWidget(fileedit);
    uint row = 0;
    mainLayout->addLayout(lofile, row, 0);

    QLabel *formatlabel = new QLabel(tr("Format:"));
    formatedit = new QComboBox();
    QStringList txtformats;
    txtformats << tr("One point per line") << tr("Multiple points per line")
	       << tr("*.odb for Psion 2");
    formatedit->addItems(txtformats);

    QHBoxLayout *foformat = new QHBoxLayout;
    foformat->addStretch();
    foformat->addWidget(formatlabel);
    foformat->addWidget(formatedit);
    mainLayout->addLayout(foformat, row++, 1);

    QLabel *seplabel = new QLabel(tr("Separator:"));
    separedit = new QComboBox();
    QStringList sepformats;
    sepformats << tr("Comma") << tr("Tab") << tr("Space") << tr("Spaces");
    separedit->addItems(sepformats);
    polyperline = new QCheckBox(tr("1 poly / file line"));

    QHBoxLayout *sepformat = new QHBoxLayout;
    sepformat->addWidget(seplabel);
    sepformat->addWidget(separedit);
    sepformat->addWidget(polyperline);
    mainLayout->addLayout(sepformat, row, 1);

    QLabel *connectlabel = new QLabel(tr("Connect:"));
    connectedit = new QComboBox();
    QStringList connections;
    connections << tr("No (points only)") << tr("With Lines")
		<< tr("As Polyline");
    connectedit->addItems(connections);
    closepoly = new QCheckBox(tr("Close Polylines"));

    QHBoxLayout *coformat = new QHBoxLayout;
    coformat->addWidget(connectlabel);
    coformat->addWidget(connectedit);
    coformat->addWidget(closepoly);
    coformat->addSpacing(20);
    mainLayout->addLayout(coformat, row++, 0);

    pt2d = new pointBox(tr("2D Point"),tr("Draw 2D Point"));
    pt3d = new pointBox(tr("3D Point"),tr("Draw 3D Point"));
    QStringList fontl;
    doc->getFontlist(&fontl);
    ptnumber = new textBox(tr("Point Number"), tr("Draw point number"), fontl);
    ptelev = new textBox(tr("Point Elevation"), tr("Draw point elevation"),
			 fontl);
    ptcode = new textBox(tr("Point Code"), tr("Draw point code"), fontl);
    ptnumber->setPos(DPT::NO);

    QVBoxLayout *lo2d3d = new QVBoxLayout;

    lo2d3d->addWidget(pt2d);
    lo2d3d->addWidget(pt3d);
    mainLayout->addLayout(lo2d3d, row, 0);

    mainLayout->addWidget(ptnumber, row++, 1);
    mainLayout->addWidget(ptelev, row, 0);
    mainLayout->addWidget(ptcode, row++, 1);

    QHBoxLayout *loaccept = new QHBoxLayout;
    QPushButton *acceptbut = new QPushButton(tr("Accept"));
    QPushButton *helpbut = new QPushButton(tr("Help"));
    loaccept->addStretch();
    loaccept->addWidget(helpbut);
    loaccept->addStretch();
    loaccept->addWidget(acceptbut);
    mainLayout->addLayout(loaccept, row, 0);

    QPushButton *cancelbut = new QPushButton(tr("Cancel"));
    QHBoxLayout *locancel = new QHBoxLayout;
    locancel->addWidget(cancelbut);
    locancel->addStretch();
    mainLayout->addLayout(locancel, row++, 1);

    setLayout(mainLayout);
    readSettings();

    connect(cancelbut, SIGNAL(clicked()), this, SLOT(reject()));
    connect(acceptbut, SIGNAL(clicked()), this, SLOT(checkAccept()));
    connect(helpbut, SIGNAL(clicked()), this, SLOT(showHelp()));

    connect(filebut, SIGNAL(clicked()), this, SLOT(dptFile()));
}

void dibPunto::checkAccept()
{

    errmsg.clear();
    if (failGUI(&errmsg)) {
        QMessageBox::critical ( this, "Asciifile plugin", errmsg );
        errmsg.clear();
        return;
    }
    writeSettings();
    accept();
}

void dibPunto::showHelp()
{
  QMessageBox
    helpbox(QMessageBox::Information, tr("asciifile help"),
	    tr("This plugin plots points, the coordinates of which are"
	       " read from an ascii file."),
	    QMessageBox::Ok, this);
  helpbox.setDetailedText(
   tr(R"HELP(There are currently three formats for the file read by this plugin:
(A) One point per line.  (B) Multiple points per line.
(C) Organizer database (.odb file) for the Psion 2
For (A)/(B), one can choose the character(s) that separate coordinates
within a string that represents a point: comma, tab, space, or
multiple spaces. In (A), each line (with newline removed) is considered one
point, and blank lines separate polys. In (B), each substring of a line that
matches the representation of a point is extracted and interpreted as a point.
Also in (B), polys may be separated by blank lines, or the points on each
line may be treated as a poly (by checking the box). Format (C) is specialized,
search for details online; currently the entire file is treated as a single
poly.
Points may be plotted by themselves, connected sequentially by lines, or each
poly may be connected by a polyline (optionally closed if the box is checked).
The option to plot 3D points is currently disabled pending 3D support in
LibreCad; however, you may display the point's number, code, or
elevation (z-coordinate) next to each point, in a configurable layer, size,
and location relative to the point.
These additional pieces of information are determined from the information
in the file as follows. Each point representation read may have from 2 to 5
coordinates; depending on how many are present, they are interpreted in order
as follows:
2: X,Y;   3: PT#,X,Y;   4: PT#,X,Y,Z;   5: PT#,X,Y,Z,CODE
where "PT#" represents the point's number, and CODE can be an arbitrary string.
If PT# is not present, the point number is simply incremented.



)HELP")); // leave blank lines above, otherwise end of help invisible
  helpbox.setStyleSheet("QMessageBoxDetailsText {min-width:270em;}"
			" QTextEdit {min-height:30ex}");
  helpbox.exec();
}

void dibPunto::dptFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"));
    fileedit->setText(fileName);
}

bool dibPunto::failGUI(QString *msg)
{
//Warning, can change adding or reordering "separedit" or "formatedit"
    if (separedit->currentIndex() < 0 or separedit->currentIndex() > 3) {
        msg->insert(0, tr("Impossible separator chosen")); return true;
    }
    if (formatedit->currentIndex() == 2 and separedit->currentIndex() != 1) {
        msg->insert(0, tr("Separator for odb file must be TAB")); return true;
    }
    if (pt2d->checkOn() == true) {
        if (pt2d->getLayer().isEmpty()) {msg->insert(0, tr("Point 2D layer is empty")); return true;}
    }
    if (pt3d->checkOn() == true) {
        if (pt3d->getLayer().isEmpty()) {msg->insert(0, tr("Point 3D layer is empty")); return true;}
    }
    if (ptelev->checkOn() == true) {
        if (ptelev->getLayer().isEmpty()) {msg->insert(0, tr("Point elevation layer is empty")); return true;}
        if (ptelev->getHeightStr().isEmpty()) {msg->insert(0, tr("Point elevation height is empty")); return true;}
        if (ptelev->getSeparationStr().isEmpty()) {msg->insert(0, tr("Point elevation separation is empty")); return true;}
    }
    if (ptnumber->checkOn() == true) {
        if (ptnumber->getLayer().isEmpty()) {msg->insert(0, tr("Point number layer is empty")); return true;}
        if (ptnumber->getHeightStr().isEmpty()) {msg->insert(0, tr("Point number height is empty")); return true;}
        if (ptnumber->getSeparationStr().isEmpty()) {msg->insert(0, tr("Point number separation is empty")); return true;}
    }
    if (ptcode->checkOn() == true) {
        if (ptcode->getLayer().isEmpty()) {msg->insert(0, tr("Point code layer is empty")); return true;}
        if (ptcode->getHeightStr().isEmpty()) {msg->insert(0, tr("Point code height is empty")); return true;}
        if (ptcode->getSeparationStr().isEmpty()) {msg->insert(0, tr("Point code separation is empty")); return true;}
    }
    return false;
}

void dibPunto::procesFile(Document_Interface *doc)
{
    QString sep;
    QString::SplitBehavior skip = QString::KeepEmptyParts;

    QMessageBox msgbox(QMessageBox::Information, tr("asciifile plugin"),
		       tr("Processing file..."));
    msgbox.show();
    currDoc = doc;

//Warning, can change adding or reordering "separedit"
    switch (separedit->currentIndex()) {
    case 0: sep = ","; break;
    case 1: sep = "\t"; break;
    case 3: skip = QString::SkipEmptyParts; // FALL THROUGH
    case 2: sep = " "; break;
    }
    if (!QFile::exists(fileedit->text()) ) {
        QMessageBox::critical ( this, "DibPunto", QString(tr("The file %1 not exist")).arg(fileedit->text()) );
        return;
    }
    QFile infile(fileedit->text());
    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical ( this, "DibPunto", QString(tr("Can't open the file %1")).arg(fileedit->text()) );
         return;
    }

//Warning, can change adding or reordering "formatedit"
    switch (formatedit->currentIndex()) {
    case 1: procesfileNormal(&infile, sep, skip, true); break;
    case 2: procesfileODB(&infile, sep); break;
    default: procesfileNormal(&infile, sep, skip);
    }
    infile.close ();
    QString currlay = currDoc->getCurrentLayer();

    if (pt2d->checkOn() == true)
        draw2D();
    if (pt3d->checkOn() == true)
        draw3D();
    if (ptelev->checkOn() == true)
        drawElev();
    if (ptnumber->checkOn() == true)
        drawNumber();
    if (ptcode->checkOn() == true)
        drawCode();

    currDoc->setLayer(currlay);
    /* draw lines in current layer */
    switch ( connectedit->currentIndex() ) {
    case 1: drawLine(); break;
    case 2: drawPoly(); break;
    default: break;
    }

    currDoc = NULL;
    msgbox.close();
}

void dibPunto::drawLine()
{
    QPointF prevP, nextP;
    int i;

    for (i = 0; i < dataList.size(); ++i) {
        pointData *pd = dataList.at(i);
        if (!pd->x.isEmpty() && !pd->y.isEmpty()){
            prevP.setX(pd->x.toDouble());
            prevP.setY(pd->y.toDouble());
            break;
        }
    }
    for (; i < dataList.size(); ++i) {
        pointData *pd = dataList.at(i);
        if (!pd->x.isEmpty() && !pd->y.isEmpty()){
            nextP.setX(pd->x.toDouble());
            nextP.setY(pd->y.toDouble());
            currDoc->addLine(&prevP, &nextP);
            prevP = nextP;
        }
    }
}

void dibPunto::drawPoly()
{
    std::vector<Plug_VertexData> pp;
    for (int i = 0; i < dataList.size(); ++i) {
      pointData *pd = dataList.at(i);
      if (!pd->x.isEmpty() && !pd->y.isEmpty()) {
	pp.resize(pp.size()+1);
	pp.back().bulge = 0;
	pp.back().point.setX(pd->x.toDouble());
	pp.back().point.setY(pd->y.toDouble());
      }
      if (pd->endsPoly) {
	currDoc->addPolyline(pp, closepoly->isChecked());
	pp.resize(0);
      }
    }
}

void dibPunto::draw2D()
{
    QPointF pt;
    currDoc->setLayer(pt2d->getLayer());
    for (int i = 0; i < dataList.size(); ++i) {
        pointData *pd = dataList.at(i);
        if (!pd->x.isEmpty() && !pd->y.isEmpty()){
            pt.setX(pd->x.toDouble());
            pt.setY(pd->y.toDouble());
            currDoc->addPoint(&pt);
        }
    }
}
void dibPunto::draw3D()
{
    QPointF pt;
    currDoc->setLayer(pt3d->getLayer());
    for (int i = 0; i < dataList.size(); ++i) {
        pointData *pd = dataList.at(i);
        if (!pd->x.isEmpty() && !pd->y.isEmpty()){
            pt.setX(pd->x.toDouble());
            pt.setY(pd->y.toDouble());
/*RLZ:3d support            if (pd->z.isEmpty()) pt.setZ(0.0);
            else  pt.setZ(pd->z.toDouble());*/
            currDoc->addPoint(&pt);
        }
    }
}

void dibPunto::calcPos(DPI::VAlign *v, DPI::HAlign *h, double sep,
                 double *x, double *y, DPT::txtposition sit)
{
    double inc, incx, incy;
    DPI::VAlign va;
    DPI::HAlign ha;
    incx = incy = sep;
    inc = sqrt(incx*incx/2);
    switch (sit) {
    case DPT::NO:
        va = DPI::VAlignBottom;
        ha = DPI::HAlignRight;
        incx = -1.0*inc; incy = inc;
        break;
    case DPT::O:
        va = DPI::VAlignMiddle;
        ha = DPI::HAlignRight;
        incx = -1.0*incx; incy = 0.0;
        break;
    case DPT::NE:
        va = DPI::VAlignBottom;
        ha = DPI::HAlignLeft;
        incx = inc; incy = inc;
        break;
    case DPT::SO:
        va = DPI::VAlignTop;
        ha = DPI::HAlignRight;
        incx = -1.0*inc; incy = -1.0*inc;
        break;
    case DPT::SE:
        va = DPI::VAlignTop;
        ha = DPI::HAlignLeft;
        incx = inc; incy = -1.0*inc;
        break;
    case DPT::E:
        va = DPI::VAlignMiddle;
        ha = DPI::HAlignLeft;
        incy = 0.0;
        break;
    case DPT::S:
        va = DPI::VAlignMiddle;
        ha = DPI::HAlignCenter;
        incx = 0.0; incy = -1.0*incy;
        break;
    default: //N
        va = DPI::VAlignBottom;
        ha = DPI::HAlignCenter;
        incx = 0.0;
    }
    *x =incx;
    *y =incy;
    *v =va;
    *h =ha;
}

void dibPunto::drawNumber()
{
    double incx, incy, newx, newy;
    DPI::VAlign va;
    DPI::HAlign ha;
    calcPos(&va, &ha, ptnumber->getSeparation(),
                 &incx, &incy, ptnumber->getPosition());

    currDoc->setLayer(ptnumber->getLayer());
    QString sty = ptnumber->getStyleStr();
    for (int i = 0; i < dataList.size(); ++i) {
        pointData *pd = dataList.at(i);
        if (!pd->x.isEmpty() && !pd->y.isEmpty()) {
            newx = pd->x.toDouble() + incx;
            newy = pd->y.toDouble() + incy;
            QPointF pt(newx,newy);
            currDoc->addText(QString::number(pd->number), sty, &pt,
			     ptnumber->getHeightStr().toDouble(), 0.0, ha, va);
        }
    }
}

void dibPunto::drawElev()
{

    double incx, incy, newx, newy;
    DPI::VAlign va;
    DPI::HAlign ha;
    calcPos(&va, &ha, ptelev->getSeparation(),
                 &incx, &incy, ptelev->getPosition());

    currDoc->setLayer(ptelev->getLayer());
    QString sty = ptelev->getStyleStr();
    for (int i = 0; i < dataList.size(); ++i) {
        pointData *pd = dataList.at(i);
        if (!pd->x.isEmpty() && !pd->y.isEmpty() && !pd->z.isEmpty()){
            newx = pd->x.toDouble() + incx;
            newy = pd->y.toDouble() + incy;
            QPointF pt(newx,newy);
            currDoc->addText(pd->z, sty, &pt, ptelev->getHeightStr().toDouble(), 0.0, ha, va);
        }
    }
}
void dibPunto::drawCode()
{
    double incx, incy, newx, newy;
    DPI::VAlign va;
    DPI::HAlign ha;
    calcPos(&va, &ha, ptcode->getSeparation(),
                 &incx, &incy, ptcode->getPosition());

    currDoc->setLayer(ptcode->getLayer());
    QString sty = ptcode->getStyleStr();
    for (int i = 0; i < dataList.size(); ++i) {
        pointData *pd = dataList.at(i);
        if (!pd->x.isEmpty() && !pd->y.isEmpty() && !pd->code.isEmpty()){
            newx = pd->x.toDouble() + incx;
            newy = pd->y.toDouble() + incy;
            QPointF pt(newx,newy);
            currDoc->addText(pd->code, sty, &pt, ptcode->getHeightStr().toDouble(), 0.0, ha, va);
        }
    }
}

void dibPunto::procesfileODB(QFile* file, QString sep)
{
    QStringList data;
    pointData *pd;

    while (!file->atEnd()) {
        QString line = file->readLine();
        line.remove ( line.size()-2, 1);
        data = line.split(sep);
        pd = new pointData;
        int i = 0;
        int j = data.size();
	int ptnum = 0;
        if (i<j && data.at(i).compare("4")==0 ){
            i = i+2;
            if (i<j) pd->x = data.at(i); else pd->x = QString();
            i++;
            if (i<j) pd->y = data.at(i); else pd->y = QString();
            i++;
            if (i<j) pd->z = data.at(i); else pd->z = QString();
            i++;
            if (i<j) {
	      ptnum = data.at(i).toInt();
	      pd->number = ptnum;
	    } else pd->number = ++ptnum;
            i++;
            if (i<j) pd->code = data.at(i); else pd->code = QString();
        }
        dataList.append(pd);
    }
    if (!dataList.isEmpty()) dataList.last()->endsPoly = true;
}

static pointData* fieldsToPointData(const QStringList& data, int* ptnum)
{
    if (data.size() < 2) return NULL;

    pointData* pd = new pointData;
    pd->endsPoly = false;
    switch (data.size()) {
    case 2:
        pd->number = ++(*ptnum);
        pd->x = data.at(0);
	pd->y = data.at(1);
	return pd;
    case 5: pd->code = data.at(4); // FALL THROUGH
    case 4: pd->z = data.at(3); // FALL THROUGH
    case 3:
        *ptnum = data.at(0).toInt(); pd->number = *ptnum;
        pd->x = data.at(1);
	pd->y = data.at(2);
    }
    return pd;
}

void dibPunto::procesfileNormal(QFile* file, QString sep,
				QString::SplitBehavior skip, bool multi)
{
    int ptnum = 0;
    while (!file->atEnd()) {
        QString line = file->readLine();
	line.truncate(line.size()-1);
	if (line.isEmpty()) {
	    if (!dataList.isEmpty()) dataList.last()->endsPoly = true;
	    continue;
	}
	QStringList ptex;
	if (multi) {
	    static QString dblre = "[+-]?\\d*[,.]?\\d+(?:[eE][+-]?\\d+)?";
	    QRegularExpression
	        pntre(dblre + "(?:[" + sep + "]+" + dblre + "){1,4}(?:["
		            + sep + "]+\\w+)?");
	    QRegularExpressionMatchIterator m = pntre.globalMatch(line);
	    if (!m.hasNext() and !dataList.isEmpty())
	         dataList.last()->endsPoly = true;
	    while (m.hasNext()) {
	         QRegularExpressionMatch pnt = m.next();
		 ptex << pnt.captured(0);
	    }
	} else ptex << line;
	for (QStringList::iterator p = ptex.begin(); p != ptex.end(); ++p) {
	    pointData* pd = fieldsToPointData(p->split(sep, skip), &ptnum);
	    if (pd == NULL) {
	        if (!multi and !dataList.isEmpty())
		    dataList.last()->endsPoly = true;
	    } else dataList.append(pd);
	}
	if (polyperline->isChecked() and !dataList.isEmpty())
	  dataList.last()->endsPoly = true;
    }
    if (!dataList.isEmpty()) dataList.last()->endsPoly = true;
}

dibPunto::~dibPunto()
{
    while (!dataList.isEmpty())
         delete dataList.takeFirst();
}

void dibPunto::readSettings()
 {
    QString str;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "LibreCAD", "asciifile");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(500,300)).toSize();
    str = settings.value("lastfile").toString();
    fileedit->setText(str);
    formatedit->setCurrentIndex( settings.value("format", 0).toInt() );
    separedit->setCurrentIndex( settings.value("separator", 0).toInt() );
    connectedit->setCurrentIndex(settings.value("connectpoints", 0).toInt() );
    closepoly->setChecked(settings.value("closepoly", false).toBool() );
    polyperline->setChecked(settings.value("polyperline", false).toBool() );
    pt2d->setCheck( settings.value("draw2d", false).toBool() );
    str = settings.value("layer2d").toString();
    pt2d->setLayer(str);
    pt3d->setCheck( settings.value("draw3d", false).toBool() );
    str = settings.value("layer3d").toString();
    pt3d->setLayer(str);
    ptelev->setCheck( settings.value("drawelev", false).toBool() );
    str = settings.value("layerelev").toString();
    ptelev->setLayer(str);
    ptnumber->setCheck( settings.value("drawnumber", false).toBool() );
    str = settings.value("layernumber").toString();
    ptnumber->setLayer(str);
    ptcode->setCheck( settings.value("drawcode", false).toBool() );
    str = settings.value("layercode").toString();
    ptcode->setLayer(str);
    ptelev->setStyleStr( settings.value("styleelev").toString() );
    ptnumber->setStyleStr( settings.value("stylenumber").toString() );
    ptcode->setStyleStr(settings.value("stylecode").toString() );
    ptelev->setHeight( settings.value("heightelev", 0.5).toDouble() );
    ptnumber->setHeight( settings.value("heightnumber", 0.5).toDouble() );
    ptcode->setHeight( settings.value("heightcode", 0.5).toDouble() );
    ptelev->setSeparation( settings.value("separationelev", 0.3).toDouble() );
    ptnumber->setSeparation( settings.value("separationnumber", 0.3).toDouble() );
    ptcode->setSeparation( settings.value("separationcode", 0.3).toDouble() );
    ptelev->setPosition( static_cast<DPT::txtposition>( settings.value("positionelev", DPT::S).toInt() ) );
    ptnumber->setPosition( static_cast<DPT::txtposition>( settings.value("positionnumber", DPT::N).toInt() ) );
    ptcode->setPosition( static_cast<DPT::txtposition>( settings.value("positioncode", DPT::E).toInt() ) );
    resize(size);
    move(pos);
 }

void dibPunto::writeSettings()
 {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "LibreCAD", "asciifile");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("lastfile", fileedit->text());
    settings.setValue("format", formatedit->currentIndex());
    settings.setValue("draw2d", pt2d->checkOn());
    settings.setValue("draw3d", pt3d->checkOn());
    settings.setValue("drawelev", ptelev->checkOn());
    settings.setValue("drawnumber", ptnumber->checkOn());
    settings.setValue("drawcode", ptcode->checkOn());
    settings.setValue("closepoly", closepoly->isChecked());
    settings.setValue("connectpoints", connectedit->currentIndex());
    settings.setValue("layer2d", pt2d->getLayer());
    settings.setValue("layer3d", pt3d->getLayer());
    settings.setValue("layerelev", ptelev->getLayer());
    settings.setValue("layernumber", ptnumber->getLayer());
    settings.setValue("layercode", ptcode->getLayer());
    settings.setValue("styleelev", ptelev->getStyleStr());
    settings.setValue("stylenumber", ptnumber->getStyleStr());
    settings.setValue("stylecode", ptcode->getStyleStr());
    settings.setValue("heightelev", ptelev->getHeightStr());
    settings.setValue("heightnumber", ptnumber->getHeightStr());
    settings.setValue("heightcode", ptcode->getHeightStr());
    settings.setValue("separationelev", ptelev->getSeparationStr());
    settings.setValue("separationnumber", ptnumber->getSeparationStr());
    settings.setValue("separationcode", ptcode->getSeparationStr());
    settings.setValue("separator", separedit->currentIndex());
    settings.setValue("polyperline", polyperline->isChecked());
    settings.setValue("positionelev", ptelev->getPosition());
    settings.setValue("positionnumber", ptnumber->getPosition());
    settings.setValue("positioncode", ptcode->getPosition());
 }
