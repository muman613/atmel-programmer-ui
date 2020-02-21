#include <QCoreApplication>
#include <QDebug>
#include <QString>

enum state {
    STATE_UNDEFINED,
    STATE_INWORD,
    STATE_INSPACE,
    STATE_INQUOTED,
};

/**
 * @brief Split a string using spaces to seperate arguments, quoted strings
 *        are not split. Implemented with a simple state-machine.
 * @param input
 * @param output
 * @return
 */

bool wunderSplit(const QString & input, QStringList & output) {
    QString temp;

    state current_state = STATE_UNDEFINED;

    foreach(QChar uch, input) {
        switch (current_state) {
        case STATE_UNDEFINED:
            if (!uch.isSpace()) {
                if (uch == QChar('"')) {
                    current_state = STATE_INQUOTED;
                } else {
                    temp.append(uch);
                    current_state = STATE_INWORD;
                }
            } else {
                current_state = STATE_INSPACE;
            }
            break;

        case STATE_INWORD:
            if (uch.isSpace()) {
                current_state = STATE_INSPACE;
            } else if (uch == QChar('"')) {
                current_state = STATE_INQUOTED;
            } else {
                temp.append(uch);
            }
            break;

        case STATE_INSPACE:
            if (uch.isSpace()) {
                current_state = STATE_INSPACE;
            } else if (uch == QChar('"')) {
                output.push_back(temp);
                temp.clear();
                current_state = STATE_INQUOTED;
            } else {
                if (!temp.isEmpty()) {
                    output.push_back(temp);
                    temp.clear();
                }
                current_state = STATE_INWORD;
                temp.append(uch);
            }
            break;

        case STATE_INQUOTED:
            if (uch == QChar('"')) {
//              qDebug() << temp;
                current_state = STATE_INWORD;
            } else {
                temp.append(uch);
            }
            break;
        }
    }

    if (!temp.isEmpty())
        output.push_back(temp);

    return !output.isEmpty();
}

