#include "QSyntaxHL.h"
#include <QtGui>

using namespace std;

QSyntaxHL::QSyntaxHL(QTextDocument *parent)
  : QSyntaxHighlighter(parent)
{
  HighlightingRule rule;

  keywordFormat.setForeground(Qt::darkBlue);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList keywordPatterns;
  keywordPatterns
      << "\\bAND\\b"
      << "\\bOR\\b"
      << "\\bXOR\\b"
      << "\\bNOT\\b"
      << "\\bINT\\b"
      << "\\bBOOL\\b"
      << "\\bCONST\\b"
      << "\\bTIMER_M\\b"
      << "\\bCOUNTER_M\\b"
      << "\\btrue\\b"
      << "\\bfalse\\b"
      << "\\bGT\\b"
      << "\\bGE\\b"
      << "\\bEQ\\b"
      << "\\bLE\\b"
      << "\\bLT\\b"
      << "\\bNE\\b";

  for (auto pattern: keywordPatterns)
  {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  intFormat.setFontWeight(QFont::Bold);
  intFormat.setForeground(Qt::darkRed);
  rule.pattern = QRegExp("\\b[0-9]+\\b");
  rule.format = intFormat;
  highlightingRules.append(rule);

  XYioFormat.setFontWeight(QFont::Bold);
  XYioFormat.setForeground(Qt::darkMagenta);
  rule.pattern = QRegExp("\\b[XY][0-9]\\b");
  rule.format = XYioFormat;
  highlightingRules.append(rule);

  ladderDiagramFormat.setForeground(Qt::black);
  ladderDiagramFormat.setBackground(Qt::lightGray);
  ladderDiagramFormat.setFontWeight(QFont::Bold);
  rule.pattern = QRegExp("^[-]+[[]/{0,1}[]]([-]+[(][RS/]{0,1}[)]){1,}[-]+");
  rule.format = ladderDiagramFormat;
  highlightingRules.append(rule);
}

void QSyntaxHL::highlightBlock(const QString &text)
{
  for (int i = 0; i < text.length(); ++i)
  {
    if (text.mid(i, 2) == "//")
    {
      setFormat(i, text.length(), Qt::red);
    }
  }
  setCurrentBlockState(0);
  for (auto rule: highlightingRules)
  {
    QRegExp expression(rule.pattern);
    int index = text.indexOf(expression);
    while (index >= 0)
    {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = text.indexOf(expression, index +length);
    }
  }
}
