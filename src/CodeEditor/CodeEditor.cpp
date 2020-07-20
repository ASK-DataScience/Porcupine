/*  Copyright (C) Tim van Mourik, 2014-2016
    Copyright (C) Tim van Mourik, 2017, DCCN
    All rights reserved

 This file is part of the Porcupine pipeline tool, see
 https://github.com/TimVanMourik/Porcupine for the documentation and
 details.

    This toolbox is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This toolbox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the fmri analysis toolbox. If not, see
    <http://www.gnu.org/licenses/>.
*/

#include <QDebug>
#include <QFileDialog>
#include <QTextEdit>
#include <QProcess>
#include <QMessageBox>

#include "CodeEditor.hpp"
#include "CodeGenerator.hpp"
#include "BashHighlighter.hpp"
#include "MatlabHighlighter.hpp"
#include "PythonHighlighter.hpp"
#include "TvmGenerator.hpp"
#include "NipypeGenerator.hpp"
#include "DockerGenerator.hpp"
#include "ParameterEditor.hpp"

CodeEditor::CodeEditor(
        QWidget* _parent
        ) :
    QTabWidget(_parent)
{
}

void CodeEditor::setParameterEditor(
        ParameterEditor* _editor
        )
{
    m_parameterEditor = _editor;
}


QMap<QString, QString> CodeEditor::getParameters(
        )
{
    return m_parameterEditor->getParameters();
}

bool CodeEditor::isPresentInEditor(
        const QString& _language,
        const QList<NodeTreeItem*>& _nodeList
        )
{
    foreach(const NodeTreeItem* item, _nodeList)
    {
        QJsonObject json = item->getJson();
        Argument title(json["title"].toObject());
        if(!title.getArgument(_language).isEmpty())
        {
            return true;
        }
    }
    return false;
}

void CodeEditor::generateCode(
        const QList<NodeTreeItem*>& _nodeList,
        const QVector<const Link*>& _linkList
        )
{
    QString language;
    language = "Nipype";
    if(isPresentInEditor(language, _nodeList))
    {
        if(!m_textEditors[language])
        {
            setupNipypeEditor();
        }
        m_textEditors[language]->setPlainText(m_codeGenerators[language]->generateCode(_nodeList, _linkList));

        QFile styleFile(":/qss/codeEditor.qss");
        styleFile.open(QFile::ReadOnly);
        QString translucentLabel = QString::fromLatin1(styleFile.readAll());
        m_textEditors[language]->setStyleSheet(translucentLabel);
    }

    language = "TvM";
    if(isPresentInEditor(language, _nodeList))
    {
        if(!m_textEditors[language])
        {
            setupTvmEditor();
        }
        m_textEditors[language]->setPlainText(m_codeGenerators[language]->generateCode(_nodeList, _linkList));

        QFile styleFile(":/qss/codeEditor.qss");
        styleFile.open(QFile::ReadOnly);
        QString translucentLabel = QString::fromLatin1(styleFile.readAll());
        m_textEditors[language]->setStyleSheet(translucentLabel);
    }

    language = "Docker";
    if(isPresentInEditor(language, _nodeList))
    {
        if(!m_textEditors[language])
        {
            setupDockerEditor();
        }
        m_textEditors[language]->setPlainText(m_codeGenerators[language]->generateCode(_nodeList, _linkList));

        QFile styleFile(":/qss/codeEditor.qss");
        styleFile.open(QFile::ReadOnly);
        QString translucentLabel = QString::fromLatin1(styleFile.readAll());
        m_textEditors[language]->setStyleSheet(translucentLabel);
    }
}

void CodeEditor::saveCodeToFile(
        )
{
    QTextEdit* currentTextEdit = (QTextEdit*) currentWidget();
    if(currentTextEdit)
    {
        QString fileName = QFileDialog::getSaveFileName();
        if (fileName.isEmpty())
        {
            return;
        }
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(currentTextEdit->toPlainText().toUtf8());
        }
        file.close();
    }
}

