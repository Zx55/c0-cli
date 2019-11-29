/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#include "lexer.h"

#define _pos_end { (_ptr.first), ((_ptr.second) + 1) }
#define _lex_token(_type) { Token(_type, _ss.str(), pos, _pos_end), {}, {} }
#define _lex_err(_type) { {}, C0Err(_type, pos, _pos_end), {} }
#define _lex_wrn(_token, _type) { _token, {}, C0Err(_type, pos, _pos_end) }
#define _eof '\0'

namespace cc0 {
    inline void Lexer::_get() {
        if (_ch == _eof)
            return;

        if (_ptr.first >= static_cast<int64_t >(_in.size())
            || _ptr.second >= static_cast<int64_t>(_in[_ptr.first].size())) {
            // some errors happen
            _ch = _eof;
            return;
        }

        if (_ch == '\n' && _ptr.first == static_cast<int64_t>(_in.size()) - 1) {
            _ch = _eof;
            return;
        }

        if (_ch == '\n') {
            ++_ptr.first;
            _ptr.second = 0;
        } else
            ++_ptr.second;
        _ch = _in[_ptr.first][_ptr.second];
    }

    inline void Lexer::_unget() {
        if (_ch == '\0')
            return;

        if (!_ptr.second) {
            // return previous line
            --_ptr.first;
            _ptr.second = _in[_ptr.first].size() - 1;
        } else
            --_ptr.second;
        _ch = _in[_ptr.first][_ptr.second];
    }

    inline Lexer::_ParseResult Lexer::_parse_int(pos_t pos, int base = 10) const {
        auto type = (base == 10) ? TokenType::DECIMAL : TokenType::HEXADECIMAL;

        try {
            auto res = std::stoi(_ss.str(), nullptr, base);
            return { Token(type, res, pos, _pos_end), {}, {} };
        } catch (const std::out_of_range &) {
            return _lex_wrn(Token(type, 0, pos, _pos_end), ErrCode::WrnInt32Overflow);
        }
    }

    inline Lexer::_ParseResult Lexer::_parse_float(pos_t pos) const {
        try {
            auto res = std::stod(_ss.str());
            return { Token(TokenType::FLOAT, res, pos, _pos_end), {}, {} };
        } catch (const std::out_of_range &) {
            return _lex_wrn(Token(TokenType::FLOAT, 0., pos, _pos_end), ErrCode::WrnFloat64Overflow);
        }
    }

