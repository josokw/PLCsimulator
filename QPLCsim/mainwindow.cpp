#include <string>
#include <iostream>
#include <boost/version.hpp>

#include <QCompleter>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QSizePolicy>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "AppInfo.h"
#include "QPlcDialog.h"
// VirtualPLC lib headers
#include "LibInfoVPLC.h"
//#include "MemoryConfig.h"
//#include "ProcessorConfig.h"
#include "LibInfoCPLC.h"

using namespace std;
//-- widgets
//plcDialog->setEnabled(false);
//editer->setEnabled(false);
//messages->setEnabled(false);
//openToLoadButton->setEnabled(true);
//buildToLoadButton->setEnabled(true);
//startstopButton->setEnabled(false);
//clearButton->setEnabled(false);
//-- Menu 1
//newAction->setEnabled(false);
//openAction->setEanabled(false);
//saveAction->setEnabled(false);
//saveAsAction->setEnabled(false);
//openBinAction->setEnabled(false);
//saveBinAction->setEnabled(false);
//saveBinAsAction->setEnabled(false);
//-- Menu 2
//buildAction->setEnabled(false);
//buildToStartButton->setEnabled(false);
//-- Menu 3
//PLCStartStopAction->setEnabled(false);
//PLCclearAction->setEnabled(false);

