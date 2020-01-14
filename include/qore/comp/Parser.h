//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 - 2020 Qore Technologies, s.r.o.
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
/// \brief Qore language parser.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_PARSER_H_
#define INCLUDE_QORE_COMP_PARSER_H_

#include <string>
#include <vector>
#include "qore/core/util/Util.h"
#include "qore/comp/ast/Script.h"
#include "qore/comp/ast/Class.h"
#include "qore/comp/ast/Expression.h"
#include "qore/comp/Context.h"
#include "qore/comp/DiagManager.h"
#include "qore/comp/Token.h"

namespace qore {
namespace comp {

/**
 * \brief Qore language parser - converts a stream of tokens into an abstract syntax tree.
 */
class Parser {

public:
    /**
     * \brief Constructs the parser for given source of tokens.
     * \param ctx the compiler context
     * \param src the source of tokens
     */
    Parser(Context &ctx, ITokenStream &src) : ctx(ctx), src(src), hasToken(false), recorder(nullptr) {
    }

    /**
     * \brief Parses the script.
     * \return parsed script
     */
    ast::Script::Ptr parseScript();

    /**
     * \brief Parses a top-level statement or declaration.
     * \return parsed statement, declaration, or eof
     */
    class DeclOrStmt parseDeclOrStmt();

private:
    Parser(const Parser &) = delete;
    Parser(Parser &&) = delete;
    Parser &operator=(const Parser &) = delete;
    Parser &operator=(Parser &&) = delete;

private:
    using RecoverStrategy = void (Parser::*)();

    TokenType tokenType() {
        ensureToken();
        return token.type;
    }

    SourceLocation location() {
        ensureToken();
        return token.location;
    }

    Token consume() {
        assert(hasToken);
        hasToken = false;
        LOG("Consuming " << token.type);
        if (recorder) {
            recorder->tokens.push_back(token);
        }
        return token;
    }

    void ensureToken() {
        if (!hasToken) {
            token = src.read(ITokenStream::Mode::Normal);
            hasToken = true;
        }
    }

    Token match(TokenType expected, RecoverStrategy recoverStrategy = &Parser::recoverDoNothing) {
        if (tokenType() == expected) {
            return consume();
        }
        report(DiagId::ParserUnexpectedToken) << util::to_string(token.type) << util::to_string(expected);
        (this->*recoverStrategy)();
        return token;
    }

    DiagBuilder report(DiagId id) {
        assert(hasToken);
        return ctx.report(id, token.location);
    }

    String::Ref strVal() {
        return ctx.getStringTable().put(lexeme());
    }

    std::string lexeme() {
        ensureToken();
        return ctx.getLexeme(token);
    }

    void recoverDoNothing() {}
    void recoverSkipToCurlyRight();
    void recoverSkipToSemicolon();

private:
    class Recorder {
    public:
        explicit Recorder(Parser &parser) : parser(parser) {
            assert(parser.recorder == nullptr);
            parser.recorder = this;
        }

        ~Recorder() {
            assert(parser.recorder == nullptr);
        }

        void stop() {
            assert(parser.recorder == this);
            parser.recorder = nullptr;
        }

        void rewind() {
            assert(parser.recorder == this);
            if (parser.hasToken) {
                parser.consume();
            }
            parser.recorder = nullptr;
            parser.src.unread(tokens.begin(), tokens.end());
        }

        Parser &parser;
        std::vector<Token> tokens;
    };

private:
    ast::Namespace::Ptr namespaceDecl(ast::Modifiers mods);
    ast::Declaration::Ptr namespaceMember(bool topLevel);
    ast::Modifiers modifiers();
    ast::Class::Ptr classDecl(ast::Modifiers mods);
    ast::Declaration::Ptr classMember();
    ast::Declaration::Ptr classMemberList(ast::Modifiers mods);
    ast::Statement::Ptr statement();
    ast::ExpressionStatement::Ptr expressionStmt();
    ast::CompoundStatement::Ptr compoundStmt();
    ast::ReturnStatement::Ptr returnStmt();
    ast::IfStatement::Ptr ifStmt();
    ast::TryStatement::Ptr tryStmt();
    ast::ForeachStatement::Ptr foreachStmt();
    ast::ThrowStatement::Ptr throwStmt();
    ast::WhileStatement::Ptr whileStmt();
    ast::DoWhileStatement::Ptr doWhileStmt();
    ast::ForStatement::Ptr forStmt();
    ast::SwitchStatement::Ptr switchStmt();
    ast::Expression::Ptr expression();
    ast::Expression::Ptr assignmentExpr();
    ast::Expression::Ptr condExpr();
    ast::Expression::Ptr coalescingExpr();
    ast::Expression::Ptr logOrExpr();
    ast::Expression::Ptr logAndExpr();
    ast::Expression::Ptr orExpr();
    ast::Expression::Ptr xorExpr();
    ast::Expression::Ptr andExpr();
    ast::Expression::Ptr relationalExpr();
    ast::Expression::Ptr instanceofExpr();
    ast::Expression::Ptr shiftExpr();
    ast::Expression::Ptr addExpr();
    ast::Expression::Ptr multExpr();
    ast::Expression::Ptr prefixExpr();
    ast::Expression::Ptr postfixExpr();
    ast::Expression::Ptr primaryExpr();
    ast::Expression::Ptr hash(Token openToken, ast::Expression::Ptr expr);
    ast::Expression::Ptr list(Token openToken, ast::Expression::Ptr expr);
    ast::ClosureExpression::Ptr closure(ast::Modifiers mods, ast::Type type);
    std::vector<ast::Routine::Param> paramList();
    ast::Name name();
    ast::Type type();
    ast::Routine::Ptr routine(ast::Modifiers mods, ast::Type type, bool method = false);
    ast::ArgList::Ptr argList();
    ast::Constant::Ptr constant(ast::Modifiers mods);
    ast::VarDeclExpression::Ptr varDecl(ast::Type type);

private:
    Context &ctx;
    TokenQueue src;
    Token token;
    bool hasToken;
    Recorder *recorder;
};

/**
 * \brief Helper class for returning either a declaration or a statement from the parser.
 */
class DeclOrStmt {

public:
    /**
     * \brief Constructs an instance that represents eof.
     */
    DeclOrStmt() : decl(false) {
    }

    /**
     * \brief Constructs an instance that represents a declaration.
     * \param decl the declaration
     */
    explicit DeclOrStmt(ast::Declaration::Ptr decl) : node(std::move(decl)), decl(true) {
    }

    /**
     * \brief Constructs an instance that represents a statement.
     * \param stmt the statement
     */
    explicit DeclOrStmt(ast::Statement::Ptr stmt) : node(std::move(stmt)), decl(false) {
    }

    /**
     * \brief Returns true if this instance represents a declaration.
     * \return true if this instance represents a declaration
     */
    bool isDeclaration() const {
        return node && decl;
    }

    /**
     * \brief Returns true if this instance represents a statement.
     * \return true if this instance represents a statement
     */
    bool isStatement() const {
        return node && !decl;
    }

    /**
     * \brief Returns the declaration represented by this instance.
     * \return the declaration represented by this instance
     */
    ast::Declaration &getDeclaration() {
        assert(isDeclaration());
        return static_cast<ast::Declaration &>(*node);
    }

    /**
     * \brief Returns the statement represented by this instance.
     * \return the statement represented by this instance
     */
    ast::Statement &getStatement() {
        assert(isStatement());
        return static_cast<ast::Statement &>(*node);
    }

private:
    ast::Node::Ptr node;
    bool decl;
};

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_PARSER_H_