    Lexer::_ParseResult Lexer::_next() {
        using namespace utils;
        
        auto current = DFAState::INIT;
        pos_t pos;
        _ss.clear();
        _ss.str("");

        while (true) {
            L_START:
            switch(_get(); current) {
                case DFAState::INIT: {
                    if (iseof(_ch))
                        return _lex_err(ErrCode::ErrEOF);
                    if (!isaccept(_ch))
                        return _lex_err(ErrCode::ErrInvalidInput);

                    if (isspace(_ch))
                        goto L_START;
                    else if (_ch == '0')
                        current = DFAState::ZERO;
                    else if (isdigit(_ch))
                        current = DFAState::DECIMAL;
                    else if (isalpha(_ch))
                        current = DFAState::IDENTIFIER;
                    else {
                        switch(_ch) {
                            case '+':
                                current = DFAState::PLUS;
                                break;
                            case '-':
                                current = DFAState::MINUS;
                                break;
                            case '*':
                                current = DFAState::MULTIPLY;
                                break;
                            case '/':
                                current = DFAState::DIVISION;
                                break;
                            case '=':
                                current = DFAState::ASSIGN;
                                break;
                            case '>':
                                current = DFAState::GREATER;
                                break;
                            case '<':
                                current = DFAState::LESS;
                                break;
                            case '!':
                                current = DFAState::NEQUAL;
                                break;
                            case ';':
                                current = DFAState::SEMI;
                                break;
                            case ':':
                                current = DFAState::COLON;
                                break;
                            case ',':
                                current = DFAState::COMMA;
                                break;
                            case '(':
                                current = DFAState::LPARENTHESIS;
                                break;
                            case ')':
                                current = DFAState::RPARENTHESIS;
                                break;
                            case '{':
                                current = DFAState::LBRACE;
                                break;
                            case '}':
                                current = DFAState::RBRACE;
                                break;
                            // <float-literal> ::= '.'<digit-seq>[<exp>]
                            case '.': {
                                if (pos = _ptr, _ss << _ch, _get(); isdigit(_ch)) {
                                    _ss << _ch;
                                    current = DFAState::FLOAT_DOT;
                                    goto L_START;
                                }
                                return _lex_err(ErrCode::ErrInvalidFloat);
                            }
                            case '\'':
                                current = DFAState::CHAR;
                                break;
                            case '\"':
                                current = DFAState::STRING;
                                break;
                            }
                    }

                    _ss << _ch;
                    pos = _ptr;
                    break;
                }
                case DFAState::ZERO: {
                    switch (_ch) {
                        // <hex-literal> ::= '0'('x'|'X')<hex-digit-seq>
                        case 'x':
                            [[fallthrough]];
                        case 'X': {
                            current = DFAState::HEXADECIMAL;
                            break;
                        }
                        // <float-literal> :: '0'<exp>
                        case 'e':
                            [[fallthrough]];
                        case 'E': {
                            current = DFAState::FLOAT_EXP;
                            break;
                        }
                        // <float-liter> ::= '0.'[<digit-seq>][<exp>]
                        case '.':
                            current = DFAState::FLOAT_DOT;
                            break;
                        // <digit> ::= '0'
                        default:
                            _unget();
                            return { Token(TokenType::DECIMAL, 0, pos, _pos_end), {}, {} };
                    }

                    _ss << _ch;
                    break;
                }
                case DFAState::DECIMAL: {
                    // <dem-literal> ::= <nozero-digit>{<digit>}
                    while (isdigit(_ch)) {
                        _ss << _ch;
                        _get();
                    }

                    switch (_ch) {
                        // <float-literal> ::= <digit-seq>'.'[<digit-seq>][<exp>]
                        case '.':
                            current = DFAState::FLOAT_DOT;
                            break;
                        // <float-literal> ::= <digit-seq><exp>
                        case 'e':
                            [[fallthrough]];
                        case 'E': {
                            current = DFAState::FLOAT_EXP;
                            break;
                        }
                        default: {
                            _unget();
                            return _parse_int(pos);
                        }
                    }

                    _ss << _ch;
                    break;
                }
                case DFAState::IDENTIFIER: {
                    // <identifier> ::= <alpha>{<alnum>}
                    while (isalnum(_ch)) {
                        _ss << _ch;
                        _get();
                    }
                    _unget();

                    auto id_str = _ss.str();
                    if (auto res = Token::get_reserved(id_str); res.has_value())
                        return { Token(res.value(), id_str, pos, _pos_end), {}, {} };
                    else
                        return _lex_token(TokenType::IDENTIFIER);
                }
                case DFAState::HEXADECIMAL: {
                    // <hex-literal> ::= '0'('x'|'X')<hex-digit-seq>
                    if (!ishex(_ch))
                        return _lex_err(ErrCode::ErrInvalidHexadecimal);

                    do {
                        _ss << _ch;
                        _get();
                    } while (ishex(_ch));
                    _unget();
                    return _parse_int(pos, 16);
                }
                case DFAState::FLOAT_DOT: {
                    while (isdigit(_ch)) {
                        _ss << _ch;
                        _get();
                    }

                    switch (_ch) {
                        // <float-literal> ::= ... '.'[<digit-seq>]('e'|'E')<exp>
                        case 'e':
                            [[fallthrough]];
                        case 'E': {
                            _ss << _ch;
                            current = DFAState::FLOAT_EXP;
                            break;
                        }
                        default: {
                            _unget();
                            return _parse_float(pos);
                        }
                    }
                    break;
                }
                case DFAState::FLOAT_EXP: {
                    // <float-literal> ::= ... ('e'|'E')[('+'|'-')]<digit-seq>
                    if (_ch == '+' || _ch == '-')
                        _ss << _ch;
                    while (isdigit(_ch)) {
                        _ss << _ch;
                        _get();
                    }
                    _unget();
                    return _parse_float(pos);
                }
                case DFAState::PLUS:
                    _unget();
                    return _lex_token(TokenType::PLUS);
                case DFAState::MINUS:
                    _unget();
                    return _lex_token(TokenType::MINUS);
                case DFAState::MULTIPLY:
                    _unget();
                    return _lex_token(TokenType::MULTIPLY);
                case DFAState::DIVISION: {
                    if (_ch == '/')
                        current = DFAState::SINGLE_COMMENT;
                    else if (_ch == '*')
                        current = DFAState::MULTI_COMMENT;
                    else {
                        _unget();
                        return _lex_token(TokenType::DIVISION);
                    }

                    _ss << _ch;
                    break;
                }
                case DFAState::SINGLE_COMMENT: {
                    _ss.str("");

                    while (!isline(_ch) && !iseof(_ch)) {
                        _ss << _ch;
                        _get();
                    }
                    _unget();
                    return _lex_token(TokenType::COMMENT);
                }
                case DFAState::MULTI_COMMENT: {
                    _ss.str("");

                    while (true) {
                        if (iseof(_ch))
                            return _lex_err(ErrCode::ErrIncompleteComment);

                        if (_ch == '*') {
                            if (_get(); _ch == '/')
                                return _lex_token(TokenType::COMMENT);
                            else
                                _ss << '*';
                        } else {
                            _ss << _ch;
                            _get();
                        }
                    }
                }
                case DFAState::ASSIGN: {
                    if (_ch == '=') {
                        _ss << _ch;
                        return _lex_token(TokenType::EQUAL);
                    }
                    _unget();
                    return _lex_token(TokenType::ASSIGN);
                }
                case DFAState::GREATER: {
                    if (_ch == '=') {
                        _ss << _ch;
                        return _lex_token(TokenType::GREATER_EQUAL);
                    }
                    _unget();
                    return _lex_token(TokenType::GREATER);
                }
                case DFAState::LESS: {
                    if (_ch == '=') {
                        _ss << _ch;
                        return _lex_token(TokenType::LESS_EQUAL);
                    }
                    _unget();
                    return _lex_token(TokenType::LESS);
                }
                case DFAState::NEQUAL: {
                    if (_ch == '=') {
                        _ss << _ch;
                        return _lex_token(TokenType::NEQUAL);
                    }
                    _unget();
                    return _lex_err(ErrCode::ErrMissEqOp);
                }
                case DFAState::SEMI:
                    _unget();
                    return _lex_token(TokenType::SEMI);
                case DFAState::COLON:
                    _unget();
                    return _lex_token(TokenType::COLON);
                case DFAState::COMMA:
                    _unget();
                    return _lex_token(TokenType::COMMA);
                case DFAState::LPARENTHESIS:
                    _unget();
                    return _lex_token(TokenType::LPARENTHESIS);
                case DFAState::RPARENTHESIS:
                    _unget();
                    return _lex_token(TokenType::RPARENTHESIS);
                case DFAState::LBRACE:
                    _unget();
                    return _lex_token(TokenType::LBRACE);
                case DFAState::RBRACE:
                    _unget();
                    return _lex_token(TokenType::RBRACE);
                case DFAState::CHAR: {
                    _ss.str("");

                    if (isc_char(_ch)) {
                        _ss << _ch;
                        if (_get(); _ch == '\'')
                            return _lex_token(TokenType::CHAR_LITERAL);
                        _unget();
                        return _lex_err(ErrCode::ErrMissQuote);
                    } else if (_ch == '\\') {
                        _ss << _ch;
                        if (_get(); ise_char(_ch)) {
                            _ss << _ch;
                            if (_get(); _ch == '\'')
                                return _lex_token(TokenType::CHAR_LITERAL);
                            _unget();
                            return _lex_err(ErrCode::ErrMissQuote);
                        } else if (_ch == 'x') {
                            _ss << _ch;
                            if (_get(); ishex(_ch)) {
                                _ss << _ch;
                                if (_get(); ishex(_ch))
                                    _ss << _ch;
                                else
                                    _unget();

                                if (_get(); _ch == '\'')
                                    return _lex_token(TokenType::CHAR_LITERAL);
                                else
                                    return _lex_err(ErrCode::ErrMissQuote);
                            } else {
                                _unget();
                                return _lex_err(ErrCode::ErrInvalidEscape);
                            }
                        } else {
                            _unget();
                            return _lex_err(ErrCode::ErrInvalidEscape);
                        }
                    } else {
                        _unget();
                        return _lex_err(ErrCode::ErrInvalidChar);
                    }
                }
                case DFAState::STRING: {
                    _ss.str("");

                    while (true) {
                        if (iss_char(_ch))
                            _ss << _ch;
                        else if (_ch == '\\') {
                            _ss << _ch;
                            if (_get(); ise_char(_ch))
                                _ss << _ch;
                            else if (_ch == 'x') {
                                _ss << _ch;
                                if (_get(); ishex(_ch)) {
                                    _ss << _ch;
                                    if (_get(); ishex(_ch))
                                        _ss << _ch;
                                    else
                                        _unget();
                                } else
                                    return _lex_err(ErrCode::ErrInvalidEscape);
                            } else {
                                _unget();
                                return _lex_err(ErrCode::ErrInvalidEscape);
                            }
                        } else if (_ch == '\"') {
                            if (_ss.str().empty())
                                return _lex_err(ErrCode::ErrInvalidString);
                            else
                                return _lex_token(TokenType::STRING_LITERAL);
                        } else {
                            _unget();
                            return _lex_err(ErrCode::ErrMissQuote);
                        }

                        _get();
                    }
                }
            }
        }
    }

    void Lexer::next_token() {
        const auto [tk, err, wrn] = _next();
        if (err.has_value()) {
            if (err.value().get_code() != ErrCode::ErrEOF)
                RuntimeContext::put_fatal(err.value());
            return;
        } else if (wrn.has_value())
            RuntimeContext::put_wrn(wrn.value());
        RuntimeContext::put_token(tk.value());
    }

    void Lexer::all_tokens() {
        auto tks = std::vector<Token>();
        auto errs = std::vector<C0Err>();
        auto wrns = std::vector<C0Err>();

        while (true) {
            if (const auto [tk, err, wrn] = _next(); err.has_value()) {
                if (err.value().get_code() == ErrCode::ErrEOF) {
                    RuntimeContext::set_tokens(std::move(tks));
                    RuntimeContext::put_fatals(std::move(errs));
                    RuntimeContext::put_wrns(std::move(wrns));
                    break;
                }
                else
                    errs.push_back(err.value());
            } else {
                if (wrn.has_value())
                    wrns.push_back(wrn.value());
                tks.push_back(tk.value());
            }
        }
    }
}