void CodeEditor::runCodeCMD()
{
    //QProcess cmd;
    //cmd.start("cmd");
   // QString command = " docker run --rm -ti";
    //        command = command + " --mount type=bind,source=\"%cd%\"/Data,target=/Data";
     //       command = command + " --mount type=bind,source=\"%cd%\"/results2,target=/Porcupine/";
      //      command = command + " --mount type=bind,source=\"%cd%\"/results2,target=/tmp/";
       //     command = command + " nipype-fmri-por2";

    //Docker Build
    QString commandBuild = "docker build -t nipype-fmri-por-code-datasink-all-qt-3subjects-nopar-separateresults -f C:/Users/akhaled/Documents/Dockerfile.txt C:/Users/akhaled/Documents/";
    system(commandBuild.toUtf8());

    //Docker Run
    QString command = " docker run --rm -ti";
            command = command + " --mount type=bind,source=C:/Users/akhaled/Documents/Data,target=/Data";
            command = command + " --mount type=bind,source=C:/Users/akhaled/Documents/results,target=/work/";
            //command = command + " --mount type=bind,source=C:/Users/akhaled/Documents/results,target=/tmp/";
            command = command + " --mount type=bind,source=C:/Users/akhaled/Documents/Data/sub-01/working,target=/results-01/";
            command = command + " --mount type=bind,source=C:/Users/akhaled/Documents/Data/sub-02/working,target=/results-02/";
            command = command + " --mount type=bind,source=C:/Users/akhaled/Documents/Data/sub-03/working,target=/results-03/";
            //command = command + " nipype-fmri-por2";
            command = command + " nipype-fmri-por-code-datasink-all-qt-3subjects-nopar-separateresults";


    /*QMessageBox msgWarning;
            msgWarning.setText(command);
            msgWarning.setIcon(QMessageBox::Warning);
            msgWarning.setWindowTitle("Caution");
            msgWarning.exec();*/
    qDebug()  << "Systemcall: command_name =" << command;
    system(command.toUtf8());
    //QProcess::execute(command.toUtf8());
    //QProcess::execute("cmd /c mkdir Test");


}

void CodeEditor::saveResultsCMD()
{

}

void CodeEditor::setupTvmEditor(
        )
{
    /// @todo put tabWidth in the preferences
    const int tabWidth = 4;
    QString matlab("TvM");
    m_programmingLanguages << matlab;
    QFont matlabFont = QFont("Courier", 10);
    matlabFont.setStyleHint(QFont::Monospace);
    matlabFont.setFixedPitch(true);
    QFontMetrics matlabMetric(matlabFont);

    QTextEdit* matlabEditor = new QTextEdit(this);
    matlabEditor->setFont(matlabFont);
    matlabEditor->setTabStopWidth(tabWidth * matlabMetric.width(' '));

    new MatlabHighlighter(matlabEditor->document());
    m_textEditors[matlab] = matlabEditor;
    m_codeGenerators[matlab] = new TvmGenerator();
    addTab(m_textEditors[matlab], matlab);
}

void CodeEditor::setupNipypeEditor(
        )
{
    const int tabWidth = 4;
    QString python("Nipype");
    m_programmingLanguages << python;
    QFont pythonFont = QFont("Courier", 10);
    pythonFont.setStyleHint(QFont::Monospace);
    pythonFont.setFixedPitch(true);
    QFontMetrics pythonMetric(pythonFont);

    QTextEdit* pythonEditor = new QTextEdit(this);
    pythonEditor->setFont(pythonFont);
    pythonEditor->setTabStopWidth(tabWidth * pythonMetric.width(' '));

    new PythonHighlighter(pythonEditor->document());
    m_textEditors[python] = pythonEditor;
    m_codeGenerators[python] = new NipypeGenerator(this);
    addTab(m_textEditors[python], python);
}

void CodeEditor::setupDockerEditor(
        )
{
    const int tabWidth = 4;
    QString docker("Docker");
    m_programmingLanguages << docker;
    QFont dockerFont = QFont("Courier", 10);
    dockerFont.setStyleHint(QFont::Monospace);
    dockerFont.setFixedPitch(true);
    QFontMetrics dockerMetric(dockerFont);

    QTextEdit* dockerEditor = new QTextEdit(this);
    dockerEditor->setFont(dockerFont);
    dockerEditor->setTabStopWidth(tabWidth * dockerMetric.width(' '));

    m_textEditors[docker] = dockerEditor;
    m_codeGenerators[docker] = new DockerGenerator(this);
    addTab(m_textEditors[docker], docker);
}


CodeEditor::~CodeEditor()
{
}

