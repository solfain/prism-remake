// SPDX-License-Identifier: GPL-3.0-only
/*
 *  SOL Launcher - Minecraft Launcher
 *  Copyright (C) 2026 SOL Launcher Contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTest>

#include "BuildConfig.h"
#include "ui/dialogs/MsaSetupDialog.h"

class MsaSetupTest : public QObject {
    Q_OBJECT

    static constexpr const char* kId = "3f2a6b7c-1d4e-4a5b-8c9d-0e1f2a3b4c5d";

   private slots:
    void clientIdValidation_data()
    {
        QTest::addColumn<QString>("input");
        QTest::addColumn<bool>("valid");
        QTest::newRow("typical") << kId << true;
        QTest::newRow("upper case") << QString(kId).toUpper() << true;
        QTest::newRow("padded") << QString("  ") + kId + " \n" << true;
        QTest::newRow("quoted, as copied from JSON") << QString("\"") + kId + "\"," << true;
        QTest::newRow("braces") << QString("{") + kId + "}" << true;
        QTest::newRow("empty") << "" << false;
        QTest::newRow("too short") << "3f2a6b7c-1d4e-4a5b-8c9d" << false;
        QTest::newRow("not hex") << "zzzzzzzz-1d4e-4a5b-8c9d-0e1f2a3b4c5d" << false;
        QTest::newRow("not a version 4 uuid") << "3f2a6b7c-1d4e-1a5b-8c9d-0e1f2a3b4c5d" << false;
        QTest::newRow("bad variant") << "3f2a6b7c-1d4e-4a5b-0c9d-0e1f2a3b4c5d" << false;
        QTest::newRow("a client secret must never be accepted") << "abC8Q~dEf1234567890.secret_value_not_an_id" << false;
        QTest::newRow("two ids") << QString(kId) + " " + kId << false;
    }
    void clientIdValidation()
    {
        QFETCH(QString, input);
        QFETCH(bool, valid);
        QCOMPARE(MsaSetupDialog::isValidClientId(input), valid);
    }

    void normalizeGivesTheCanonicalForm()
    {
        QCOMPARE(MsaSetupDialog::normalizeClientId(QString("  {") + QString(kId).toUpper() + "}, "), QString(kId));
        QCOMPARE(MsaSetupDialog::normalizeClientId(kId), QString(kId));
    }

    void redirectUrisMatchTheOAuthCallback()
    {
        // MSAStep uses the loopback handler and a callback of "<binary>://oauth/microsoft"; the registration must contain both
        const QStringList uris = MsaSetupDialog::redirectUris();
        QCOMPARE(uris.size(), 2);
        QCOMPARE(uris.at(0), QString("http://localhost"));
        QCOMPARE(uris.at(1), BuildConfig.LAUNCHER_APP_BINARY_NAME + "://oauth/microsoft");
    }

    void linksPointAtMicrosoft()
    {
        QCOMPARE(MsaSetupDialog::registrationUrl().scheme(), QString("https"));
        QVERIFY(MsaSetupDialog::registrationUrl().host().endsWith("microsoft.com"));
        QCOMPARE(MsaSetupDialog::approvalUrl().host(), QString("aka.ms"));
    }

    void saveIsOnlyEnabledForAValidId()
    {
        MsaSetupDialog dialog;
        QLineEdit* input = nullptr;
        for (auto* edit : dialog.findChildren<QLineEdit*>()) {
            if (!edit->isReadOnly()) {
                input = edit;
            }
        }
        QVERIFY(input);
        QPushButton* save = nullptr;
        for (auto* button : dialog.findChild<QDialogButtonBox*>()->buttons()) {
            if (dialog.findChild<QDialogButtonBox*>()->buttonRole(button) == QDialogButtonBox::AcceptRole) {
                save = qobject_cast<QPushButton*>(button);
            }
        }
        QVERIFY(save);

        QVERIFY(!save->isEnabled());
        QVERIFY(dialog.clientId().isEmpty());

        input->setText("not an id");
        QVERIFY(!save->isEnabled());
        QVERIFY(dialog.clientId().isEmpty());

        input->setText(QString("  ") + QString(kId).toUpper() + "  ");
        QVERIFY(save->isEnabled());
        QCOMPARE(dialog.clientId(), QString(kId));

        input->clear();
        QVERIFY(!save->isEnabled());
    }

    void copyButtonsPutTheUrisOnTheClipboard()
    {
        MsaSetupDialog dialog;
        QList<QPushButton*> copyButtons;
        for (auto* button : dialog.findChildren<QPushButton*>()) {
            if (button->text() == MsaSetupDialog::tr("Copy")) {
                copyButtons.append(button);
            }
        }
        QCOMPARE(copyButtons.size(), 2);

        const QStringList uris = MsaSetupDialog::redirectUris();
        copyButtons.at(0)->click();
        QCOMPARE(QGuiApplication::clipboard()->text(), uris.at(0));
        copyButtons.at(1)->click();
        QCOMPARE(QGuiApplication::clipboard()->text(), uris.at(1));
    }
};

// A widget test needs a platform plugin. Default to the offscreen one so the test also runs on machines without a display
// (this main() is renamed to MsaSetup_test by tests/CMakeLists.txt, like the generated one of QTEST_MAIN would be).
int main(int argc, char** argv)
{
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }
    QApplication app(argc, argv);
    MsaSetupTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "MsaSetup_test.moc"
