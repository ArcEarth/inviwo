/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2015 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 *********************************************************************************/

#include <QMenu>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QKeyEvent>
#include <QLabel>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/qt/editor/consolewidget.h>

namespace inviwo {

ConsoleWidget::ConsoleWidget(QWidget* parent) : InviwoDockWidget(tr("Console"), parent)
    , infoTextColor_(153,153,153), warnTextColor_(221,165,8), errorTextColor_(255,107,107)
    , errorsLabel_(NULL), warningsLabel_(NULL), infoLabel_(NULL)
    , numErrors_(0), numWarnings_(0), numInfos_(0)
{
    setObjectName("ConsoleWidget");
    setAllowedAreas(Qt::BottomDockWidgetArea);
    textField_ = new QTextEdit(this);
    textField_->setReadOnly(true);
    textField_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    textField_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(textField_, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

    QHBoxLayout *statusBar = new QHBoxLayout();

    errorsLabel_ = new QLabel("0", this);
    warningsLabel_ = new QLabel("0", this);
    infoLabel_ = new QLabel("0", this);

    QFrame* line1 = new QFrame();
    QFrame* line2 = new QFrame();
    QFrame* line3 = new QFrame();
    line1->setFrameShape(QFrame::VLine);
    line2->setFrameShape(QFrame::VLine);
    line3->setFrameShape(QFrame::VLine);
    
    line1->setFrameShadow(QFrame::Raised);
    line2->setFrameShadow(QFrame::Raised);
    line3->setFrameShadow(QFrame::Raised);

    statusBar->addWidget(new QLabel( "<img width='16' height='16' src=':/icons/error.png'>", this));
    statusBar->addWidget(errorsLabel_);
    statusBar->addWidget(line1);
    statusBar->addWidget(new QLabel( "<img width='16' height='16' src=':/icons/warning.png'>", this));
    statusBar->addWidget(warningsLabel_);
    statusBar->addWidget(line2);
    statusBar->addWidget(new QLabel( "<img width='16' height='16' src=':/icons/info.png'>", this));
    statusBar->addWidget(infoLabel_);
    statusBar->addWidget(line3);
    

    statusBar->addItem(new QSpacerItem(40, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textField_);
    layout->addLayout(statusBar);

    layout->setContentsMargins(3, 0, 0, 3);


    QWidget* w = new QWidget();
    w->setLayout(layout);
    setWidget(w);

}

ConsoleWidget::~ConsoleWidget() {
}

void ConsoleWidget::showContextMenu(const QPoint& pos) {
    QMenu* menu = textField_->createStandardContextMenu();
    QAction* clearAction = menu->addAction("Clear console");
    clearAction->setShortcut(Qt::ControlModifier + Qt::Key_E);
    menu->addAction(clearAction);
    QAction* result = menu->exec(QCursor::pos());

    if (result == clearAction) {
        textField_->clear();
        errorsLabel_->setText("0");
        warningsLabel_->setText("0");
        infoLabel_->setText("0");
        numErrors_ = numWarnings_ = numInfos_ = 0;
    }

    delete menu;
}

void ConsoleWidget::log(std::string logSource, unsigned int logLevel, const char* fileName,
                        const char* functionName, int lineNumber, std::string logMsg) {
    IVW_UNUSED_PARAM(functionName);
    QString message;

    switch (logLevel) {
         case Error: {
            textField_->setTextColor(errorTextColor_);
            std::ostringstream lineNumberStr;
            lineNumberStr << lineNumber;
            message = QString::fromStdString("(" + logSource + ") [" + std::string(fileName) +
                ", " + lineNumberStr.str() + "]: " + logMsg);
            errorsLabel_->setText(toString(++numErrors_).c_str());
            break;
        }
    
        case Warn: {
            textField_->setTextColor(warnTextColor_);
            std::ostringstream lineNumberStr;
            lineNumberStr << lineNumber;
            message = QString::fromStdString("(" + logSource + ") [" + std::string(fileName) +
                ", " + lineNumberStr.str() + "]: " + logMsg);
            warningsLabel_->setText(toString(++numWarnings_).c_str());
            break;
        }
        
        case Info:
        default: {
            textField_->setTextColor(infoTextColor_);
            message = QString::fromStdString("(" + logSource + ") " + logMsg);
            infoLabel_->setText(toString(++numInfos_).c_str());
            break;
        }
    }

    textField_->append(message);
    QTextCursor c =  textField_->textCursor();
    c.movePosition(QTextCursor::End);
    textField_->setTextCursor(c);
}

void ConsoleWidget::keyPressEvent(QKeyEvent* keyEvent) {
	if (keyEvent->key() == Qt::Key_E && keyEvent->modifiers() == Qt::ControlModifier){
        textField_->clear();
        errorsLabel_->setText("0");
        warningsLabel_->setText("0");
        infoLabel_->setText("0");
        numErrors_ = numWarnings_ = numInfos_ = 0;
    }
}

} // namespace