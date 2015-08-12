#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QLabel>
#include <QMenu>
#include <QSplitter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QGridLayout>

#include "Compiler.h"
#include "VirtualPLC.h"
#include "QSyntaxHL.h"
#include "QScriptEditor.h"

namespace Ui
{
class MainWindow;
}

class QPlcDialog;
class QCompleter;
class QAbstractItemModel;

class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent* event);

private slots:
  void newFile();
  bool open();
  bool loadFile(const QString& fileName);
  bool save();
  bool saveAs();
  bool saveFile(const QString& fileName);
  bool openBin();
  bool loadBinFile(const QString& fileName);
  bool saveBin();
  bool saveBinAs();
  bool saveBinFile(const QString& fileName);
  void about();
  void info();
  void openRecentFile();
  void openRecentBinFile();

  void build();
  void plcLoad();
  void plcStartStop();
  void plcClear();

  void updateStatusBar();
  void scriptModified();

  void openToLoad();
  void buildToLoad();

private:
  Ui::MainWindow *ui;

public:
  Compiler* compiler;
  VirtualPLC* virtualPLC;

private:
  bool PLCisStarted;

  QPushButton* openToLoadButton;
  QPushButton* buildToLoadButton;
  QPushButton* startstopButton;
  QPushButton* clearButton;

  QPlcDialog* plcDialog;
  QSplitter* rightSplitter;
  QSplitter* leftSplitter;
  QSplitter* mainSplitter;

  //    QAbstractItemModel* modelFromFile;
  QCompleter* completer;
  QScriptEditor* editer;
  QTextEdit* messages;
  QSyntaxHighlighter* syntaxHL;

  QLabel* locationLabel;
  QStringList recentFiles;
  QStringList recentBinFiles;
  QString curFile;
  QString curBinFile;

  enum { MaxRecentFiles = 5 };
  QAction* recentFileActions[MaxRecentFiles];
  QAction* recentBinFileActions[MaxRecentFiles];
  QAction* separatorAction;
  QAction* separatorBinAction;

  QAction* newAction;
  QAction* openAction;
  QAction* saveAction;
  QAction* saveAsAction;
  QAction* openBinAction;
  QAction* saveBinAction;
  QAction* saveBinAsAction;
  QAction* aboutAction;
  QAction* infoAction;
  QAction* exitAction;

  QAction* buildAction;
  QAction* loadPLCaction;
  QAction* PLCclearAction;
  QAction* PLCStartStopAction;
  QAction* PLCMonitorAction;

  QMenu* fileMenu;
  QMenu* projectMenu;
  QMenu* onlineMenu;
  QMenu* editMenu;
  QMenu* helpMenu;

  void createActions();
  void createMenus();
  void createStatusBar();
  void readSettings();
  void writeSettings();
  bool okToContinue();
  void setCurrentFile(const QString& fileName);
  void setCurrentBinFile(const QString& fileName);
  void updateRecentFileActions();
  void updateRecentBinFileActions();
  QString strippedName(const QString& fullFileName);
  QAbstractItemModel* modelFromFile(const QString& fileName);
};

#endif // MAINWINDOW_H
