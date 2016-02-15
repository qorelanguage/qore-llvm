//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 Qore Technologies
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//
//------------------------------------------------------------------------------
///
/// \file
/// \brief Scanner implementation.
///
//------------------------------------------------------------------------------
#include "qore/comp/Scanner.h"
#include "qore/common/Logging.h"

/// \cond IGNORED_BY_DOXYGEN
#define REPORT(diagId)  diagMgr.report(DiagId::diagId, src.getMarkLocation())
/// \endcond

namespace qore {
namespace comp {

const std::unordered_map<std::string, TokenType> Scanner::Keywords{
    { "abstract",       TokenType::KwAbstract },
    { "background",     TokenType::KwBackground },
    { "break",          TokenType::KwBreak },
    { "cast",           TokenType::KwCast },
    { "catch",          TokenType::KwCatch },
    { "chomp",          TokenType::KwChomp },
    { "class",          TokenType::KwClass },
    { "const",          TokenType::KwConst },
    { "continue",       TokenType::KwContinue },
    { "delete",         TokenType::KwDelete },
    { "deprecated",     TokenType::KwDeprecated },
    { "do",             TokenType::KwDo },
    { "elements",       TokenType::KwElements },
    { "else",           TokenType::KwElse },
    { "exists",         TokenType::KwExists },
    { "extract",        TokenType::KwExtract },
    { "False",          TokenType::KwFalse },
    { "final",          TokenType::KwFinal },
    { "foldl",          TokenType::KwFoldl },
    { "foldr",          TokenType::KwFoldr },
    { "for",            TokenType::KwFor },
    { "foreach",        TokenType::KwForeach },
    { "if",             TokenType::KwIf },
    { "in",             TokenType::KwIn },
    { "inherits",       TokenType::KwInherits },
    { "instanceof",     TokenType::KwInstanceof },
    { "keys",           TokenType::KwKeys },
    { "map",            TokenType::KwMap },
    { "new",            TokenType::KwNew },
    { "namespace",      TokenType::KwNamespace },
    { "NOTHING",        TokenType::KwNothing },
    { "NULL",           TokenType::KwNull },
    { "on_error",       TokenType::KwOnError },
    { "on_exit",        TokenType::KwOnExit },
    { "on_success",     TokenType::KwOnSuccess },
    { "our",            TokenType::KwOur },
    { "pop",            TokenType::KwPop },
    { "private",        TokenType::KwPrivate },
    { "public",         TokenType::KwPublic },
    { "push",           TokenType::KwPush },
    { "remove",         TokenType::KwRemove },
    { "rethrow",        TokenType::KwRethrow },
    { "return",         TokenType::KwReturn },
    { "select",         TokenType::KwSelect },
    { "self",           TokenType::KwSelf },
    { "shift",          TokenType::KwShift },
    { "splice",         TokenType::KwSplice },
    { "static",         TokenType::KwStatic },
    { "sub",            TokenType::KwSub },
    { "synchronized",   TokenType::KwSynchronized },
    { "thread_exit",    TokenType::KwThreadExit },
    { "throw",          TokenType::KwThrow },
    { "trim",           TokenType::KwTrim },
    { "True",           TokenType::KwTrue },
    { "try",            TokenType::KwTry },
    { "unshift",        TokenType::KwUnshift },
    { "while",          TokenType::KwWhile },

    { "my",             TokenType::KwMy },
};

Scanner::Scanner(DiagManager &diagMgr) : diagMgr(diagMgr) {
    LOG_FUNCTION();
}

Token Scanner::read(Source &src) {
    LOG_FUNCTION();

    while (true) {
        while (isWhitespace(src.peek()) || isNewline(src.peek())) {
            src.read();
        }

        src.setMark();
        TokenType type = readInternal(src);

        if (type != TokenType::None) {
            LOG("Returning token " << type);
            return Token(type, src.getMarkLocation(), src.getMarkedLength());
        }
    }
}

//TODO binary, number, float, absolute/relative date
TokenType Scanner::readInternal(Source &src) {
    LOG_FUNCTION();
    switch (char c = src.read()) {
        case '\0':
            return TokenType::EndOfFile;
        case '+':
            if (src.peek() == '+') {
                src.read();
                return TokenType::DoublePlus;
            }
            if (src.peek() == '=') {
                src.read();
                return TokenType::PlusEquals;
            }
            return TokenType::Plus;
        case '-':
            if (src.peek() == '-') {
                src.read();
                return TokenType::DoubleMinus;
            }
            if (src.peek() == '=') {
                src.read();
                return TokenType::MinusEquals;
            }
            return TokenType::Minus;
        case ';':
            return TokenType::Semicolon;
        case '{':
            return TokenType::CurlyLeft;
        case '}':
            return TokenType::CurlyRight;
        case '(':
            return TokenType::ParenLeft;
        case ')':
            return TokenType::ParenRight;
        case '[':
            return TokenType::SquareLeft;
        case ']':
            return TokenType::SquareRight;
        case '.':
            return TokenType::Dot;
        case '$':
            if (src.peek() == '$') {
                src.read();
                return TokenType::DoubleDollar;
            }
            if (src.peek() == '#') {
                src.read();
                return TokenType::DollarHash;
            }
            while (isdigit(src.peek())) {
                src.read();
            }
            if (src.getMarkedLength() < 2) {
                REPORT(ScannerInvalidCharacter) << c;
                return TokenType::None;
            }
            return TokenType::ImplicitArg;
        case '^':
            if (src.peek() == '=') {
                src.read();
                return TokenType::CaretEquals;
            }
            return TokenType::Caret;
        case '&':
            if (src.peek() == '&') {
                src.read();
                return TokenType::DoubleAmpersand;
            }
            if (src.peek() == '=') {
                src.read();
                return TokenType::AmpersandEquals;
            }
            return TokenType::Ampersand;
        case '|':
            if (src.peek() == '|') {
                src.read();
                return TokenType::DoubleVerticalBar;
            }
            if (src.peek() == '=') {
                src.read();
                return TokenType::VerticalBarEquals;
            }
            return TokenType::VerticalBar;
        case '=':
            if (src.peek() == '=') {
                src.read();
                if (src.peek() == '=') {
                    src.read();
                    return TokenType::TripleEquals;
                }
                return TokenType::DoubleEquals;
            }
            return TokenType::Equals;
        case ',':
            return TokenType::Comma;
        case '!':
            if (src.peek() == '=') {
                src.read();
                if (src.peek() == '=') {
                    src.read();
                    return TokenType::ExclamationDoubleEquals;
                }
                return TokenType::ExclamationEquals;
            }
            return TokenType::Exclamation;
        case '?':
            if (src.peek() == '?') {
                src.read();
                return TokenType::DoubleQuestion;
            }
            if (src.peek() == '*') {
                src.read();
                return TokenType::QuestionAsterisk;
            }
            return TokenType::Question;
        case '~':
            return TokenType::Tilde;
        case '\\':
            return TokenType::Backslash;
        case '*':
            if (src.peek() == '=') {
                src.read();
                return TokenType::AsteriskEquals;
            }
            return TokenType::Asterisk;
        case '#':
            readLineComment(src);
            return TokenType::None;
        case ':':
            if (src.peek() == ':') {
                src.read();
                return TokenType::DoubleColon;
            }
            return TokenType::Colon;
        case '/':
            if (src.peek() == '*') {
                readBlockComment(src);
                return TokenType::None;
            }
            if (src.peek() == '=') {
                src.read();
                return TokenType::SlashEquals;
            }
            return TokenType::Slash;
        case '%':
            if (src.wasFirstOnLine()) {
                return readParseDirective(src);
            }
            if (src.peek() == '=') {
                src.read();
                return TokenType::PercentEquals;
            }
            return TokenType::Percent;
        case '<':
            if (src.peek() == '<') {
                src.read();
                if (src.peek() == '=') {
                    src.read();
                    return TokenType::DoubleAngleLeftEquals;
                }
                return TokenType::DoubleAngleLeft;
            }
            if (src.peek() == '>') {
                src.read();
                return TokenType::AngleLeftAngleRight;
            }
            if (src.peek() == '=') {
                src.read();
                if (src.peek() == '>') {
                    src.read();
                    return TokenType::AngleLeftEqualsAngleRight;
                }
                return TokenType::AngleLeftEquals;
            }
            return TokenType::AngleLeft;
        case '>':
            if (src.peek() == '>') {
                src.read();
                if (src.peek() == '=') {
                    src.read();
                    return TokenType::DoubleAngleRightEquals;
                }
                return TokenType::DoubleAngleRight;
            }
            if (src.peek() == '=') {
                src.read();
                return TokenType::AngleRightEquals;
            }
            return TokenType::AngleRight;
        case '"':
        case '\'':
            src.unread();
            readString(src);
            return TokenType::String;
        case '`':
            src.unread();
            readString(src);
            return TokenType::BackquotedString;
        case '0':
            return src.peek() == 'x' ? readHexLiteral(src) : handleDigit(src);
        case '1':   case '2':   case '3':   case '4':
        case '5':   case '6':   case '7':   case '8':   case '9':
            return handleDigit(src);
        case 'a':   case 'b':   case 'c':   case 'd':   case 'e':
        case 'f':   case 'g':   case 'h':   case 'i':   case 'j':
        case 'k':   case 'l':   case 'm':   case 'n':   case 'o':
        case 'p':   case 'q':   case 'r':   case 's':   case 't':
        case 'u':   case 'v':   case 'w':   case 'x':   case 'y': case 'z':
        case 'A':   case 'B':   case 'C':   case 'D':   case 'E':
        case 'F':   case 'G':   case 'H':   case 'I':   case 'J':
        case 'K':   case 'L':   case 'M':   case 'N':   case 'O':
        case 'P':   case 'Q':   case 'R':   case 'S':   case 'T':
        case 'U':   case 'V':   case 'W':   case 'X':   case 'Y': case 'Z':
        case '_':
            return readIdentifier(src);
        default:
            REPORT(ScannerInvalidCharacter) << c;
            return TokenType::None;
    }
}

TokenType Scanner::readIdentifier(Source &src) {
    while (isalnum(src.peek()) || src.peek() == '_') {
        src.read();
    }
    auto it = Keywords.find(src.getMarkedString());
    return it == Keywords.end() ? TokenType::Identifier : it->second;
}

void Scanner::readString(Source &src) {
    int type = src.read();
    assert(type == '\'' || type == '"' || type == '`');
    bool escape = false;
    while (true) {
        char c = src.read();
        if (c == '\0') {
            REPORT(ScannerUnendedStringLiteral);
            break;
        }
        if (c == type && !escape) {
            break;
        }
        escape = c == '\\' && type == '"' && !escape;
    }
}

void Scanner::readLineComment(Source &src) {
    while (!isNewline(src.peek()) && src.peek() != '\0') {
        src.read();
    }
}

void Scanner::readBlockComment(Source &src) {
    src.read();         //consume opening asterisk
    bool asterisk = false;
    while (true) {
        int c = src.read();
        if (c == '\0') {
            REPORT(ScannerUnendedBlockComment);
            return;
        }
        if (c == '/' && asterisk) {
            return;
        }
        asterisk = c == '*';
    }
}

TokenType Scanner::readParseDirective(Source &src) {
    while (src.peek() != '\0' && !isNewline(src.peek()) && !isWhitespace(src.peek())) {
        src.read();
    }
    return TokenType::ParseDirective;
}

TokenType Scanner::readHexLiteral(Source &src) {
    src.read(); //consume 'x'
    while (isHexDigit(src.peek())) {
        src.read();
    }
    if (src.getMarkedLength() < 3) {
        REPORT(ScannerInvalidHexLiteral);
    }
    return TokenType::Integer;
}

TokenType Scanner::handleDigit(Source &src) {
    while (isdigit(src.peek())) {
        src.read();
    }
    return TokenType::Integer;
}


} //namespace comp
} //namespace qore