MainWindow::MainWindow(QWidget *parent)
  :  QMainWindow(parent)
  ,  ui(new Ui::MainWindow)
  ,  compiler(new Compiler)
  ,  virtualPLC(new VirtualPLC)
  ,  PLCisStarted(false)
{
  ui->setupUi(this);
  setWindowIcon(QIcon(":/images/ICO64x64.ico"));

  plcDialog = new QPlcDialog(virtualPLC, this);
  plcDialog->setFixedHeight(150);
  plcDialog->setMinimumWidth(1000);
  plcDialog->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  //plcDialog->setEnabled(false);

  // Right splitter
  openToLoadButton = new QPushButton("Open .. Load PLC", this);
  openToLoadButton->setFixedSize(130, 40);
  openToLoadButton->setEnabled(true);
  connect(openToLoadButton, SIGNAL(clicked()), this, SLOT(openToLoad()));

  buildToLoadButton = new QPushButton("Build .. Load PLC", this);
  buildToLoadButton->setFixedSize(130, 40);
  buildToLoadButton->setEnabled(false);
  connect(buildToLoadButton, SIGNAL(clicked()), this, SLOT(buildToLoad()));

  startstopButton = new QPushButton("Start PLC", this);
  startstopButton->setFixedSize(130, 40);
  connect(startstopButton, SIGNAL(clicked()), this, SLOT(plcStartStop()));
  startstopButton->setEnabled(false);

  clearButton = new QPushButton("Clear PLC", this);
  clearButton->setFixedSize(130, 40);
  connect(clearButton, SIGNAL(clicked()), this, SLOT(plcClear()));
  clearButton->setEnabled(false);

  rightSplitter = new QSplitter(Qt::Vertical, this);
  rightSplitter->setFixedWidth(140);
  rightSplitter->addWidget(openToLoadButton);
  rightSplitter->addWidget(buildToLoadButton);
  rightSplitter->addWidget(startstopButton);
  rightSplitter->addWidget(clearButton);

  // Left splitter
  editer  = new QScriptEditor(this);
  editer->setFocus();
  editer->setMinimumSize(600, 400);
  editer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  editer->setCursor(Qt::IBeamCursor);
  editer->setFont(QFont("Courier New", 10));
  editer->setEnabled(true);

  syntaxHL = new QSyntaxHL(editer->document());
  connect(editer->document(), SIGNAL(modificationChanged(bool)), this,
          SLOT(setWindowModified(bool)));
  setWindowModified(editer->document()->isModified());

  completer = new QCompleter(this);
  completer->setModel(modelFromFile("nofile.txt"));
  completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  completer->setWrapAround(false);
  editer->setCompleter(completer);

  messages = new QTextEdit(this);
  messages->setMaximumHeight(150);
  messages->setReadOnly(true);
  messages->setFont(QFont("Courier New", 11));

  leftSplitter = new QSplitter(Qt::Vertical, this);
  leftSplitter->addWidget(editer);
  leftSplitter->addWidget(messages);
  leftSplitter->addWidget(plcDialog);
  leftSplitter->setStretchFactor(1, 1);

  mainSplitter = new QSplitter(Qt::Horizontal, this);
  mainSplitter->addWidget(leftSplitter);
  mainSplitter->addWidget(rightSplitter);
  mainSplitter->setStretchFactor(1, 1);
  mainSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setCentralWidget(mainSplitter);

  setWindowTitle(APPNAME_VERSION);
  createActions();
  createMenus();
  createStatusBar();
  readSettings();
  setCurrentFile("");
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  if (okToContinue())
  {
    writeSettings();
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

void MainWindow::createActions()
{
  newAction = new QAction(tr("&New"), this);
  newAction->setIcon(QIcon(":/images/new.png"));
  newAction->setShortcut(QKeySequence::New);
  newAction->setStatusTip(tr("New empty .plc script"));
  connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

  openAction = new QAction(tr("&Open script"), this);
  openAction->setIcon(QIcon(":/images/open.png"));
  openAction->setShortcut(QKeySequence::Open);
  openAction->setStatusTip(tr("Load new .plc script"));
  connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

  saveAction = new QAction(tr("&Save script"), this);
  saveAction->setIcon(QIcon(":/images/save.png"));
  saveAction->setShortcut(QKeySequence::Save);
  saveAction->setStatusTip(tr("Save .plc script"));
  saveAction->setEnabled(false);
  connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
  //saveAction->setEnabled(editer->document()->isModified());

  saveAsAction = new QAction(tr("Save script &As ..."), this);
  saveAsAction->setIcon(QIcon(":/images/save_as.png"));
  saveAsAction->setShortcut(QKeySequence::SaveAs);
  saveAsAction->setStatusTip(tr("Save .plc script with a new name"));
  saveAsAction->setEnabled(false);
  connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

  openBinAction = new QAction(tr("Open binary"), this);
  openBinAction->setIcon(QIcon(":/images/open.png"));
  //openBinAction->setShortcut(QKeySequence::Open);
  openBinAction->setStatusTip(tr("Load new .plcbin binary"));
  connect(openBinAction, SIGNAL(triggered()), this, SLOT(openBin()));

  saveBinAction = new QAction(tr("Save binary"), this);
  saveBinAction->setIcon(QIcon(":/images/save.png"));
  //saveBinAction->setShortcut(QKeySequence::Save);
  saveBinAction->setStatusTip(tr("Save .plcbin binary"));
  saveBinAction->setEnabled(false);
  connect(saveBinAction, SIGNAL(triggered()), this, SLOT(saveBin()));
  //saveAction->setEnabled(editer->document()->isModified());

  saveBinAsAction = new QAction(tr("Save binary As ..."), this);
  saveBinAsAction->setIcon(QIcon(":/images/save_as.png"));
  //saveBinAsAction->setShortcut(QKeySequence::SaveAs);
  saveBinAsAction->setStatusTip(tr("Save .plcbin with a new name"));
  saveBinAsAction->setEnabled(false);
  connect(saveBinAsAction, SIGNAL(triggered()), this, SLOT(saveBinAs()));

  for (int i = 0; i < MaxRecentFiles; ++i)
  {
    recentFileActions[i] = new QAction(this);
    recentFileActions[i]->setVisible(false);
    connect(recentFileActions[i], SIGNAL(triggered()), this,
            SLOT(openRecentFile()));
  }

  for (int i = 0; i < MaxRecentFiles; ++i)
  {
    recentBinFileActions[i] = new QAction(this);
    recentBinFileActions[i]->setVisible(false);
    connect(recentBinFileActions[i], SIGNAL(triggered()), this,
            SLOT(openRecentBinFile()));
  }

  exitAction = new QAction(tr("&Quit"), this);
  exitAction->setIcon(QIcon(":/images/exit.png"));
  exitAction->setShortcut(tr("Ctrl+Q"));
  exitAction->setStatusTip(tr("Exit the application"));
  connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

  aboutAction = new QAction(tr("A&bout"), this);
  aboutAction->setShortcut(tr("Ctrl+A"));
  aboutAction->setStatusTip(tr("Show the About box"));
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

  infoAction = new QAction(tr("I&nfo"), this);
  infoAction->setShortcut(tr("Ctrl+I"));
  infoAction->setStatusTip(tr("Show the Information box"));
  connect(infoAction, SIGNAL(triggered()), this, SLOT(info()));

  buildAction= new QAction(tr("B&uild"), this);
  buildAction->setStatusTip(tr("Build PLC program"));
  connect(buildAction, SIGNAL(triggered()), this, SLOT(build()));
  buildAction->setEnabled(false);

  loadPLCaction = new QAction(tr("&Load PLC"), this);
  loadPLCaction->setStatusTip(tr("Load PLC with program"));
  connect(loadPLCaction, SIGNAL(triggered()), this, SLOT(plcLoad()));
  loadPLCaction->setEnabled(false);

  PLCclearAction = new QAction(tr("PLC c&lear"), this);
  PLCclearAction->setShortcut(tr("Alt+C"));
  connect(PLCclearAction, SIGNAL(triggered()), this, SLOT(plcClear()));
  PLCclearAction->setEnabled(false);

  PLCStartStopAction = new QAction(tr("PLC Start / Stop"), this);
  PLCStartStopAction->setShortcut(tr("Alt+S"));
  PLCStartStopAction->setStatusTip(tr("Start/Stop PLC"));//setText(completion);
  connect(PLCStartStopAction, SIGNAL(triggered()), this, SLOT(plcStartStop()));
  PLCStartStopAction->setEnabled(false);

  PLCMonitorAction = new QAction(tr("Monitoring Mode"), this);
  PLCMonitorAction->setStatusTip(tr("Monitoring mode"));
  connect(PLCMonitorAction, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAction);
  fileMenu->addSeparator();
  fileMenu->addAction(openAction);
  fileMenu->addAction(saveAction);
  fileMenu->addAction(saveAsAction);
  fileMenu->addSeparator();
  fileMenu->addAction(openBinAction);
  fileMenu->addAction(saveBinAction);
  fileMenu->addAction(saveBinAsAction);

  separatorAction = fileMenu->addSeparator();
  for (int i = 0; i < MaxRecentFiles; ++i)
  {
    fileMenu->addAction(recentFileActions[i]);
  }
  separatorBinAction = fileMenu->addSeparator();
  for (int i = 0; i < MaxRecentFiles; ++i)
  {
    fileMenu->addAction(recentBinFileActions[i]);
  }
  fileMenu->addSeparator();
  fileMenu->addAction(exitAction);
  menuBar()->addSeparator();
  projectMenu = menuBar()->addMenu(tr("&Project"));
  projectMenu->addAction(buildAction);
  projectMenu->addAction(loadPLCaction);
  menuBar()->addSeparator();
  onlineMenu = menuBar()->addMenu(tr("On&line"));
  onlineMenu->addAction(PLCStartStopAction);
  onlineMenu->addAction(PLCclearAction);
  //onlineMenu->addAction(PLCMonitorAction);
  menuBar()->addSeparator();
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(infoAction);
  helpMenu->addAction(aboutAction);
  menuBar()->addSeparator();
}

void MainWindow::createStatusBar()
{
  locationLabel = new QLabel("      ");
  locationLabel->setAlignment(Qt::AlignCenter);
  locationLabel->setMinimumSize(locationLabel->sizeHint());
  statusBar()->addWidget(locationLabel);
  updateStatusBar();
}

void MainWindow::readSettings()
{
  QSettings settings("ONKJ", APPNAME);
  restoreGeometry(settings.value("geometry").toByteArray());
  recentFiles = settings.value("recentFiles").toStringList();
  recentBinFiles = settings.value("recentBinFiles").toStringList();
  updateRecentFileActions();
  updateRecentBinFileActions();
  mainSplitter->restoreState(settings.value("mainSplitter").toByteArray());
  leftSplitter->restoreState(settings.value("leftSplitter").toByteArray());
}

void MainWindow::writeSettings()
{
  QSettings settings("ONKJ", APPNAME);
  settings.setValue("geometry", saveGeometry());
  settings.setValue("recentFiles", recentFiles);
  settings.setValue("recentBinFiles", recentBinFiles);
  settings.setValue("mainSplitter", mainSplitter->saveState());
  settings.setValue("leftSplitter", leftSplitter->saveState());
}

void MainWindow::setCurrentFile(const QString& fileName)
{
  curFile = fileName;
  curBinFile = fileName + "bin";
  setWindowModified(false);
  QString shownName = tr("<untitled>");
  if (!curFile.isEmpty())
  {
    shownName = strippedName(curFile);
    recentFiles.removeAll(curFile);
    recentFiles.prepend(curFile);
    updateRecentFileActions();
  }
  setWindowTitle(APPNAME_VERSION + tr("    - %1[*] %2").arg(shownName)
                 .arg(tr("  PLC script -")));
}

void MainWindow::setCurrentBinFile(const QString& fileName)
{
  curFile = "";
  curBinFile = fileName;
  setWindowModified(false);
  QString shownName = tr("<untitled>");
  if (!curBinFile.isEmpty())
  {
    shownName = strippedName(curBinFile);
    recentBinFiles.removeAll(curBinFile);
    recentBinFiles.prepend(curBinFile);
    updateRecentBinFileActions();
  }
  setWindowTitle(APPNAME_VERSION + tr("    - %1 %2").arg(shownName)
                 .arg(tr("  PLC binary file -")));
}

void MainWindow::newFile()
{
  if (okToContinue())
  {
    virtualPLC->clear();
    compiler->clear();
    setCurrentFile("");
    setCurrentBinFile("");
    editer->clear();
    messages->clear();
    plcDialog->setEnabled(false);
    saveAction->setEnabled(false);
    saveAsAction->setEnabled(false);
    saveBinAction->setEnabled(false);
    saveBinAsAction->setEnabled(false);
    buildAction->setEnabled(false);
    loadPLCaction->setEnabled(false);
    buildToLoadButton->setEnabled(false);
    PLCStartStopAction->setEnabled(false);
    startstopButton->setEnabled(false);
    PLCclearAction->setEnabled(false);
    clearButton->setEnabled(false);
    setCurrentFile("");
  }
}

bool MainWindow::okToContinue()
{
  if (editer->document()->isModified())
  {
    //if (isWindowModified()) {
    saveAction->setEnabled(editer->document()->isModified());
    int r = QMessageBox::warning(this, tr("PLC script"),
                                 tr("The script has been modified.\n"
                                    "Do you want to save your changes?"),
                                 QMessageBox::Yes | QMessageBox::No
                                 | QMessageBox::Cancel);

    if (r == QMessageBox::Yes)
    {
      return save();
    }
    else
    {
      if (r == QMessageBox::Cancel)
      {
        return false;
      }
    }
  }
  return true;
}

bool MainWindow::open()
{
  if (okToContinue())
  {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open PLC script"), ".",
                                                    tr("PLC script files (*.plc)"));
    if (!fileName.isEmpty())
    {
      if (loadFile(fileName))
      {
        setWindowModified(false);
        plcDialog->setEnabled(true);
        saveAction->setEnabled(true);
        saveAsAction->setEnabled(true);
        saveBinAction->setEnabled(true);
        saveBinAsAction->setEnabled(true);
        buildAction->setEnabled(true);
        buildToLoadButton->setEnabled(true);
        editer->document()->setModified(false);
        return true;
      }
    }
    return false;
  }
  return false;
}

bool MainWindow::loadFile(const QString& fileName)
{
  string file(fileName.toLatin1());
  if (!compiler->readFile(file))
  {
    statusBar()->showMessage(tr("Editer: loading script cancelled"), 2000);
    return false;
  }
  editer->clear();
  editer->insertPlainText(QString(compiler->getScript()));
  editer->document()->setModified(false);
  setCurrentFile(fileName);
  messages->clear();
  statusBar()->showMessage(tr("Editer: script loaded"), 2000);
  buildAction->setEnabled(true);
  buildToLoadButton->setEnabled(true);
  return true;
}

bool MainWindow::save()
{
  if (curFile.isEmpty())
  {
    return saveAs();
  }
  else
  {
    return saveFile(curFile);
  }
  return true;
}

bool MainWindow::saveFile(const QString& fileName)
{
  string file(fileName.toLatin1());
  string temp(editer->toPlainText().toLatin1());
  compiler->setScript(temp);
  if (!compiler->writeFile(file))
  {
    statusBar()->showMessage(tr("Editer: saving script cancelled"), 2000);
    return false;
  }
  setCurrentFile(fileName);
  statusBar()->showMessage(tr("Editer: script saved"), 2000);
  editer->document()->setModified(false);
  return true;
}

bool MainWindow::saveAs()
{
  QString fileName =
      QFileDialog::getSaveFileName(this,
                                   tr("Save PLC script"), ".",
                                   tr("PLC script files (*.plc)"));
  if (fileName.isEmpty()) {
    return false;
  }

  return saveFile(fileName);
}

bool MainWindow::openBin()
{
  if (okToContinue())
  {
    QString fileName =
        QFileDialog::getOpenFileName(this,
                                     tr("Open PLC binary file"), ".",
                                     tr("PLC binary files (*.plcbin)"));
    if (!fileName.isEmpty())
    {
      loadBinFile(fileName);
      setWindowModified(false);
      saveAction->setEnabled(false);
      saveAsAction->setEnabled(false);
      buildAction->setEnabled(false);
      buildToLoadButton->setEnabled(false);
      startstopButton->setEnabled(true);
      clearButton->setEnabled(true);
      PLCclearAction->setEnabled(true);
      PLCStartStopAction->setEnabled(true);
      return true;
    }
    return false;
  }
  return false;
}

bool MainWindow::loadBinFile(const QString& fileName)
{
  string file(fileName.toLatin1());
  if (!virtualPLC->readBinFile(file))
  {
    statusBar()->showMessage(tr("PLC: loading binary file cancelled"), 2000);
    return false;
  }
  setCurrentBinFile(fileName);
  editer->clear();
  editer->document()->clear();
  messages->clear();
  statusBar()->showMessage(tr("PLC: binary file loaded"), 2000);
  saveBinAction->setEnabled(true);
  saveBinAsAction->setEnabled(true);
  startstopButton->setEnabled(true);
  clearButton->setEnabled(true);
  buildToLoadButton->setEnabled(false);
  PLCclearAction->setEnabled(true);
  PLCStartStopAction->setEnabled(true);
  startstopButton->setText("Start PLC");
  PLCisStarted = false;
  return true;
}

bool MainWindow::saveBin()
{
  if (curBinFile.isEmpty())
  {
    return saveBinAs();
  }
  else
  {
    return saveBinFile(curBinFile);
  }
  return true;
}

bool MainWindow::saveBinFile(const QString& fileName)
{
  string file(fileName.toLatin1());

  if (!virtualPLC->writeBinFile(file))
  {
    statusBar()->showMessage(tr("PLC: saving binary file cancelled"), 2000);
    return false;
  }
  setCurrentBinFile(fileName);
  statusBar()->showMessage(tr("PLC: binary file saved"), 2000);
  return true;
}

bool MainWindow::saveBinAs()
{
  QString fileName =
      QFileDialog::getSaveFileName(this,
                                   tr("Save PLC binary file"), ".",
                                   tr("PLC binary files (*.plcbin)"));
  if (fileName.isEmpty())
  {
    return false;
  }
  return saveBinFile(fileName);
}

QString MainWindow::strippedName(const QString& fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

void MainWindow::updateRecentFileActions()
{
  QMutableStringListIterator i(recentFiles);
  while (i.hasNext())
  {
    if (!QFile::exists(i.next()))
    {
      i.remove();
    }
  }
  for (int j = 0; j < MaxRecentFiles; ++j)
  {
    if (j < recentFiles.count())
    {
      QString text = tr("&%1    %2").arg(j+1).arg(strippedName(recentFiles[j]));
      recentFileActions[j]->setText(text);
      recentFileActions[j]->setData(recentFiles[j]);
      recentFileActions[j]->setVisible(true);
    }
    else
    {
      recentFileActions[j]->setVisible(false);
    }
  }
  separatorAction->setVisible(!recentFiles.isEmpty());
}

void MainWindow::updateRecentBinFileActions()
{
  QMutableStringListIterator i(recentBinFiles);
  while (i.hasNext())
  {
    if (!QFile::exists(i.next()))
    {
      i.remove();
    }
  }
  for (int j = 0; j < MaxRecentFiles; ++j)
  {
    if (j < recentBinFiles.count())
    {
      QString text = tr("&%1   %2").arg(j+1).arg(strippedName(recentBinFiles[j]));
      recentBinFileActions[j]->setText(text);
      recentBinFileActions[j]->setData(recentBinFiles[j]);
      recentBinFileActions[j]->setVisible(true);
    }
    else
    {
      recentBinFileActions[j]->setVisible(false);
    }
  }
  separatorBinAction->setVisible(!recentBinFiles.isEmpty());
}

void MainWindow::openRecentFile()
{
  if (okToContinue())
  {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
    {
      loadFile(action->data().toString());
    }
  }
}

void MainWindow::openRecentBinFile()
{
  if (okToContinue())
  {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
    {
      loadBinFile(action->data().toString());
    }
  }
}

void MainWindow::about()
{
  static const QString bv(QString(BOOST_LIB_VERSION).replace("_", "."));
  QMessageBox::about(this,
                     tr("About " APPNAME),
                     tr("<h2>" APPNAME_VERSION "</h2>"
                        "<p>PLCsim simulates a Programmable Logic Controller. "
                        "This program uses a script as input containing variable, "
                        "constant, timer and counter declarations, "
                        "mapping of variables to IO, logical expressions, "
                        "comparison functions and one line ladderdiagrams."
                        "</p>"
                        "<p>Main goal: training PLC programming basics.</p>"
                        "<p>Developed by Jos Onokiewicz.</p>"
                        "<p>This apllication uses:</p>"
                        "<p>- Qt version ")
                     .append(QT_VERSION_STR)
                     .append("</p><p>- VirtualPLC C++ library version ")
                     .append(vplc::libVersion.c_str())
                     .append("</p><p>- CompilerPLC C++ library version ")
                     .append(cplc::libVersion.c_str())
                     .append("</p><p>- Boost C++ libraries version ")
                     .append(bv).append("\n</p>"));
}

void MainWindow::info()
{
  QMessageBox::about(this,
                     tr("Info PLCsim"),
                     tr("PLCsim contains a virtual PLC. Some information:"
                        "<p>- Memory size: ")
                     + QString::number(MemoryConfig::MEM_SIZE)
                     + tr("</p>"
                          "<p>- Data types: INT and BOOL</p>"
                          "<p>- Max. number of constants: ")
                     + QString::number(MemoryConfig::CONSTANTS_SIZE)
                     + tr("</p>"
                          "<p>- Max. number of variables: ")
                     + QString::number(MemoryConfig::VARS_SIZE)
                     + tr("</p>"
                          "<p>- Max. number of expressions: ")
                     + QString::number(MemoryConfig::TMPVARS_SIZE)
                     + tr("</p>"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  "<p>- Max. number of timers TIMER_M: ") + QString::number(MemoryConfig::nTIMERS) + tr("</p>"                                                                                                                                                                                                                                                                                                                                                                      "<p>- Max. number of counters COUNTER_M: ") + QString::number(MemoryConfig::nCOUNTERS) + tr("</p>"));
}

void MainWindow::build()
{
  // Set compiler with latest editer contents
  string temp(editer->toPlainText().toLatin1());
  compiler->setScript(temp);

  messages->clear();
  messages->setText("Compiler: compiling script file '"
                    + strippedName(curFile) + "' ....");
  compiler->parse();

  bool errorStatus = false;
  QString message;
  std::vector<Compiler::message_t> compilerMessages = compiler->getMessages();

  // Format messages
  for (size_t i= 0; i < compilerMessages.size(); ++i)
  {
    if (!errorStatus)
    {
      char c = std::get<0>(compilerMessages[i]);
      if (c == 'S' || c == 'E')
      {
        errorStatus = true;
      }
    }
    switch (std::get<0>(compilerMessages[i]))
    {
      case 'S':
        message += "**** System error in line ";
        break;
      case 'E':
        message += "** Error in line ";
        break;
    }

    int lineNumber = std::get<1>(compilerMessages[i]);
    message += QString(to_string(lineNumber).c_str());
    message += QString(": ");
    message += QString(std::get<3>(compilerMessages[i]).c_str());
    messages->append(message);
    message.clear();
  }
  if (!errorStatus)
  {
    locationLabel->setText(tr("Compiler: PLC program compiled"));
    messages->append(tr("Compiler: PLC program compiled"));
  }
  loadPLCaction->setEnabled(!errorStatus);
}

void MainWindow::plcLoad()
{
  if (compiler->containsErrors())
  {
    locationLabel->setText(
          tr("PLC: not loaded with program (script contains errors)"));
    messages->append(
          tr("PLC: not loaded with program (script contains errors)"));
    startstopButton->setEnabled(false);
    clearButton->setEnabled(false);
    saveBinAction->setEnabled(false);
  }
  else
  {
    locationLabel->setText(
          tr("PLC: loaded with program"));
    messages->append(
          tr("PLC: loaded with program"));
    virtualPLC->loadMemory(compiler->getResult());
    PLCisStarted = false;
    PLCStartStopAction->setEnabled(true);
    startstopButton->setEnabled(true);
    clearButton->setEnabled(true);
    PLCclearAction->setEnabled(true);
    saveBinAction->setEnabled(true);
  }
}

void MainWindow::plcClear()
{
  virtualPLC->clear();
  locationLabel->setText(tr("PLC: cleared"));
}

void MainWindow::plcStartStop()
{
  if (!PLCisStarted)
  {
    startstopButton->setText("Stop PLC");
    newAction->setEnabled(false);
    virtualPLC->start();
    locationLabel->setText(tr("PLC: running"));
  }
  else
  {
    startstopButton->setText("Start PLC");
    newAction->setEnabled(true);
    virtualPLC->stop();
    locationLabel->setText(tr("PLC: stopped"));
  }
  PLCisStarted = !PLCisStarted;
}

void MainWindow::updateStatusBar()
{
  //locationLabel->setText(tr("PLC: is off, no program loaded"));
}

void MainWindow::scriptModified()
{
  setWindowModified(true);
  updateStatusBar();
}

void MainWindow::openToLoad()
{
  try {
    if (open())
    {
      build();
      plcLoad();
    }
  }
  catch(std::exception& x)
  {
    std::cerr << "[] ERROR " << x.what() << endl;
  }
  catch(...)
  {
    std::cerr << "[] CRASH" << endl;
  }
}

void MainWindow::buildToLoad()
{
  try {
    build();
    plcLoad();
  }
  catch(std::exception& x)
  {
    std::cerr << "[] ERROR " << x.what() << endl;
  }
  catch(...)
  {
    std::cerr << "[] CRASH" << endl;
  }
}

QAbstractItemModel* MainWindow::modelFromFile(const QString& fileName)
{
  QFile file(fileName);

  //     if (!file.open(QFile::ReadOnly))
  //     {
  //         return new QStringListModel(completer);
  //     }

  QStringList words;
  words.append("CONST");
  words.append("BOOL");
  words.append("INT");
  words.append("TIMER_M");
  words.append("false");
  words.append("true");
  return new QStringListModel(words, completer);
}
