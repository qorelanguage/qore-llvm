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

DIAG(CompScriptFileIoError,                     "PARSE-EXCEPTION", Error,
        "error reading file %s")
DIAG(CompNulCharactersIgnored,                  "PARSE-WARNING", Warning,
        "null character(s) ignored")

DIAG(PdpUnknownDirective,                       "PARSE-EXCEPTION", Error,
        "unknown parse directive %s")
DIAG(PdpUnexpectedArgument,                     "PARSE-EXCEPTION", Error,
        "unexpected argument of parse directive %%s")
DIAG(PdpMissingArgument,                        "PARSE-EXCEPTION", Error,
        "missing argument of parse directive %%s")

DIAG(ScannerUnendedBlockComment,                "PARSE-EXCEPTION", Error,
        "open block comment at the end of file")
DIAG(ScannerInvalidCharacter,                   "PARSE-EXCEPTION", Error,
        "unexpected '%s'")
DIAG(ScannerInvalidInteger,                     "PARSE-EXCEPTION", Error,
        "numeric overflow in integer '%s': cannot represent in a 64-bit signed number; "
        "please use a floating-point or arbitrary-precision number instead (ex: '%sn'); "
        "integers must be in the range %s (MININT) to %s (MAXINT)")
DIAG(ScannerUnendedStringLiteral,               "PARSE-EXCEPTION", Error,
        "unended string literal")
DIAG(ScannerInvalidHexLiteral,                  "PARSE-EXCEPTION", Error,
        "invalid hexadecimal literal")
DIAG(ScannerExpectedRegularExpression,          "PARSE-EXCEPTION", Error,
        "expected regular expression")

DIAG(ParserUnexpectedToken,                     "PARSE-EXCEPTION", Error,
        "syntax error, unexpected %s, expecting %s")
DIAG(ParserExpectedPrimaryExpression,           "PARSE-EXCEPTION", Error,
        "unexpected %s, expecting primary expression")
DIAG(ParserExpectedVariableName,                "PARSE-EXCEPTION", Error,
        "unexpected %s, expecting variable name")
DIAG(ParserModifierGivenTwice,                  "PARSE-EXCEPTION", Error,
        "modifier '%s' given twice")
DIAG(ParserExpectedName,                        "PARSE-EXCEPTION", Error,
        "expected name")
DIAG(ParserExpectedNamespaceMember,             "PARSE-EXCEPTION", Error,
        "expected namespace member")
DIAG(ParserUnendedNamespaceDecl,                "PARSE-EXCEPTION", Error,
        "unended namespace declaration")
DIAG(ParserInvalidMemberAccess,                 "PARSE-EXCEPTION", Error,
        "syntax error, unexpected %s, expecting identifier or '('")
DIAG(ParserExpectedClassMember,                 "PARSE-EXCEPTION", Error,
        "expected class member")
DIAG(ParserExpectedCaseOrDefault,               "PARSE-EXCEPTION", Error,
        "expected 'case' or 'default'")
DIAG(ParserModuleIgnored,                       "PARSE-WARNING", Warning,
        "module declaration ignored")

DIAG(SemaInvalidNamespaceMemberName,            "PARSE-EXCEPTION", Error,
        "invalid namespace member name")
DIAG(SemaNamespaceNotFound,                     "PARSE-EXCEPTION", Error,
        "namespace %s not found in namespace %s")
DIAG(SemaDuplicateNamespaceName,                "PARSE-EXCEPTION", Error,
        "cannot declare namespace %s in %s because a class with the same name already exists")
DIAG(SemaDuplicateClassName,                    "PARSE-EXCEPTION", Error,
        "cannot declare class %s in %s because a class or namespace with the same name already exists")

DIAG(SemaPreviousDeclaration,                   "PARSE-EXCEPTION", Info,
        "previous declaration here")
